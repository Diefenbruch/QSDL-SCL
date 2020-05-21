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
| SCProcessList | SCProcessList.cc  | 10. Apr 1997  |   SCL             |      |
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

#include <stdlib.h>

#include "SCProcessList.h"
#include "SCProcess.h"
#include "SCTimerControl.h"
#include "SCMachine.h"
#include "SCMachineLtd.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCDebug.h"
#include "SCListSave.h"
#include "SCMem.h"
#include "SCStream.h"

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


SCProcessList::SCProcessList(SCBoolean pDeleteElems,
                             const SCObject* father) :
  SCList<SCProcess>(pDeleteElems, father)
{
}


SCProcessList::~SCProcessList(void)
{
}


SCBoolean SCProcessList::Save(SCMem& saveArea) const
{
  SCProcess *     process;
  SCProcessCons * ptr;
  SCProcessID     processID;
  SCProcessType * processType;
#if _SC_VALIDATION_OPTIMIZE
  SCBoolean       process_modified;

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus   process_input_queue_modified;
  SCQueueStatus   process_timer_queue_modified;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
  SCMem *         lastState;
  SCStackElem *   lastStackElem;
  size_t          lastOffset, lastHistoryOffset;
#endif

#endif

  //
  // First, we write the number of processs in this queue:
  //  
  saveArea.HistoryStore(&numOfElems, sizeof(SCNatural));

  if (numOfElems == 0)
    return true;

  // The following loop writes all processs which are in the current
  // queue, in the given saveArea:
  
  for (ptr = head; ptr; ptr = ptr->Next())
  {
    process = (*ptr)();
    assert(process);

    // Bei Prozessen ist kein Verlass auf die Pointer,
    // daher ProcessID + ProcessType speichern:
    processID = ((SCProcess *)process)->GetID();
    saveArea.HistoryStore(&processID, sizeof(SCProcessID));
    processType = ((SCProcess *)process)->GetType();
    saveArea.HistoryStore(&processType, sizeof(SCProcessType *));

#if _SC_VALIDATION_OPTIMIZE

    // Erste Optimierungsstufe: Testen ob sich die verschiedenen
    // processs geaendert haben. Diese Optimierung dient der
    // Beschleunigung der Restore()-Methode. Processs die sich
    // gegenueber dem vorangegangenen Systemzustand nicht geaendert
    // haben koennen beim Restore uebersprungen werden!
    // Jedes Process-Objekt erhaelt dazu ein Modified-Flag.
    // Dieses Flag wird vor jedem Validationsschritt vom
    // Validator auf false zurueckgesetzt (siehe SCIndetVal).
    // Das Flag wird als zusaetzliche Infomation VOR jedes Process
    // in die saveArea geschrieben (s.u.). Es sollte NICHT in der
    // Save()-Methode des Processs gesichert werden!
    // Beim Backtracking muss das Flag daher explizit in der
    // Restore-Methode zurueckgesetzt werden.

    process_modified = process->IsModified();
    saveArea.HistoryStore(&process_modified, sizeof(SCBoolean));

#if _SC_VALIDATION_OPTIMIZE >= 2

    // Zweite Optimierungsstufe: Testen ob sich die verschiedenen Queues der
    // processs geaendert haben. Diese Optimierung dient ebenfalls der
    // Beschleunigung der Restore()-Methode. Folgende Kombinationen von
    // modifizierten(m) / unmodifizierten(u) Processs und Queues sind moeglich:
    //
    //     Process   ||    Queues des Process
    // =======================================
    //       m        ||      m                 <- Trivialfall
    //       u        ||      u                 <- Trivialfall
    //       m        ||      u                 <- hier setzt die
    //                                             Optimierung der 2. Stufe an!!
    //
    // Der Fall das ein Process unmodifiziert ist aber seine Queue modifiziert ist,
    // ist nicht moeglich!
    //
    // Dabei tritt folgendes Problem auf: Selbst wenn ein Process beim Restore geskippt
    // wird (s.u.), mussen die modified-Flags des Processs und seiner Queue restauriert
    // werden, da sie vor jedem Validationsschritt vom Algorithmus zunaechst auf
    // false zurueckgesetzt werden. Um beim Bachtracking wieder die korrekten
    // Werte herstellen zu koennen, muessen die modified-Flagse vor den
    // eigentlichen Processs gesondert in die Save-Area geschrieben werden.

    process_input_queue_modified = ((SCProcess *)process)->GetInputQueue()->GetModified();
    saveArea.HistoryStore(&process_input_queue_modified, sizeof(SCQueueStatus));
    process_timer_queue_modified = ((SCProcess *)process)->GetTimerQueue()->GetModified();
    saveArea.HistoryStore(&process_timer_queue_modified, sizeof(SCQueueStatus));

#endif  //  _SC_VALIDATION_OPTIMIZE >= 2

#endif  //  _SC_VALIDATION_OPTIMIZE
 
#if _SC_VALIDATION_OPTIMIZE >= 3

    // Dritte Optimierungsstufe: Bei Processs die sich gegenueber dem letzten
    // Systemzustand nicht geaendert haben, koennen wir das entsprechende
    // saveArea-Segment aus dem Vorgaengerzustand wiederverwenden.
    // Dadurch brauchen wir nicht die zeitintensive Save-Methode
    // fuer das Process aufrufen, sondern koennen die sehr
    // effiziente Append-Methode von SCMem verwenden. Dabei
    // wird der Speicherbereich in dem das Process im Vorgaengerzustand
    // gespeichert ist, an die aktuelle Position der saveArea kopiert.

    // letztes Stackelement (mit letztem Systemzustand) holen:
    
    if (!process_modified &&  // Optimierung moeglich ?
        (lastStackElem = ((SCIndetVal *)SCScheduler::GetIndet())->GetStack()->Top()) != NULL) 
    {
      lastState = lastStackElem->GetState();
      assert(lastState);

      // Alte Offsets des Processs muessen hier gemerkt werden,
      // da sie unten zunaechst neu gesetzt werden, bevor
      // die alten Werte benoetigt werden:
      lastOffset = process->GetLastOffset();
      lastHistoryOffset = process->GetLastHistoryOffset();

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcessList::Save(): using last stack elem for " << *process << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): at offset = " << lastOffset << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): at history offset = " << lastHistoryOffset << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): with size = " << process->currentSize << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): with history size = " << process->currentHistorySize << std::endl;
#endif

      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      process->SetLastOffset(saveArea.GetOffset());
      process->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcessList::Save(): setting new offset for " << *process;
      scValidationDebugLog << " on " << process->GetLastOffset() << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): setting new history offset for " << *process;
      scValidationDebugLog << " on " << process->GetLastHistoryOffset() << std::endl;
#endif

      saveArea.Append(*lastState,
                      lastOffset, process->currentSize,
                      lastHistoryOffset, process->currentHistorySize);

    }
    else
    {
      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      process->SetLastOffset(saveArea.GetOffset());
      process->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcessList::Save(): setting new offset for " << *process;
      scValidationDebugLog << " on " << process->GetLastOffset() << std::endl;
      scValidationDebugLog << "SCProcessList::Save(): setting new history offset for " << *process;
      scValidationDebugLog << " on " << process->GetLastHistoryOffset() << std::endl;
#endif

      process->Save(saveArea);
    }
#else
    process->Save(saveArea);
#endif

#if _SC_VALIDATION_OPTIMIZE
#if _SC_VALIDATION_DEBUG >= 2
    if (process->IsModified())
    {
      scValidationDebugLog << "SCProcessList::Save(): saved changed " << *process;
#if _SC_VALIDATION_OPTIMIZE >= 2
      switch (process_input_queue_modified)
      {
        case kSCQueueUnmodified:
          scValidationDebugLog << " with unmodified input queue";
          break;
        case kSCQueueCorrupted:
          scValidationDebugLog << " with corrupted input queue";
          break;
        case kSCQueueEnqueued:
          scValidationDebugLog << " with enqueued input queue";
          break;
        case kSCQueueDequeued:
          scValidationDebugLog << " with dequeued input queue";
          break;
      }
      switch (process_timer_queue_modified)
      {
        case kSCQueueUnmodified:
          scValidationDebugLog << " and unmodified timer queue";
          break;
        case kSCQueueCorrupted:
          scValidationDebugLog << " and corrupted timer queue";
          break;
        case kSCQueueEnqueued:
          scValidationDebugLog << " and enqueued timer queue";
          break;
        case kSCQueueDequeued:
          scValidationDebugLog << " and dequeued timer queue";
          break;
      }
#endif  //  _SC_VALIDATION_OPTIMIZE >= 2
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
    }
    else
    {      
      scValidationDebugLog << "SCProcessList::Save(): saved unchanged " << *process;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
    }
#endif  //  _SC_VALIDATION_OPTIMIZE

#else // !_SC_VALIDATION_OPTIMIZE

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCProcessList::Save(): saved " << *process;
    scValidationDebugLog << ", offset now " << saveArea.GetOffset();
    scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
#endif

#endif // _SC_VALIDATION_OPTIMIZE

#if _SC_PROFILING
    save_ineff++;
#endif

  } // for (...)
  return true;
}


SCBoolean SCProcessList::Restore(SCMem &saveArea)
{
  SCProcess *     process = NULL;
  SCNatural       i, number;
  SCProcessID     processID;
  SCProcessType * processType;
  SCProcessCons * newCons;

#if _SC_VALIDATION_OPTIMIZE
  SCBoolean       process_modified;

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus   process_input_queue_modified;
  SCQueueStatus   process_timer_queue_modified;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
  size_t          lastOffset, lastHistoryOffset;
#endif

#endif

  RemoveAllElements();
  
  saveArea.HistoryRestore(&number, sizeof(SCNatural)); // nicht in numOfElems einlesen!

  if (number == 0)
    return true;

  for (i = 0; i < number; i++)
  {
    // Bei Prozessen zunaechst ProcessID holen; falls
    // zugehoeriger Prozess noch existiert Zeiger holen;
    // falls Prozess nicht mehr existiert neu anlegen:
    saveArea.HistoryRestore(&processID, sizeof(SCProcessID));
    saveArea.HistoryRestore(&processType, sizeof(SCProcessType *));

#if _SC_VALIDATION_OPTIMIZE
    saveArea.HistoryRestore(&process_modified, sizeof(SCBoolean));
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
    saveArea.HistoryRestore(&process_input_queue_modified, sizeof(SCQueueStatus));
    saveArea.HistoryRestore(&process_timer_queue_modified, sizeof(SCQueueStatus));
#endif
    process = (SCProcess *)SCScheduler::GetRunnableFromID(processID);

#if _SC_VALIDATION_OPTIMIZE >= 3
    lastOffset =  saveArea.GetOffset();
    lastHistoryOffset = saveArea.GetHistoryOffset();
#endif

    if (process)  // process living?
    {
#if _SC_VALIDATION_OPTIMIZE >= 3
      process->SetLastOffset(lastOffset);
      process->SetLastHistoryOffset(lastHistoryOffset);
#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcessList::Restore(): setting new offset for " << *process;
      scValidationDebugLog << " on " << lastOffset << std::endl;
      scValidationDebugLog << "SCProcessList::Restore(): setting new history offset for " << *process;
      scValidationDebugLog << " on " << lastHistoryOffset << std::endl;
#endif
#endif

#if _SC_VALIDATION_OPTIMIZE
      if (process->IsModified())
      {
        process->Restore(saveArea);
#if _SC_VALIDATION_DEBUG >= 2
        scValidationDebugLog << "SCProcessList::Restore(): restored " << *process;
        scValidationDebugLog << ", offset now " << saveArea.GetOffset();
        scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << std::endl;
#endif
      }
      else // process is NOT modified!
      {
        saveArea.SkipOffset(process->currentSize);
        saveArea.SkipHistoryOffset(process->currentHistorySize);
#if _SC_VALIDATION_DEBUG >= 2
        scValidationDebugLog << "SCProcessList::Restore(): skipped unchanged " << *process;
        scValidationDebugLog << ", offset now " << saveArea.GetOffset() << "(+" << process->currentSize << ")";
        scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << "(+" << process->currentHistorySize << ")" << std::endl;
#endif
      }
#else // !_SC_VALIDATION_OPTIMIZE
      process->Restore(saveArea);
#endif // _SC_VALIDATION_OPTIMIZE

      // the calculated enabled transition are no longer valid
      // if we do a backtracking:
      if (((SCProcess *)process)->GetEnabledTransitions())
      {
        delete ((SCProcess *)process)->RetrieveEnabledTransitions();
      }
    }
    else           // process stopped -> recreation!
    {
      process = processType->NewInstance(saveArea);
      assert(process);
#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCProcessList::Restore(): recreated " << *process;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << std::endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
      process->SetLastOffset(lastOffset);
      process->SetLastHistoryOffset(lastHistoryOffset);
#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCProcessList::Restore(): setting new offset for " << *process;
      scValidationDebugLog << " on " << lastOffset << std::endl;
      scValidationDebugLog << "SCProcessList::Restore(): setting new history offset for " << *process;
      scValidationDebugLog << " on " << lastHistoryOffset << std::endl;
#endif
#endif
    } // if (process)

#if _SC_VALIDATION_OPTIMIZE

    //
    // Explicit restore of both modified flags:
    //

    if (process_modified)      // this MUST be done, because validator resets
      process->SetModified();  // the modified flags before every step!
    else
      process->UnsetModified();

#if _SC_VALIDATION_OPTIMIZE >= 2
    ((SCProcess *)process)->GetInputQueue()->SetModified(process_input_queue_modified);
    ((SCProcess *)process)->GetTimerQueue()->SetModified(process_timer_queue_modified);
#endif // _SC_VALIDATION_OPTIMIZE >= 2

#endif // _SC_VALIDATION_OPTIMIZE

    newCons = InsertAfter(process); // inkrementiert numOfElems!
    process->SetParent(this);
    process->SetContainer(newCons);

#if _SC_PROFILING
    restore_ineff++;
#endif
  }
  assert(numOfElems == number);

  return true;
}


void SCProcessList::Size(SCSize *curSize) const
{
  SCProcess *process;
  SCProcessCons *ptr;

  curSize->historySize += sizeof(SCNatural); // Speicher fuer Elementszaehler

  for (ptr = head; ptr; ptr = ptr->Next())
  {
    process = (*ptr)();
    assert(process);

    curSize->historySize += sizeof(SCProcessID);
    curSize->historySize += sizeof(SCProcessType *);

#if _SC_VALIDATION_OPTIMIZE
    curSize->historySize += sizeof(SCBoolean);     // modified (Process)

#if _SC_VALIDATION_OPTIMIZE >= 2
    curSize->historySize += sizeof(SCQueueStatus); // modified (Input Queue)
    curSize->historySize += sizeof(SCQueueStatus); // modified (Timer Queue)
#endif

#endif

#if _SC_VALIDATION_OPTIMIZE
    if (!process->HasVariableSize())
    {
      curSize->size += process->GetCurrentSize();
      curSize->historySize += process->GetCurrentHistorySize();
    }
    else
    {
      process->Size(curSize);
    }
#else
    process->Size(curSize);
#endif

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCProcessList::Size(): added " << *process;
#if _SC_VALIDATION_OPTIMIZE
    scValidationDebugLog << ", size now " << curSize->size << "(+" << process->GetCurrentSize() << ")";
    scValidationDebugLog << ", hist size now " << curSize->historySize << "(+" << process->GetCurrentHistorySize() << ")";
#endif
    scValidationDebugLog << std::endl;
#endif

#if _SC_PROFILING
    size_ineff++;
#endif
  }
}
