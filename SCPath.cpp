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
|  SCPath       |  SCPath.cc        | 30. Aug 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     30.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCPath.h"
#include "SCDataType.h"
#include "SCDelayedSignal.h"
#include "SCSignal.h"
#include "SCProcess.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCMem.h"

#include "SCTraceControl.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCPath * SCPath::path = NULL;


/*----- Constructors -----*/

SCPath::SCPath (void) :
  SCRunnable (kSCPathProcessID,   // runnableID
              true,               // sleeping
              SC_PATH)            // type
{
#if _SC_VALIDATION_OPTIMIZE
  SCSize mySize;
#endif

  if (IsVerbose())
    SCScheduler::outputStream << "Creating " << *this << std::endl;

  path = this;

  delayQueue = new SCDelayedSignalSaveList(true, this);
  assert(delayQueue);

#if _SC_VALIDATION_OPTIMIZE
  // Die Initialgroesse des Path-Objekts muss nur einmal
  // beim Konstruieren berechnet werden. Danach sorgt
  // SCListSave automatisch fuer die Anpassung der aktuellen
  // Groesse die in SCRunnable gespeichert ist:

  Size(&mySize);
  SetCurrentSize(mySize.size);
  SetCurrentHistorySize(mySize.historySize);
#endif

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " created" << std::endl;
#endif
}

/*----- Destructor -----*/

SCPath::~SCPath (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;

  if (delayQueue)
    delete delayQueue;
}


/*----- Member-Funktionen -----*/

void SCPath::SpawnDelayed (SCProcess *receiver,
                           SCSignal * signal,
                           const SCTime &pWhen)
{
  SCDelayedSignalCons * elem;
  SCDelayedSignal *     delayedSignal;
  
  delayedSignal = new SCDelayedSignal(signal,
                                      receiver->Self(),
                                      receiver->GetType(),
                                      pWhen,
                                      this);
  assert(delayedSignal);
  
#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " queueing " << *delayedSignal << std::endl;
#endif

  if (delayQueue->IsEmpty())
  {
    delayQueue->InsertBefore (delayedSignal);
  }
  else
  {
    for (elem = delayQueue->Head();
         elem != NULL;
         elem = elem->Next())
    {
      if ((*elem)()->GetDelayedUntil() > pWhen)
      {
        delayQueue->InsertBefore (delayedSignal, elem);
        break;
      }
    }
    if (!elem)
    {
      delayQueue->InsertAfter (delayedSignal);
    }
  }
  
  Reschedule();
}


void SCPath::SpawnDelayed (SCProcessType *receiverType,
                           SCSignal * signal,
                           const SCTime &pWhen)
{
  SCDelayedSignalCons * elem;
  SCDelayedSignal *     delayedSignal;
  
  delayedSignal = new SCDelayedSignal(signal,
                                      kSCNoProcessID,
                                      receiverType,
                                      pWhen,
                                      this);
  assert(delayedSignal);
  
#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " queueing " << *delayedSignal << std::endl;
#endif

  if (delayQueue->IsEmpty())
  {
    delayQueue->InsertBefore (delayedSignal);
  }
  else
  {
    for (elem = delayQueue->Head();
         elem != NULL;
         elem = elem->Next())
    {
      if ((*elem)()->GetDelayedUntil() > pWhen)
      {
        delayQueue->InsertBefore (delayedSignal, elem);
        break;
      }
    }
    if (!elem)
    {
      delayQueue->InsertAfter (delayedSignal);
    }
  }

  Reschedule();
}


void SCPath::Reschedule (void)
{
  SCTime nextTime;

  nextTime = -1.0;

  if (!delayQueue->IsEmpty())
  {
    nextTime = (*delayQueue->Head())()->GetDelayedUntil();
  }

  if (nextTime >= 0.0)
  {
    Schedule (kSCScheduleTime, nextTime);
  }
  else
  {
    Schedule (kSCScheduleWaiting);
  }
}


void SCPath::Body (void)
{
  SCDelayedSignal * delayedSignal;
  SCProcess *        process;
  SCProcessType *    procType;
  
#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " activated" << std::endl;
#endif

  if (IsVerbose())
    SCScheduler::outputStream << "Executing " << *this << std::endl;

  while (true)
  {
    while (!delayQueue->IsEmpty() &&
           (*delayQueue->Head())()->GetDelayedUntil() <=
            SCScheduler::GetCurrentTime())
    {
      delayedSignal = delayQueue->Remove(); // dequeue first element

      //
      // Delayed output to process type or to process instance?
      //      
      if (delayedSignal->GetReceiverID() != kSCNoProcessID)
      {
        process = (SCProcess *)delayedSignal->GetReceiver();
        assert(process);
      }
      else
      {
        procType = (SCProcessType *)delayedSignal->GetReceiverType();
        assert(procType);
        
        process = procType->GetAProcess();
      }

      if (process)
      {
#if _SC_DEBUGOUTPUT
        scDebugLog << SCScheduler::GetCurrentTime() << ": ";
        scDebugLog << *this << " delivering " << *delayedSignal->GetSignal();
        scDebugLog << " to " << *process << std::endl;
#endif

        process->Receive(delayedSignal->RetrieveSignal());
      }
      else
      {
#if _SC_DEBUGOUTPUT
        scDebugLog << SCScheduler::GetCurrentTime() << ": ";
        scDebugLog << *this << " discarding " << *delayedSignal->GetSignal();
        scDebugLog << std::endl;
#endif
      }
      
      delete delayedSignal;

    } // while (!delayQueue->IsEmpty() && ...)

    Reschedule();
    Suspend();
  } // while (true)

#if _SC_DEBUGOUTPUT
  scDebugLog << *this << " finishing" << std::endl;
#endif
}


void SCPath::Size(SCSize *curSize) const
{
#if _SC_VALIDATION_DEBUG >= 2
  SCNatural size = curSize->size;
  SCNatural hsize = curSize->historySize;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  curSize->historySize += sizeof(SCQueueStatus);
#endif

  SCRunnable::Size(curSize);

  assert(delayQueue);
  delayQueue->Size(curSize);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCPath::Size(): added " << *this;
  scValidationDebugLog << ", size now " << curSize->size;
  scValidationDebugLog << "(+" << curSize->size - size << ")";
  scValidationDebugLog << ", hist size now " << curSize->historySize;
  scValidationDebugLog << "(+" << curSize->historySize - hsize << ")" << std::endl;
#endif
}


SCBoolean SCPath::Save(SCMem& saveArea) const
{
#if _SC_VALIDATION_DEBUG >= 2
  SCNatural pos = saveArea.GetOffset();
  SCNatural hpos = saveArea.GetHistoryOffset();
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus delay_queue_modified;

  delay_queue_modified = GetDelayQueue()->GetModified();
  saveArea.HistoryStore(&delay_queue_modified, sizeof(SCQueueStatus));
#endif

  SCRunnable::Save(saveArea);
  
  assert(delayQueue);
  delayQueue->Save(saveArea);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCPath::Save(): saved " << *this;
#if _SC_VALIDATION_OPTIMIZE >= 2
  switch (delay_queue_modified)
  {
    case kSCQueueUnmodified:
      scValidationDebugLog << " with unmodified delay queue";
      break;
    case kSCQueueCorrupted:
      scValidationDebugLog << " with corrupted delay queue";
      break;
    case kSCQueueEnqueued:
      scValidationDebugLog << " with enqueued delay queue";
      break;
    case kSCQueueDequeued:
      scValidationDebugLog << " with dequeued delay queue";
      break;
  }
#endif
  scValidationDebugLog << ", offset now " << saveArea.GetOffset();
  scValidationDebugLog << "(+" << saveArea.GetOffset() - pos << ")";
  scValidationDebugLog << ", hist offset now " << saveArea.GetHistoryOffset();
  scValidationDebugLog << "(+" << saveArea.GetHistoryOffset() - hpos << ")";
  scValidationDebugLog << std::endl;
#endif

  return true;
}


SCBoolean SCPath::Restore(SCMem &saveArea)
{
#if _SC_VALIDATION_DEBUG >= 2
  SCNatural pos = saveArea.GetOffset();
  SCNatural hpos = saveArea.GetHistoryOffset();
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  SCQueueStatus delay_queue_modified;

  saveArea.HistoryRestore(&delay_queue_modified, sizeof(SCQueueStatus));
#endif

  SCRunnable::Restore(saveArea);

  assert(delayQueue);
  delayQueue->Restore(saveArea);
  
#if _SC_VALIDATION_OPTIMIZE >= 2
  GetDelayQueue()->SetModified(delay_queue_modified);
#endif

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCPath::Restore(): restored " << *this;
  scValidationDebugLog << ", offset now " << saveArea.GetOffset();
  scValidationDebugLog << "(+" << saveArea.GetOffset() - pos << ")";
  scValidationDebugLog << ", hist offset now " << saveArea.GetHistoryOffset();
  scValidationDebugLog << "(+" << saveArea.GetHistoryOffset() - hpos << ")";
  scValidationDebugLog << std::endl;
#endif

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCPath& pData)
{
  pStream << "runnable <path>(#" << pData.GetID() << ")";

  return pStream;
}


SCStream& SCPath::Display(SCStream& pStream) const
{
  return (pStream << *this);
}
