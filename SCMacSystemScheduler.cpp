/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-95 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+------------------------+---------------+--------------+------+
|   Module      |   File                 |   Created     |   Project    |      |
+---------------+------------------------+---------------+--------------+------+
|  SCScheduler  |  SCSystemScheduler.cc  | 12. Jul 1994  |   SCL        |      |
+---------------+------------------------+---------------+--------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     04.06.96    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#include "SCSchedulerTypes.h"

#if (_SC_THREAD_TYPE == SCMacintoshThreads)

#include <Threads.h>

#include "SCDebug.h"


SCSystemScheduler::SCSystemScheduler (void):
  SCSystemThread(SC_SCHEDULER, NULL)
{
  // do nothing
}


SCSystemScheduler::~SCSystemScheduler (void)
{
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " in     "
       << "SCSystemScheduler::~SCSystemScheduler ()" << endl;
#endif
}


void SCSystemScheduler::Suspend(void)
{
  OSErr error;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCSystemScheduler::Suspend"
       << " (" << Identity() << ")" << endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity());            // Only scheduler thread may
                                                  // run in this code!
  error = SetThreadState (threadID, kStoppedThreadState, kNoThreadID);
  assert (error == noErr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCSystemScheduler::Suspend"
       << " (" << Identity() << ")" << endl;
#endif
}

void SCSystemScheduler::Resume(void)
{
  OSErr error;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCSystemScheduler::Resume"
       << " (" << Identity() << ")" << endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() != Identity());            // Scheduler thread must NEVER
                                                  // run in this code!
  error = SetThreadState (threadID, kReadyThreadState, kNoThreadID);
  assert (error == noErr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCSystemScheduler::Resume"
       << " (" << Identity() << ")" << endl;
#endif

}

#endif
