/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                 |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCMainThread  |  SCMainThread.cc  | 04.06.1996    |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
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

#if (_SC_THREAD_TYPE == SCMacintoshThreads)

#include <Threads.h>

#include "SCMainThreadTypes.h"
#include "SCRunnableTypes.h"
#include "SCScheduler.h"

#if _SC_THREAD_DEBUG
  #include "SCDebug.h"
#endif

SCMainThread *SCMainThread::mainThread = NULL;

/*----- Main-Thread -----*/

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
  OSErr   error;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCMainThread::Suspend"
       << " (" << Identity() << ")" << endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity()); // Only the owner thread
                                                          // may run in this code!

//  SCScheduler::GetScheduler()->Resume();          // Wake up scheduler.
  error = SetThreadState (threadID, kStoppedThreadState, kNoThreadID);
  assert (error == noErr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCMainThread::Suspend"
       << " (" << Identity() << ")" << endl;
#endif
}

void SCMainThread::Resume (void)
{
  OSErr   error;

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCMainThread::Resume"
       << " (" << Identity() << ")" << endl;
#endif

  error = SetThreadState (threadID, kReadyThreadState, kNoThreadID);
  assert (error == noErr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCMainThread::Resume"
       << " (" << Identity() << ")" << endl;
#endif
}

#endif
