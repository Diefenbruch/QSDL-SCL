/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-----------------+--------------------+---------------+-------------------+---+
|   Module        |   File             |   Created     |   Project         |   |
+-----------------+--------------------+---------------+-------------------+---+
|SCPosixMainThread|SCPosixMainThread.cc| 04.06.1996    |   SCL             |   |
+-----------------+--------------------+---------------+-------------------+---+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     04.06.1996   Neu angelegt                                          |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCThreadTypes.h"

#if (_SC_THREAD_TYPE == SCPosixThreads)

#include "SCMainThreadTypes.h"
#include "SCRunnableTypes.h"
#include "SCScheduler.h"
#include "SCDebug.h"

#if _SC_THREAD_DEBUG
  #include "SCStream.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCMainThread *SCMainThread::mainThread = NULL;


SCMainThread::SCMainThread (void)                 // calls default constructor
{                                                 // of SCSystemThread
  // do nothing
}


SCMainThread::~SCMainThread (void)                // calls default destructor
{                                                 // of SCSystemThread
  // do nothing
}


void SCMainThread::Initialize (void)              // Set up the thread system
{
  mainThread = new SCMainThread;
  assert(mainThread);
                                                  // associate main thread with
                                                  // thread data structure as to
                                                  // enable the resume/suspend mechanism.
  SCSystemRunnable::SetInteractionThread(mainThread);
                                                  // inform all system runnables
                                                  // that they should interact with
                                                  // main thread. this is changed
                                                  // in SCScheduler::Run()
}


void SCMainThread::Finish (void) // static
{
  if (mainThread)
    delete mainThread;
}


void SCMainThread::Suspend (void)
{
  pthread_mutex_lock(&mutexSleep);                // Aquire lock.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCMainThread::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity());
                                                  // Only the owner thread
                                                  // may run in this code!

  runIt--;                                        // Decrement runIt.
  while (!runIt)
    pthread_cond_wait(&condSleep, &mutexSleep);   // Wait until signalled.
                                                  // THIS IS NOT BUSY WAITING!
                                                  // C.f. pthread documentation.
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCMainThread::Suspend (runIt = " << runIt << ")." << std::endl;
#endif

  pthread_mutex_unlock(&mutexSleep);              // Free lock and go on.
}


void SCMainThread::Resume (void)
{
  pthread_mutex_lock (&mutexSleep);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCMainThread::Resume (runIt = " << runIt << ")." << std::endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() != Identity());
                                                  // Main thread must NEVER
                                                  // run in this code!
  runIt++;
  pthread_cond_signal (&condSleep);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCMainThread::Resume (runIt = " << runIt << ")." << std::endl;
#endif

  pthread_mutex_unlock (&mutexSleep);
}

#endif
