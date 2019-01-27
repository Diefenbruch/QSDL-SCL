/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+-----------------+-------------------------+---------------+--------------+---+
|   Module        |   File                  |   Created     |   Project    |   |
+-----------------+-------------------------+---------------+--------------+---+
|SCSystemScheduler|SCPosixSystemScheduler.cc| 12. Jul 1994  |   SCL        |   |
+-----------------+-------------------------+---------------+--------------+---+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     04.06.96     Neu angelegt                                          |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#include "SCSchedulerTypes.h"
#include "SCDebug.h"

#if (_SC_THREAD_TYPE == SCPosixThreads)

#if _SC_THREAD_DEBUG
  #include "SCStream.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCSystemScheduler::SCSystemScheduler (void):
  SCSystemThread(SC_SCHEDULER, NULL)
{
  // do nothing
}


SCSystemScheduler::~SCSystemScheduler (void)
{
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemScheduler::~SCSystemScheduler." << std::endl;
#endif

  pthread_detach ((pthread_t) threadID);      // For stack memory cleanup
                                              // since no pthread_join
                                              // will take place on
                                              // scheduler. Remove this
                                              // if Main-Threads joins
                                              // on scheduler!
}

void SCSystemScheduler::Suspend(void)
{
  pthread_mutex_lock(&mutexSleep);                // Aquire lock.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemScheduler::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity());
                                                  // Only scheduler thread may
                                                  // run in this code!
  runIt--;                                        // Decrement runIt.
  while(!runIt)
    pthread_cond_wait(&condSleep, &mutexSleep);   // Wait until signaled.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl
       << "     Identity = " << Identity() << std::endl
       << "     exiting SCSystemScheduler::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  pthread_mutex_unlock(&mutexSleep);              // Free mutex and go on.
}


void SCSystemScheduler::Resume(void)
{
  pthread_mutex_lock(&mutexSleep);                // Aquire lock.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemScheduler::Resume (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() != Identity());
                                                  // Scheduler thread must NEVER
                                                  // run in this code!
  runIt++;                                        // Set runIt to true.
  pthread_cond_signal(&condSleep);                // Signal scheduler that it is
                                                  // time to go on.
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCSystemScheduler::Resume (runIt = " << runIt << ")." << std::endl;
#endif

  pthread_mutex_unlock(&mutexSleep);              // Free mutex and go on.
                                                  // Normally, this code is
                                                  // called from the threads
                                                  // Suspend method. Thus, after
                                                  // completing the call to
                                                  // SCSystemScheduler::Resume,
                                                  // the thread will set itself
                                                  // to sleep.
}

#endif
