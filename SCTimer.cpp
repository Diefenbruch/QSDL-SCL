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
|  SCTimer      |  SCTimer.cc       | 24. Feb 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCTimer.h"
#include "SCDataType.h"
#include "SCProcess.h"
#include "SCScheduler.h"
#include "SCTimeEventList.h"
#include "SCTimerType.h"
#include "SCTimerControl.h"
#include "SCTimerListTable.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCNatural SCTimer::nextTimerID = 1;
SCTimerTable *SCTimer::timerTable = NULL;

#if _SC_NOINLINES
  #include "SCTimer.inl.h"
#endif


/*----- Konstruktor -----*/

SCTimer::SCTimer (const SCProcess* pProcess,
                  const SCTime &pWhen,
                  const SCTimerType *const pTimerType,
                  SCDataType *pData,
                  const SCObject* pParent):
  SCObject(SC_TIMER, pParent),
  timerID(nextTimerID++),
  timerType((SCTimerType *)pTimerType),
  when(pWhen),
  data(pData),
  container(NULL),
  tableContainer(NULL),
  timeEventContainer(NULL)
{
  assert(pProcess != NULL);
  processID = pProcess->GetID();

  AddTimer(); // insert timer instace
              // into hash table of
              // timer type for
              // SCTimerControl::Locate() speedup
}


SCTimer::SCTimer(SCMem &saveArea, const SCObject *pParent):
  SCObject(SC_TIMER, pParent),
  timerType(NULL),
  data(NULL),
  container(NULL),
  tableContainer(NULL),
  timeEventContainer(NULL)
{
  Restore(saveArea);

  AddTimer(); // insert timer instace
              // into hash table of
              // timer type for
              // SCTimerControl::Locate() speedup
}


SCTimer::~SCTimer (void)
{
  if (data)
  {
    delete data;
  }

  RemoveTimer();
}


void SCTimer::AddTimer (void)
{
  SCTimerListTable *table;
  SCTimerList      *list;
  SCTimerCons      *timerContainer;
  SCTimeEventCons *retValue;
  
  // we have a hash table in SCTimerType
  // for all timers of this type:
  table = timerType->GetTimerListTable();
  assert(table);

  list = table->Find (processID);
  if (!list)                               // first timer for this timer type?
  {
    list = new SCTimerList(false);         // allocate list for timer type
    assert(list);

    timerContainer = list->InsertAfter (this);  // Containerverwaltung hier
    assert(timerContainer);                     // notwendig da keine Save-Liste!
    SetTableContainer(timerContainer);

    table->Insert (processID, list);       // insert into hash table
  }
  else                                     // list is present, so other timers
  {                                        // for this timer type have already
                                           // been inserted
    timerContainer = list->InsertAfter (this);
    assert(timerContainer);
    SetTableContainer(timerContainer);
  }
  
  retValue = SCTimerControl::GetTimerControl()->AddTimerEvent(this);
  SetTimeEventContainer(retValue);

  // we have a hash table in SCTimer
  // for all living timers:
  assert(timerTable); // allocated in SCTimer::Initialize()

  if (!timerTable->Insert(timerID, this))
  {
    std::cerr << std::endl << "Timer Hash Table is full ! " << std::endl << std::endl;
    assert (false);
    exit(1);
  }
}


void SCTimer::RemoveTimer (void)
{
  SCTimerListTable *table;
  SCTimerList      *list;
  SCTimerCons      *timerContainer;
  SCTimeEventCons  *vTimeEventContainer;
  
  assert(timerType);
  table = timerType->GetTimerListTable();
  assert(table);

  list = table->Find (processID);
  assert(list);

  timerContainer = GetTableContainer();
  assert(timerContainer);

  list->Remove(timerContainer);
  
  vTimeEventContainer = GetTimeEventContainer();
  assert(vTimeEventContainer);
  
  SCTimerControl::GetTimerControl()->RemoveTimerEvent(vTimeEventContainer);

  assert(timerTable);
  timerTable->Remove(timerID);
}


SCStream& operator<< (SCStream& pStream, const SCTimer& pData)
{
  pStream << "timer <" << *pData.GetTimerType() << ">(#";
  pStream << pData.GetTimerID() << ", time = " << pData.when << ")";

  return pStream;
}


SCStream& SCTimer::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


SCDataType *SCTimer::RetrieveData (void)
{
  SCDataType *retrData = data;

#if _SC_VALIDATION_OPTIMIZE
  if (retrData && GetContainer())  // data present and Timer is in List ?
  {                                // we must adjust the list size!
    SCSize dataSize;
    SCRunnable *runnable;
    SCTimerSaveList *list = (SCTimerSaveList *)GetContainer()->List();
    assert (list);

    runnable = (SCRunnable *)list->GetParent();
    assert(runnable);

    retrData->Size(&dataSize);

    list->SetCurrentSize(list->GetCurrentSize() - dataSize.size);
    list->SetCurrentHistorySize(list->GetCurrentHistorySize() -
                                dataSize.historySize);

    runnable->currentSize -= dataSize.size;
    runnable->currentHistorySize -= dataSize.historySize;

#if _SC_VALIDATION_OPTIMIZE >= 2
    list->SetModified(kSCQueueCorrupted);
#endif
  }
#endif

  data = NULL;

  return (retrData);
}


void SCTimer::Size(SCSize *curSize) const
{
  // system state member:
  curSize->size += (sizeof(SCNatural) +             // ID of timerType
                    sizeof(SCProcessID));           // processID

#if _SC_VALIDATION_TIME == 1
  curSize->size += sizeof(SCDuration);              // remainingTime
#endif

  // history state member:
  curSize->historySize += (sizeof(SCNatural) +      // timerID
                           sizeof(SCTimerType *) +  // timerType
                           sizeof(SCTime));         // when

  if (data)
    data->Size(curSize);
}


SCBoolean SCTimer::Save(SCMem& saveArea) const
{
  SCNatural key;

#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif
  
  saveArea.HistoryStore(&timerID, sizeof(SCNatural));

  // nur Zeiger sichern, da statisches Objekt!
  saveArea.HistoryStore(&timerType, sizeof(SCTimerType *));

  // zusaetzlich ID des Timer-Signals speichern, da
  // Signale zustandsrelevant sind:
  key = timerType->GetID();
  saveArea.Store(&key, sizeof(SCNatural));

  saveArea.Store(&processID, sizeof(SCProcessID));

  if (data)
    data->Save (saveArea);

  saveArea.HistoryStore (&when, sizeof(SCTime));

#if _SC_VALIDATION_TIME == 1
  remainingTime = when - SCScheduler::GetCurrentTime();
  assert(remainingTime >= 0.0);
  saveArea.Store(&remainingTime, sizeof(SCDuration));
#endif

  return true;
}


SCBoolean SCTimer::Restore(SCMem& saveArea)
{
  SCNatural   key;

#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif

  saveArea.HistoryRestore (&timerID, sizeof(SCNatural));

  // nur Zeiger restaurieren, da statisches Objekt!
  saveArea.HistoryRestore (&timerType, sizeof(SCTimerType *));
  saveArea.Restore (&key, sizeof(SCNatural)); // dummy key

  saveArea.Restore (&processID, sizeof(SCProcessID));
  
  if (data)
    delete data;

  data = timerType->NewData();
  if (data)
  {
    data->Restore (saveArea);
  }
  
  saveArea.HistoryRestore (&when, sizeof (SCTime));

#if _SC_VALIDATION_TIME == 1
  saveArea.Restore (&remainingTime, sizeof (SCDuration));
#endif

  return true;
}


SCBoolean SCTimer::Initialize (SCNatural timerTableSize)
{
  // allocate table for all timer instances

  timerTable = new SCTimerTable(timerTableSize,
                                false,  // delete elems
                                true,   // resolve coll
                                true,   // resize if full
                                NULL);  // parent
  assert(timerTable);

  return true;
}


SCBoolean SCTimer::Finish (void) // static!
{
  // delete table of timers

  if (timerTable)
    delete timerTable;

  return true;
}


void SCTimer::StaticSize(SCSize *curSize)
{
  curSize->historySize += sizeof(SCNatural);         // Member nextTimerID
}


SCBoolean SCTimer::StaticSave(SCMem &saveArea)
{
  saveArea.HistoryStore(&nextTimerID, sizeof(SCNatural));

  return true;
}


SCBoolean SCTimer::StaticRestore(SCMem &saveArea)
{
  saveArea.HistoryRestore(&nextTimerID, sizeof(SCNatural));

  return true;
}


SCBoolean SCTimer::operator==(const SCTimer &second) const
{
  return (timerID == second.timerID);
}


SCBoolean SCTimer::operator!=(const SCTimer &second) const
{
  return (timerID != second.timerID);
}
