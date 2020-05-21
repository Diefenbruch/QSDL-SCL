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
| SCMachineList | SCMachineList.cc  | 10. Apr 1997  |   SCL             |      |
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

#include <string.h>
#include "SCMachineList.h"
#include "SCMachine.h"
#include "SCProcess.h"
#include "SCTimerControl.h"
#include "SCMachine.h"
#include "SCMachineLtd.h"
#include "SCProcessType.h"
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


SCBoolean SCMachineList::Save(SCMem& saveArea) const
{
  SCMachine *     machine;
  SCMachineCons * ptr;
#if _SC_VALIDATION_OPTIMIZE
  SCBoolean       machine_modified;

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus   machine_service_queue_modified;
  SCQueueStatus   machine_waiting_queue_modified;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
  SCMem *         lastState;
  SCStackElem *   lastStackElem;
  size_t          lastOffset, lastHistoryOffset;
#endif

#endif

  //
  // First, we write the number of machines in this queue:
  //  
  saveArea.HistoryStore(&numOfElems, sizeof(SCNatural));

  if (numOfElems == 0)
    return true;

  // The following loop writes all machines which are in the current
  // queue, in the given saveArea:
  
  for (ptr = head; ptr; ptr = ptr->Next())
  {
    machine = (*ptr)();
    assert(machine);

    // bei Maschinen reicht der Pointer da
    // diese Objekte nicht zerstoert werden:

    saveArea.HistoryStore(&machine, sizeof(SCMachine *));

#if _SC_VALIDATION_OPTIMIZE

    // Erste Optimierungsstufe: Testen ob sich die verschiedenen
    // machines geaendert haben. Diese Optimierung dient der
    // Beschleunigung der Restore()-Methode. Machines die sich
    // gegenueber dem vorangegangenen Systemzustand nicht geaendert
    // haben koennen beim Restore uebersprungen werden!
    // Jedes Machine-Objekt erhaelt dazu ein Modified-Flag.
    // Dieses Flag wird vor jedem Validationsschritt vom
    // Validator auf false zurueckgesetzt (siehe SCIndetVal).
    // Das Flag wird als zusaetzliche Infomation VOR jedes Machine
    // in die saveArea geschrieben (s.u.). Es sollte NICHT in der
    // Save()-Methode des Machines gesichert werden!
    // Beim Backtracking muss das Flag daher explizit in der
    // Restore-Methode zurueckgesetzt werden.

    machine_modified = machine->IsModified();
    saveArea.HistoryStore(&machine_modified, sizeof(SCBoolean));

#if _SC_VALIDATION_OPTIMIZE >= 2

    // Zweite Optimierungsstufe: Testen ob sich die verschiedenen Queues der
    // machines geaendert haben. Diese Optimierung dient ebenfalls der
    // Beschleunigung der Restore()-Methode. Folgende Kombinationen von
    // modifizierten(m) / unmodifizierten(u) Machines und Queues sind moeglich:
    //
    //     Machine   ||    Queues der Machine
    // =======================================
    //       m        ||      m                 <- Trivialfall
    //       u        ||      u                 <- Trivialfall
    //       m        ||      u                 <- hier setzt die
    //                                             Optimierung der 2. Stufe an!!
    //
    // Der Fall das eine Machine unmodifiziert ist aber eine ihrer
    // Queues modifiziert ist, ist nicht moeglich!
    //
    // Dabei tritt folgendes Problem auf: Selbst wenn ein Machine beim Restore geskippt
    // wird (s.u.), mussen die modified-Flags der Machine und ihrer Queues restauriert
    // werden, da sie vor jedem Validationsschritt vom Algorithmus zunaechst auf
    // false zurueckgesetzt werden. Um beim Bachtracking wieder die korrekten
    // Werte herstellen zu koennen, muessen die modified-Flags vor den
    // eigentlichen Machines gesondert in die Save-Area geschrieben werden:

    machine_service_queue_modified = ((SCMachine *)machine)->GetServiceQueue()->GetModified();
    saveArea.HistoryStore(&machine_service_queue_modified, sizeof(SCQueueStatus));

    // Hat die Maschine auch einen Warteraum? Dann muss auch das
    // modified-Flag des Warteraums gesichert werden:
    
    if (((SCMachine *)machine)->HasWaitingQueue())
    {
      machine_waiting_queue_modified = ((SCMachineLtd *)machine)->GetWaitingQueue()->GetModified();
      saveArea.HistoryStore(&machine_waiting_queue_modified, sizeof(SCQueueStatus));
    }
    
#endif  //  _SC_VALIDATION_OPTIMIZE >= 2

#endif  //  _SC_VALIDATION_OPTIMIZE
 
#if _SC_VALIDATION_OPTIMIZE >= 3

    // Dritte Optimierungsstufe: Bei Machines die sich gegenueber dem letzten
    // Systemzustand nicht geaendert haben, koennen wir das entsprechende
    // saveArea-Segment aus dem Vorgaengerzustand wiederverwenden.
    // Dadurch brauchen wir nicht die zeitintensive Save-Methode
    // fuer das Machine aufrufen, sondern koennen die sehr
    // effiziente Append-Methode von SCMem verwenden. Dabei
    // wird der Speicherbereich in dem das Machine im Vorgaengerzustand
    // gespeichert ist, an die aktuelle Position der saveArea kopiert.

    // letztes Stackelement (mit letztem Systemzustand) holen:
    
    if (!machine_modified &&  // Optimierung moeglich ?
        (lastStackElem = ((SCIndetVal *)SCScheduler::GetIndet())->GetStack()->Top()) != NULL) 
    {
      lastState = lastStackElem->GetState();
      assert(lastState);

      // Alte Offsets der Machine muessen hier gemerkt werden,
      // da sie unten zunaechst neu gesetzt werden, bevor
      // die alten Werte benoetigt werden:
      lastOffset = machine->GetLastOffset();
      lastHistoryOffset = machine->GetLastHistoryOffset();

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCMachineList::Save(): using last stack elem for machine " << *machine << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): at offset = " << lastOffset << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): at history offset = " << lastHistoryOffset << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): with size = " << machine->currentSize << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): with history size = " << machine->currentHistorySize << std::endl;
#endif

      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      machine->SetLastOffset(saveArea.GetOffset());
      machine->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCMachineList::Save(): setting new offset for machine " << *machine;
      scValidationDebugLog << " on " << machine->GetLastOffset() << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): setting new history offset for machine " << *machine;
      scValidationDebugLog << " on " << machine->GetLastHistoryOffset() << std::endl;
#endif

      saveArea.Append(*lastState,
                      lastOffset, machine->currentSize,
                      lastHistoryOffset, machine->currentHistorySize);

    }
    else
    {
      // Neue Offset-Werte setzen (dies muss VOR Append() oder Save() geschehen!):
      machine->SetLastOffset(saveArea.GetOffset());
      machine->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCMachineList::Save(): setting new offset for machine " << *machine;
      scValidationDebugLog << " on " << machine->GetLastOffset() << std::endl;
      scValidationDebugLog << "SCMachineList::Save(): setting new history offset for machine " << *machine;
      scValidationDebugLog << " on " << machine->GetLastHistoryOffset() << std::endl;
#endif

      machine->Save(saveArea);
    }
#else
    machine->Save(saveArea);
#endif

#if _SC_VALIDATION_OPTIMIZE
#if _SC_VALIDATION_DEBUG >= 2
    if (machine->IsModified())
    {
      scValidationDebugLog << "SCMachineList::Save(): saved changed " << *machine;
#if _SC_VALIDATION_OPTIMIZE >= 2
      switch (machine_service_queue_modified)
      {
        case kSCQueueUnmodified:
          scValidationDebugLog << " with unmodified service queue";
          break;
        case kSCQueueCorrupted:
          scValidationDebugLog << " with corrupted service queue";
          break;
        case kSCQueueEnqueued:
          scValidationDebugLog << " with enqueued service queue";
          break;
        case kSCQueueDequeued:
          scValidationDebugLog << " with dequeued service queue";
          break;
      }
      if (((SCMachine *)machine)->HasWaitingQueue())
      {
        switch (machine_waiting_queue_modified)
        {
          case kSCQueueUnmodified:
            scValidationDebugLog << " with unmodified waiting queue";
            break;
          case kSCQueueCorrupted:
            scValidationDebugLog << " with corrupted waiting queue";
            break;
          case kSCQueueEnqueued:
            scValidationDebugLog << " with enqueued waiting queue";
            break;
          case kSCQueueDequeued:
            scValidationDebugLog << " with dequeued waiting queue";
            break;
        }
      }
#endif  //  _SC_VALIDATION_OPTIMIZE >= 2
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
    }
    else
    {
      scValidationDebugLog << "SCMachineList::Save(): saved unchanged " << *machine;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
    }
#endif  //  _SC_VALIDATION_OPTIMIZE

#else // !_SC_VALIDATION_OPTIMIZE

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCMachineList::Save(): saved " << *machine;
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


SCBoolean SCMachineList::Restore(SCMem &saveArea)
{
  SCMachine     *machine = NULL;
  SCNatural       i, number;
  SCMachineCons *newCons;
#if _SC_VALIDATION_OPTIMIZE
  SCBoolean machine_modified;

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus machine_service_queue_modified;
  SCQueueStatus machine_waiting_queue_modified;
#endif

#endif

  RemoveAllElements();

  saveArea.HistoryRestore(&number, sizeof(SCNatural)); // nicht in numOfElems einlesen!

  if (number == 0)
    return true;

  for (i = 0; i < number; i++)
  {
    // bei Maschinen  Pointer
    // direkt aus Save-Area holen:
    saveArea.HistoryRestore(&machine, sizeof(SCMachine *));
    assert(machine);
#if _SC_VALIDATION_OPTIMIZE
    saveArea.HistoryRestore(&machine_modified, sizeof(SCBoolean));

#if _SC_VALIDATION_OPTIMIZE >= 2
    saveArea.HistoryRestore(&machine_service_queue_modified, sizeof(SCQueueStatus));
    if (machine->HasWaitingQueue())
    {
      saveArea.HistoryRestore(&machine_waiting_queue_modified, sizeof(SCQueueStatus));
    }
#endif

#if _SC_VALIDATION_OPTIMIZE >= 3
    machine->SetLastOffset(saveArea.GetOffset());
    machine->SetLastHistoryOffset(saveArea.GetHistoryOffset());

#if _SC_VALIDATION_DEBUG >= 3
    scValidationDebugLog << "SCMachineList::Restore(): setting new offset for " << *machine;
    scValidationDebugLog << " on " << machine->GetLastOffset() << std::endl;
    scValidationDebugLog << "SCMachineList::Restore(): setting new history offset for " << *machine;
    scValidationDebugLog << " on " << machine->GetLastHistoryOffset() << std::endl;
#endif

#endif

    if (machine->IsModified()) // use modify-flag inside machine
    {                           // not machine_modified!
      machine->Restore(saveArea);

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCMachineList::Restore(): restored " << *machine;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << std::endl;
#endif
    }
    else // machine is not modified!
    {
      saveArea.SkipOffset(machine->currentSize);
      saveArea.SkipHistoryOffset(machine->currentHistorySize);

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCMachineList::Restore(): skipped unchanged " << *machine;
      scValidationDebugLog << ", offset now " << saveArea.GetOffset() << "(+" << machine->currentSize << ")";
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << "(+" << machine->currentHistorySize << ")" << std::endl;
#endif
    }

    //
    // Explicit restore of all modified flags:
    //

    if (machine_modified)      // this MUST be done, because validator resets
      machine->SetModified();  // the modified flags befor every step!
    else
      machine->UnsetModified();

#if _SC_VALIDATION_OPTIMIZE >= 2
    ((SCMachine *)machine)->GetServiceQueue()->SetModified(machine_service_queue_modified);
    if (machine->HasWaitingQueue())
    {
      ((SCMachineLtd *)machine)->GetWaitingQueue()->SetModified(machine_waiting_queue_modified);
    }
#endif

#else // !_SC_VALIDATION_OPTIMIZE
    machine->Restore(saveArea);
#endif

    newCons = InsertAfter(machine); // inkrementiert numOfElems!
    assert (newCons != NULL);

    machine->SetParent (this);

#if _SC_PROFILING
    restore_ineff++;
#endif
  }
  assert(numOfElems == number);

  return true;
}


void SCMachineList::Size(SCSize *curSize) const
{
  SCMachine *machine;
  SCMachineCons *ptr;

  curSize->historySize += sizeof(SCNatural); // Speicher fuer Elementszaehler

  for (ptr = head; ptr; ptr = ptr->Next())
  {
    machine = (*ptr)();
    assert(machine);

    curSize->historySize += sizeof(SCMachine *);

#if _SC_VALIDATION_OPTIMIZE
    curSize->historySize += sizeof(SCBoolean);     // modified (Machine)

#if _SC_VALIDATION_OPTIMIZE >= 2
    curSize->historySize += sizeof(SCQueueStatus); // modified (Service Queue)
    if (machine->HasWaitingQueue())
    {
      curSize->historySize += sizeof(SCQueueStatus); // modified (Waiting Queue)
    }
#endif

#endif

#if _SC_VALIDATION_OPTIMIZE
    curSize->size += machine->GetCurrentSize();
    curSize->historySize += machine->GetCurrentHistorySize();
#else
    machine->Size(curSize);
#endif

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCMachineList::Size(): added " << *machine;
#if _SC_VALIDATION_OPTIMIZE
    scValidationDebugLog << ", size now " << curSize->size << "(+" << machine->GetCurrentSize() << ")";
    scValidationDebugLog << ", hist size now " << curSize->historySize << "(+" << machine->GetCurrentHistorySize() << ")";
#endif
    scValidationDebugLog << std::endl;
#endif

#if _SC_PROFILING
    size_ineff++;
#endif
  }
}
