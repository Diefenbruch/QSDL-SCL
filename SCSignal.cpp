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
|   SCSignal    |   SCSignal.cc     | 24. Feb 1996  |   SCL             |      |
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
#include "SCSignal.h"
#include "SCDataType.h"
#include "SCProcess.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCSignalTable.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCNatural SCSignal::nextSignalID = 1;
SCSignalTable *SCSignal::signalTable = NULL;

#if _SC_NOINLINES
  #include "SCSignal.inl.h"
#endif

#include "SCDebug.h"

/*----- Konstruktor -----*/

SCSignal::SCSignal (const SCProcessID pSenderID,
                    const SCProcessType *const pSenderType,
                    const SCSignalType *const pSignalType,
                    SCDataType *pData,
                    const SCNatural pTimerID,
                    const SCObject* pParent) :
  SCObject(SC_SIGNAL, pParent),
  signalID(nextSignalID++),
  timerID(pTimerID),
  signalType((SCSignalType *)pSignalType),
  senderID(pSenderID),
  senderType((SCProcessType *)pSenderType),
  data(pData),
  container(NULL)
{
  creationTime = SCScheduler::GetCurrentTime();

  assert(signalTable); // allocated in SCSignal::Initialize()

  if (!signalTable->Insert(signalID, this))
  {
    std::cerr << std::endl << "Signal Hash Table is full ! " << std::endl << std::endl;
    assert (false);
    exit(1);
  }
}


SCSignal::SCSignal(SCMem &saveArea, const SCObject *pParent):
  SCObject(SC_SIGNAL, pParent),
  signalType(NULL),
  data(NULL),
  container(NULL)
{  
  Restore(saveArea);

  assert(signalTable);
  signalTable->Insert(signalID, this);
}

SCSignal::~SCSignal (void)
{
  if (data)
  {
    delete data;
  }

  assert(signalTable);
  signalTable->Remove(signalID);
}


/*----- Member-Funktionen -----*/

SCBoolean SCSignal::SameData (const SCDataType *const pData) const
{
  if (data != NULL && pData != NULL && data->Equal (*pData))
    return (true);
  else
    return (false);
}


SCDataType *SCSignal::RetrieveData (void)
{
  SCDataType *retrData = data;

#if _SC_VALIDATION_OPTIMIZE
  if (retrData && GetContainer())  // Signal is in List ?
  {                                // we must adjust the list size!
    SCSize dataSize;
    SCRunnable *runnable;
    SCSignalSaveList *list = (SCSignalSaveList *)GetContainer()->List();
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


SCStream& operator<< (SCStream& pStream, const SCSignal& pData)
{
  assert(pData.GetSignalType());

  pStream << "signal <" << *pData.GetSignalType() << ">(#";
  pStream << pData.GetSignalID() << ")";

  return pStream;
}


SCStream& SCSignal::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


void SCSignal::Size(SCSize *curSize) const
{
  // system state members:
  curSize->size += (sizeof(SCProcessID) +            // Member senderID
                    sizeof(SCNatural));              // ID of signal type
  if (data)
    data->Size(curSize);                             // Member data

  // history system state members:
  curSize->historySize += (sizeof(SCNatural) +       // Member signalID
                           sizeof(SCNatural) +       // Member timerID
                           sizeof(SCProcessType *) + // Member senderType
                           sizeof(SCSignalType *) +  // Member signalType
                           sizeof(SCTime));          // Member creationTime
}


SCBoolean SCSignal::Save (SCMem& saveArea) const
{
  SCNatural key;

  saveArea.HistoryStore(&signalID, sizeof(SCNatural));
  saveArea.HistoryStore(&timerID, sizeof(SCNatural));
  saveArea.Store(&senderID, sizeof(SCProcessID));

  // nur Zeiger sichern, da statische Objekte:
  saveArea.HistoryStore(&senderType, sizeof(SCProcessType *));
  saveArea.HistoryStore(&signalType, sizeof(SCSignalType *));

  // zusaetzlich ID des signalTypes speichern da
  // zustandsrelevant:
  key = signalType->GetID();
  saveArea.Store(&key, sizeof(SCNatural));
  
  if (data)
  {
    data->Save(saveArea);
  }

  saveArea.HistoryStore(&creationTime, sizeof(SCTime));

  return true;
}


SCBoolean SCSignal::Restore (SCMem& saveArea)
{
  SCNatural key;

  saveArea.HistoryRestore(&signalID, sizeof(SCNatural));
  saveArea.HistoryRestore(&timerID, sizeof(SCNatural));
  saveArea.Restore(&senderID, sizeof(SCProcessID));
  
  // nur Zeiger restaurieren, da statische Objekte:
  saveArea.HistoryRestore(&senderType, sizeof(SCProcessType *));
  saveArea.HistoryRestore(&signalType, sizeof(SCSignalType *));

  saveArea.Restore(&key, sizeof(SCNatural)); // dummy key

  if (data)
  {
    delete data;
  }

  assert (signalType);

  data = signalType->NewData();
  if (data)
  {
    data->Restore(saveArea);
  }

  saveArea.HistoryRestore(&creationTime, sizeof(SCTime));

  return true;
}


SCBoolean SCSignal::Initialize (SCNatural signalTableSize)
{
  // allocate table for all signal instances

  signalTable = new SCSignalTable(signalTableSize,
                                 false,  // delete elems
                                 true,   // resolveColl
                                 true);  // resize if full
  assert(signalTable);

  return true;
}


SCBoolean SCSignal::Finish (void) // static!
{
  // delete table of signal instances

  if (signalTable)
    delete signalTable;

  return true;
}


SCBoolean SCSignal::StaticSave(SCMem &saveArea)
{
  saveArea.HistoryStore(&nextSignalID, sizeof(SCNatural));

  return true;
}


SCBoolean SCSignal::StaticRestore(SCMem &saveArea)
{
  saveArea.HistoryRestore(&nextSignalID, sizeof(SCNatural));

  return true;
}


void SCSignal::StaticSize(SCSize *curSize)
{
  curSize->historySize += sizeof(SCNatural);       // Member nextSignalID
}


SCBoolean SCSignal::operator==(const SCSignal &second) const
{
  return (signalID == second.signalID);
}


SCBoolean SCSignal::operator!=(const SCSignal &second) const
{
  return (signalID != second.signalID);
}
