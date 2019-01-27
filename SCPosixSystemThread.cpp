/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                 |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+----------------------+---------------+-------------------+
|   Module          |   File               |   Created     |   Project         |
+-------------------+----------------------+---------------+-------------------+
|SCPosixSystemThread|SCPosixSystemThread.cc| 7. Mai 1995   |   SCL             |
+-------------------+----------------------+---------------+-------------------+
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

#include "SCThreadTypes.h"

#if (_SC_THREAD_TYPE == SCPosixThreads)

#include "SCBasicTypes.h"
#include "SCThreadTypes.h"
#include "SCScheduler.h"
#include "SCMainThreadTypes.h"
#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_THREAD_DEBUG
  #include "SCStream.h"
#endif


/*----- Konstruktoren / Destruktoren -----*/

SCSystemThread::SCSystemThread (void)  // only for Main-Thread
{
  pthread_mutex_init (&mutexSleep, NULL);
  pthread_cond_init (&condSleep, NULL);
  runIt = 1;
  threadID = (SCThreadID)pthread_self();
}


SCSystemThread::SCSystemThread (const SCObjectType newType,
                                const SCObject* pParent):
  SCThread(newType, pParent)
{
  pthread_attr_t attr;

  int detached = PTHREAD_CREATE_JOINABLE;

  pthread_mutex_init (&mutexSleep, NULL);
  pthread_cond_init (&condSleep, NULL);

  pthread_mutex_lock (&mutexSleep);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID();
  scThreadDebugLog << " and " << std::endl;
  scThreadDebugLog << "     Identity = <not yet initialized>" << std::endl;
  scThreadDebugLog << "     running in SCSystemThread::SCSystemThread." << std::endl;
#endif

  runIt = 0;                                      // Default state is sleeping.
                                                  // This guarantees, that the
                                                  // thread will suspend itself
                                                  // in Execute and wait for its
                                                  // resumption by whichever thread
                                                  // is responsible for that
                                                  // (Main thread or scheduler).

  pthread_attr_init (&attr);
  
  pthread_attr_setdetachstate (&attr, detached);

  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create ((pthread_t *)&threadID,         // threadID is member of SCThread
                  &attr,
                  (void*(*)(void*))SCSystemThread::Execute, (void *)this);
  pthread_attr_destroy (&attr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID() << " and " << std::endl
       << "     Identity = " << Identity() << std::endl
       << "     exiting SCSystemThread::SCSystemThread." << std::endl;
#endif

  pthread_mutex_unlock (&mutexSleep);
}


SCSystemThread::~SCSystemThread (void)
{
}


/*----- Member-Funktionen -----*/

void SCSystemThread::Cancel (void)
{
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID() << " and " << std::endl
       << "     Identity = " << Identity() << std::endl
       << "     running in SCSystemThread::Cancel (runIt = "
       << runIt << ")." << std::endl;
#endif

  if (ExecutingThreadID() == Identity())          // doing suicide ?
  {
    pthread_detach ((pthread_t)threadID);         // this is very important
                                                  // for stack memory cleanup
                                                  // since no pthread_join
                                                  // will take place in this
                                                  // case!

    SCScheduler::GetScheduler()->Resume();        // activating Scheduler to
                                                  // prevent deadlock!

    pthread_exit (NULL);                          // this will not return!
  }
  else                                            // killing another thread
  {
    pthread_cancel ((pthread_t)threadID);         // Go for a clean POSIX-kill :-)
    pthread_join ((pthread_t)threadID, NULL);     // wait for end of canceled
                                                  // thread
  }

  // don't access any data members of this object here,
  // because it may already be deleted!
}

void SCSystemThread::Join (void)
{
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID();
  scThreadDebugLog << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemThread::Join (runIt = ";
  scThreadDebugLog << runIt << ")." << std::endl;
#endif

  assert (ExecutingThreadID() != Identity());     // no join on itself!

  pthread_join ((pthread_t)threadID, NULL);       // wait for end of thread of
                                                  // this object
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID();
  scThreadDebugLog << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << Identity() << std::endl;
  scThreadDebugLog << "     exiting SCSystemThread::Join (runIt = ";
  scThreadDebugLog << runIt << ")." << std::endl;
#endif
}


SCBoolean SCSystemThread::IsRunning(void) const
{
  return (runIt > 0) ? true : false;
}

/*----- Statische Funktionen -----*/

SCThreadID SCSystemThread::ExecutingThreadID (void)
{
  return ((SCThreadID)pthread_self());
}


void SCSystemThread::Initialize (void)            // Set up the thread system
{
}


void *SCSystemThread::Execute (SCSystemThread *thread)
{
                                                  // This is the start function of all
                                                  // newly created threads.
  if (thread->Identity() != 0)                    // may not be initialized yet!
    assert(ExecutingThreadID() == thread->Identity());
                                                  // Make sure, that the no thread
                                                  // other than the associated
                                                  // thread is running in this code!
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << thread->Identity() << std::endl;
  scThreadDebugLog << "     running in SCSystemThread::Execute (runIt = " << (int)thread->runIt << ")." << std::endl;
#endif

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  pthread_cleanup_push ((void (*)(void *)) SCSystemThread::CleanUp,
                        thread);                  // Push CleanUp handler
                                                  // to make sure that object
                                                  // is deleted on thread
                                                  // termination.
                                                  // The cast is necessary as
                                                  // the linux pthread lib
                                                  // does not automatically
                                                  // cast the parameters, as does
                                                  // the sun pthread lib.

  pthread_mutex_lock(&thread->mutexSleep);        // Protect access to runIt
  while (thread->runIt == 0)                      // resume increments runIt by one.
                                                  // Thus, if runIt is zero, no resume
                                                  // has taken place so far.
    pthread_cond_wait(&thread->condSleep,         // So, wait passively for resume!
                      &thread->mutexSleep);
  pthread_mutex_unlock(&thread->mutexSleep);      // free Mutex.

  thread->Body();                                 // enter Body-method.
                                                  // Body method cannot be entered
                                                  // directly,  as the creating thread
                                                  // MUST first finish up the constructor.

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     Identity = " << thread->Identity() << std::endl;
  scThreadDebugLog << "     exiting SCSystemThread::Execute." << std::endl;
#endif

  pthread_cleanup_pop (1);                        // Pop and execute CleanUp handler

  return (NULL);                                  // That's it, babe!
}


// this function is called by the thread when it is about to terminate.
// Be careful here! The function is executed in parallel
// to other threads, so don't place any code here
// which uses global variables (like static members)

void *SCSystemThread::CleanUp (SCSystemThread *toDelete)
{
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl
       << "     To delete = " << toDelete->Identity() << std::endl
       << "     running in SCSystemThread::CleanUp." << std::endl;
#endif

  delete toDelete;  // call virtual destructor

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "**** Thread with ID = " << SCSystemThread::ExecutingThreadID() << " and " << std::endl;
  scThreadDebugLog << "     exiting SCSystemThread::CleanUp." << std::endl;
#endif
  return NULL;
}

#endif  // _SC_THREAD_TYPE == SCPosixThreads
