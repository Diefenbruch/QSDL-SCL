/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+------------------+-------+
|   Module      |   File            |   Created     |   Project        |       |
+---------------+-------------------+---------------+------------------+-------+
|   SCRunnable  |   SCRunnable.cc   | 16. Aug 1994  |   SCL            |       |
+---------------+-------------------+---------------+------------------+-------+
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
#include "SCStream.h"
#include "SCRunnable.h"
#include "SCScheduler.h"
#include "SCPath.h"
#include "SCTimerControl.h"
#include "SCMachineLtd.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCRunnable.inl.h"
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  #include "SCProcess.h"
  #include "SCTimerControl.h"
  #include "SCMachine.h"
#endif


SCRunnable::SCRunnable (SCRunnableID pRunnableID,
                        SCBoolean sleeping,
                        const SCObjectType newType,
                        const SCObject* pParent) :
  SCSystemRunnable(newType, pParent),
  runnableID(pRunnableID),
  blocked(false),
  waiting(sleeping)
{
  wakeupTime = SCScheduler::GetCurrentTime();

  traceOn = true;

#if _SC_VALIDATION_OPTIMIZE
  modified = true;                                // CurrentSize has no
                                                  // valid value!
  currentSize = 0;
  currentHistorySize = 0;

#if _SC_VALIDATION_OPTIMIZE >= 3
  lastOffset = 0;
  lastHistoryOffset = 0;
#endif

#endif
}

// Der folgende Konstruktor legt ein "leeres" Runnable an.
// Dies dient als Vorbereitung fuer ein folgendes Restore.

SCRunnable::SCRunnable (const SCObjectType newType,
                        const SCObject *pParent):
  SCSystemRunnable(newType, pParent)              // This invokes the REGULAR
                                                  // constructor, not the
                                                  // SCMem-constructor of the
                                                  // base class, hence a new
                                                  // thread is created.
{
#if _SC_VALIDATION_OPTIMIZE
  modified = true;                                // CurrentSize has no
                                                  // valid value!
  currentSize = 0;
  currentHistorySize = 0;

#if _SC_VALIDATION_OPTIMIZE >= 3
  lastOffset = 0;
  lastHistoryOffset = 0;
#endif

#endif
}


SCRunnable::~SCRunnable (void)
{
}


void SCRunnable::Stop (void)
{
  Cancel();
}


void SCRunnable::Schedule (SCNatural how, SCTime time)
{
#if _SC_VALIDATION_OPTIMIZE
  SetModified();
#endif

  waiting = false;
  blocked = false;
  wakeupTime = 0;

  switch (how)
  {
    case kSCScheduleNow:
      wakeupTime = SCScheduler::GetCurrentTime();
      break;
    case kSCScheduleTime:
      assert (time >= 0);
      wakeupTime = time;
      break;
    case kSCScheduleWaiting:
      waiting = true;
      break;
    case kSCScheduleBlocked:
      blocked = true;
      break;
  }
  SCScheduler::Schedule (this);
}


void SCRunnable::Size(SCSize *curSize) const
{
  curSize->historySize += (sizeof(SCRunnableID) + // Member runnableID
                           sizeof(SCTime) +       // Member wakeupTime
                           sizeof(SCBoolean));    // Member waiting
  curSize->historySize += sizeof(SCBoolean);      // Member traceOn
  curSize->size += sizeof(SCBoolean);             // Member blocked
}


SCBoolean SCRunnable::Save(SCMem& saveArea) const
{
  saveArea.HistoryStore(&runnableID, sizeof(SCRunnableID));
  saveArea.HistoryStore(&wakeupTime, sizeof(SCTime));
  saveArea.Store(&blocked, sizeof(SCBoolean));
  saveArea.HistoryStore(&waiting, sizeof(SCBoolean));
  saveArea.HistoryStore(&traceOn, sizeof(SCBoolean));

  return true;                
}


SCBoolean SCRunnable::Restore(SCMem& saveArea)
{
  saveArea.HistoryRestore(&runnableID, sizeof(SCRunnableID));
  saveArea.HistoryRestore(&wakeupTime, sizeof(SCTime));
  saveArea.Restore(&blocked, sizeof(SCBoolean));
  saveArea.HistoryRestore(&waiting, sizeof(SCBoolean));
  saveArea.HistoryRestore(&traceOn, sizeof(SCBoolean));

  return true;                
}


SCStream& operator<< (SCStream& pStream, const SCRunnable& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCRunnable::Display(SCStream& pStream) const
{
  pStream << "(#" << GetID() << ")";

  return pStream;
}


#if _SC_VALIDATION_OPTIMIZE

void SCRunnable::UnsetModified (SCBoolean setQueues)
{
  modified = false;
  
  if (!setQueues)
    return;

#if _SC_VALIDATION_OPTIMIZE >= 2
  switch (GetType())
  {
    case SC_PROCESS:
      ((SCProcess *)this)->GetInputQueue()->UnsetModified();
      ((SCProcess *)this)->GetTimerQueue()->UnsetModified();
      break;

    case SC_MACHINE:
      ((SCMachine *)this)->GetServiceQueue()->UnsetModified();
      if (((SCMachine *)this)->HasWaitingQueue())
      {
        ((SCMachineLtd *)this)->GetWaitingQueue()->UnsetModified();
      }
      break;

    case SC_PATH:
      ((SCPath *)this)->GetDelayQueue()->UnsetModified();
      break;
      
    case SC_TIMER_CONTROL:
      break;

    case SC_PROCEDURE:
      break;
      
    default:
      assert (false);
      break;
  }
#endif
}


void SCRunnable::SetModified (SCBoolean setQueues)
{
  modified = true;
  
  if (!setQueues)
    return;

#if _SC_VALIDATION_OPTIMIZE >= 2
  switch (GetType())
  {
    case SC_PROCESS:
      ((SCProcess *)this)->GetInputQueue()->SetModified(kSCQueueCorrupted);
      ((SCProcess *)this)->GetTimerQueue()->SetModified(kSCQueueCorrupted);
      break;

    case SC_MACHINE:
      ((SCMachine *)this)->GetServiceQueue()->SetModified(kSCQueueCorrupted);
      if (((SCMachine *)this)->HasWaitingQueue())
      {
        ((SCMachineLtd *)this)->GetWaitingQueue()->SetModified(kSCQueueCorrupted);
      }
      break;

    case SC_PATH:
      ((SCPath *)this)->GetDelayQueue()->SetModified(kSCQueueCorrupted);
      break;
      
    case SC_TIMER_CONTROL:
      break;

    case SC_PROCEDURE:
      break;
      
    default:
      assert (false);
      break;
  }
#endif
}

#endif  // _SC_VALIDATION_OPTIMIZE
