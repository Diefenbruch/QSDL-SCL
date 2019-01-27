/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|SCProcedureList| SCProcedureList.cc| 10. Apr 1997  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     16.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCProcedureList.h"
#include "SCProcedure.h"
#include "SCTimerControl.h"
#include "SCMachine.h"
#include "SCMachineLtd.h"
#include "SCProcedureType.h"
#include "SCScheduler.h"
#include "SCDebug.h"
#include "SCListSave.h"
#include "SCMem.h"

#if _SC_VALIDATION_OPTIMIZE >= 3
  #include "SCIndetVal.h"
  #include "SCStack.h"
  #include "SCStackElem.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_PROFILING
extern int save_ineff;
extern int restore_ineff;
extern int size_ineff;
#endif


SCBoolean SCProcedureList::Save(SCMem& saveArea) const
{
  SCProcedure *     procedure;
  SCProcedureCons * ptr;
  SCRunnableID      procedureID;
  SCProcedureType * procedureType;
#if _SC_VALIDATION_OPTIMIZE
  SCBoolean         procedure_modified;

#if _SC_VALIDATION_OPTIMIZE >= 3
  SCMem *           lastState;
  SCStackElem *     lastStackElem;
  size_t            lastOffset, lastHistoryOffset;
#endif

#endif

  //
  // First, we write the number of procedures in this queue:
  //  
  saveArea.HistoryStore(&numOfElems, sizeof(SCNatural));

  if (numOfElems == 0)
    return true;

  // The following loop writes all procedures which are in the current
  // queue, in the given saveArea:
  
  for (ptr = head; ptr; ptr = ptr->Next())
  {
    procedure = (*ptr)();
    assert(procedure);

    // Bei Prozeduren ist kein Verlass auf die Pointer,
    // daher RunnableID + ProcedureType speichern:
    procedureID = procedure->GetID();
    saveArea.HistoryStore(&procedureID, sizeof(SCRunnableID));
    procedureType = procedure->GetType();
    saveArea.HistoryStore(&procedureType, sizeof(SCProcedureType *));

#if _SC_VALIDATION_OPTIMIZE

    // Erste Optimierungsstufe: Testen ob sich die verschiedenen
    // procedures geaendert haben. Diese Optimierung dient der
    // Beschleunigung der Restore()-Methode. Procedures die sich
    // gegenueber dem vorangegangenen Systemzustand nicht geaendert
    // haben koennen beim Restore uebersprungen werden!
    // Jedes Procedure-Objekt erhaelt dazu ein Modified-Flag.
    // Dieses Flag wird vor jedem Validationsschritt vom
    // Validator auf false zurueckgesetzt (siehe SCIndetVal).
    // Das Flag wird als zusaetzliche Infomation VOR jedes Procedure
    // in die saveArea geschrieben (s.u.). Es sollte NICHT in der
    // Save()-Methode des Procedures gesichert werden!
    // Beim Backtracking muss das Flag daher explizit in der
    // Restore-Methode zurueckgesetzt werden.

    procedure_modified = procedure->IsModified();
    saveArea.HistoryStore(&procedure_modified, sizeof(SCBoolean));

#endif  //  _SC_VALIDATION_OPTIMIZE
 
#if _SC_VALIDATION_OPTIMIZE >= 3

    // Dritte Optimierungsstufe: Bei Procedures die sich gegenueber dem letzten
    // Systemzustand nicht geaendert haben, koennen wir das entsprechende
    // saveArea-Segment aus dem Vorgaengerzustand wiederverwenden.
    // Dadurch brauchen wir nicht die zeitintensive Save-Methode
    // fuer das Procedure aufrufen, sondern koennen die sehr
    // effiziente Append-Methode von SCMem verwenden. Dabei
    // wird der Speicherbereich in dem das Procedure im Vorgaengerzustand
    // gespeichert ist, an die aktuelle Position der saveArea kopiert.

    // letztes Stackelement (mit letztem Systemzustand) holen:
    
    if (!procedure_modified &&  // Optimierung moeglich ?
        (lastStackElem = ((SCIndetVal *)SCScheduler::GetIndet())->GetStack()->Top()) != NULL) 
    {
      lastState = lastStackElem->GetState();
      assert(lastState);

      // Alte Offsets des Procedures muessen hier gemerkt werden,
      // da sie unten zunaechst neu gesetzt werden, bevor
      // die alten Werte benoetigt werden:
      lastOffset = procedure->GetLastOffset();
      lastHistoryOffset = procedure->GetLastHistoryOffset();

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcedureList::Save(): using last stack elem for " << *procedure << endl;
      scValidationDebugLog << "SCProcedureList::Save(): at offset = " << lastOffset << endl;
      scValidationDebugLog << "SCProcedureList::Save(): at history offset = " << lastHistoryOffset << endl;
      scValidationDebugLog << "SCProcedureList::Save(): with size = " << procedure->currentSize << endl;
      scValidationDebugLog << "SCProcedureList::Save(): with history size = " << procedure->currentHistorySize << endl;
#endif

      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      procedure->SetLastOffset(saveArea.GetOffset());
      procedure->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcedureList::Save(): setting new offset for " << *procedure;
      scValidationDebugLog << " on " << procedure->GetLastOffset() << endl;
      scValidationDebugLog << "SCProcedureList::Save(): setting new history offset for " << *procedure;
      scValidationDebugLog << " on " << procedure->GetLastHistoryOffset() << endl;
#endif

      saveArea.Append(*lastState,
                      lastOffset, procedure->currentSize,
                      lastHistoryOffset, procedure->currentHistorySize);

    }
    else
    {
      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      procedure->SetLastOffset(saveArea.GetOffset());
      procedure->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcedureList::Save(): setting new offset for " << *procedure;
      scValidationDebugLog << " on " << procedure->GetLastOffset() << endl;
      scValidationDebugLog << "SCProcedureList::Save(): setting new history offset for " << *procedure;
      scValidationDebugLog << " on " << procedure->GetLastHistoryOffset() << endl;
#endif

      procedure->Save(saveArea);
    }
#else
    procedure->Save(saveArea);
#endif

#if _SC_VALIDATION_OPTIMIZE
#if _SC_VALIDATION_DEBUG >= 2
    if (procedure->IsModified())
    {
      scValidationDebugLog << "SCProcedureList::Save(): saved changed " << *procedure;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << endl;
    }
    else
    {      
      scValidationDebugLog << "SCProcedureList::Save(): saved unchanged " << *procedure;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << endl;
    }
#endif  //  _SC_VALIDATION_OPTIMIZE

#else // !_SC_VALIDATION_OPTIMIZE

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCProcedureList::Save(): saved " << *procedure;
    scValidationDebugLog << ", offset now " << saveArea.GetOffset();
    scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif

#endif // _SC_VALIDATION_OPTIMIZE

#if _SC_PROFILING
    save_ineff++;
#endif

  } // for (...)
  return true;
}


SCBoolean SCProcedureList::Restore(SCMem &saveArea)
{
  SCProcedure      *procedure = NULL;
  SCNatural       i, number;
  SCRunnableID     procedureID;
  SCProcedureType  *procedureType;
  SCProcedureCons  *newCons;
#if _SC_VALIDATION_OPTIMIZE
  SCBoolean procedure_modified;

#if _SC_VALIDATION_OPTIMIZE >= 3
  size_t lastOffset, lastHistoryOffset;
#endif

#endif

  RemoveAllElements();
  
  saveArea.HistoryRestore(&number, sizeof(SCNatural)); // nicht in numOfElems einlesen!

  if (number == 0)
    return true;

  for (i = 0; i < number; i++)
  {
    // Bei Prozessen zunaechst ProcedureID holen; falls
    // zugehoeriger Prozess noch existiert Zeiger holen;
    // falls Prozess nicht mehr existiert neu anlegen:
    saveArea.HistoryRestore(&procedureID, sizeof(SCRunnableID));
    saveArea.HistoryRestore(&procedureType, sizeof(SCProcedureType *));

#if _SC_VALIDATION_OPTIMIZE
    saveArea.HistoryRestore(&procedure_modified, sizeof(SCBoolean));
#endif

    procedure = (SCProcedure *)SCScheduler::GetRunnableFromID(procedureID);

#if _SC_VALIDATION_OPTIMIZE >= 3
    lastOffset =  saveArea.GetOffset();
    lastHistoryOffset = saveArea.GetHistoryOffset();
#endif

    if (procedure)  // procedure living?
    {
#if _SC_VALIDATION_OPTIMIZE >= 3
      procedure->SetLastOffset(lastOffset);
      procedure->SetLastHistoryOffset(lastHistoryOffset);
#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcedureList::Restore(): setting new offset for " << *procedure;
      scValidationDebugLog << " on " << lastOffset << endl;
      scValidationDebugLog << "SCProcedureList::Restore(): setting new history offset for " << *procedure;
      scValidationDebugLog << " on " << lastHistoryOffset << endl;
#endif
#endif

#if _SC_VALIDATION_OPTIMIZE
      if (procedure->IsModified())
      {
        procedure->Restore(saveArea);
#if _SC_VALIDATION_DEBUG >= 2
        scValidationDebugLog << "SCProcedureList::Restore(): restored " << *procedure;
        scValidationDebugLog << ", offset now " << saveArea.GetOffset();
        scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif
      }
      else // procedure is NOT modified!
      {
        saveArea.SkipOffset(procedure->currentSize);
        saveArea.SkipHistoryOffset(procedure->currentHistorySize);
#if _SC_VALIDATION_DEBUG >= 2
        scValidationDebugLog << "SCProcedureList::Restore(): skipped unchanged " << *procedure;
        scValidationDebugLog << ", offset now " << saveArea.GetOffset() << "(+" << procedure->currentSize << ")";
        scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << "(+" << procedure->currentHistorySize << ")" << endl;
#endif
      }
#else // !_SC_VALIDATION_OPTIMIZE
      procedure->Restore(saveArea);
#endif // _SC_VALIDATION_OPTIMIZE

      // the calculated enabled transition are no longer valid
      // if we do a backtracking:
      if (((SCProcedure *)procedure)->GetEnabledTransitions())
      {
        delete ((SCProcedure *)procedure)->RetrieveEnabledTransitions();
      }
    }
    else           // procedure stopped -> recreation!
    {
      procedure = procedureType->NewInstance(saveArea);
      assert(procedure);
#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCProcedureList::Restore(): recreated " << *procedure;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
      procedure->SetLastOffset(lastOffset);
      procedure->SetLastHistoryOffset(lastHistoryOffset);
#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcedureList::Restore(): setting new offset for " << *procedure;
      scValidationDebugLog << " on " << lastOffset << endl;
      scValidationDebugLog << "SCProcedureList::Restore(): setting new history offset for " << *procedure;
      scValidationDebugLog << " on " << lastHistoryOffset << endl;
#endif
#endif
    }

#if _SC_VALIDATION_OPTIMIZE

    //
    // Explicit restore of both modified flags:
    //

    if (procedure_modified)      // this MUST be done, because validator resets
      procedure->SetModified();  // the modified flags before every step!
    else
      procedure->UnsetModified();

#endif // _SC_VALIDATION_OPTIMIZE

    newCons = InsertAfter(procedure); // inkrementiert numOfElems!
    procedure->SetParent(this);
    procedure->SetContainer(newCons);

#if _SC_PROFILING
    restore_ineff++;
#endif
  }
  assert(numOfElems == number);

  return true;
}


void SCProcedureList::Size(SCSize *curSize) const
{
  SCProcedure *procedure;
  SCProcedureCons *ptr;

  curSize->historySize += sizeof(SCNatural); // Speicher fuer Elementszaehler

  for (ptr = head; ptr; ptr = ptr->Next())
  {
    procedure = (*ptr)();
    assert(procedure);

    curSize->historySize += sizeof(SCRunnableID);
    curSize->historySize += sizeof(SCProcedureType *);

#if _SC_VALIDATION_OPTIMIZE
    curSize->historySize += sizeof(SCBoolean);     // modified (Procedure)

#endif

#if _SC_VALIDATION_OPTIMIZE
    if (!procedure->HasVariableSize())
    {
      curSize->size += procedure->GetCurrentSize();
      curSize->historySize += procedure->GetCurrentHistorySize();
    }
    else
    {
      procedure->Size(curSize);
    }
#else
    procedure->Size(curSize);
#endif

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCProcedureList::Size(): added " << *procedure;
#if _SC_VALIDATION_OPTIMIZE
    scValidationDebugLog << ", size now " << curSize->size << "(+" << procedure->GetCurrentSize() << ")";
    scValidationDebugLog << ", hist size now " << curSize->historySize << "(+" << procedure->GetCurrentHistorySize() << ")";
#endif
    scValidationDebugLog << endl;
#endif

#if _SC_PROFILING
    size_ineff++;
#endif
  }
}
