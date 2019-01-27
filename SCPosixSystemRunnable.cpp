/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                 |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------------+------------------------+---------------+---------------+
|   Module            |   File                 |   Created     |   Project     |
+---------------------+------------------------+---------------+---------------+
|SCPosixSystemRunnable|SCPosixSystemRunnable.cc| 7. Mai 1995   |   SCL         |
+---------------------+------------------------+---------------+---------------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     07.05.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCRunnableTypes.h"

#if (_SC_THREAD_TYPE == SCPosixThreads)

#include "SCScheduler.h"
#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_THREAD_DEBUG
  #include "SCStream.h"
#endif

SCThread * SCSystemRunnable::interactionThread = NULL;


/*----- Konstruktoren / Destruktoren -----*/

SCSystemRunnable::SCSystemRunnable (const SCObjectType newType,
                                    const SCObject* pParent):
  SCSystemThread(newType, pParent)
{
  // Do nothing
}


SCSystemRunnable::~SCSystemRunnable (void)
{
  // Do nothing
}


/*----- Member-Funktionen -----*/

void SCSystemRunnable::Suspend (void)
{
  pthread_mutex_lock(&mutexSleep);                // Aquire lock.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemRunnable::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity());
                                                  // Only the owner thread
                                                  // may run in this code!

  runIt--;                                        // Decrement runIt to false.
  if (!runIt)                                     // will executing thread block
  {                                               // at while below?
    assert(interactionThread);                    // Wake up scheduler or main thread
    interactionThread->Resume();                  // to prevent deadlock!
  }

  while (!runIt)
    pthread_cond_wait(&condSleep, &mutexSleep);   // Wait until signalled.
                                                  // THIS IS NOT BUSY WAITING!
                                                  // C.f. pthread documentation. 
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCSystemRunnable::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  pthread_mutex_unlock(&mutexSleep);              // Free lock and go on.
}


void SCSystemRunnable::Resume (void)
{
  pthread_mutex_lock(&mutexSleep);                // Aquire lock.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemRunnable::Resume (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() != Identity());
                                                  // Owner thread must NEVER
                                                  // run in this code!
  runIt++;                                        // Increment runIt
  pthread_cond_signal(&condSleep);                // Signal thread to go on.
  pthread_mutex_unlock(&mutexSleep);              // Free lock and go on.

  // HERE WAS THE PROBLEM !!!!! (with the mutex_unlock above
  // the thread of this object can go on and do MANY activities
  // before the scheduler thread executes the following actions.
  // This activities of the runnable thread can cause a resume of
  // the scheduler thread (which may not be suspended) so we MUST
  // use an integer for runIt to buffer resumes for not suspended
  // threads. This gurantees that the following Suspend
  // does not block the Scheduler which would cause a deadlock
  // in cases where a resume for scheduler has already be done)

  assert(interactionThread);
  interactionThread->Suspend();                   // Set scheduler or
                                                  // main thread to sleep if its
                                                  // not already resumed
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCSystemRunnable::Resume (runIt = " << runIt << ")." << std::endl;
#endif
}

/* static members */

SCThread *SCSystemRunnable::GetInteractionThread(void)
{
  return interactionThread;
}


void SCSystemRunnable::SetInteractionThread(SCThread *iThread)
{
  assert(iThread);
  interactionThread = iThread;
}

#endif
