/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-95 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCMacThread   |  SCMacThread.cc   | 5. Jul 1994   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     05.07.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#include "SCThreadTypes.h"


#if (_SC_THREAD_TYPE == SCMacintoshThreads)

#include <Threads.h>
#include "SCScheduler.h"
#include "SCMainThreadTypes.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_PROFILER_SUPPORT
  #include <Threads.h>
  #include <profiler.h>
#endif


/*----- Konstruktoren / Destruktoren -----*/

SCSystemThread::SCSystemThread (void)  // only for Main-Thread
{
  OSErr   error;

  error = GetCurrentThread ((ThreadID *)&threadID);
  assert (error == noErr);

#if _SC_PROFILER_SUPPORT
/* Set the thread switching procs for it */
  error = SetThreadSwitcher ((ThreadID)threadID, (ThreadSwitchProcPtr)SwitchIn,
                             this, true);
  assert (error == noErr);

  error = SetThreadSwitcher ((ThreadID)threadID, (ThreadSwitchProcPtr)SwitchOut,
                             this, false);
  assert (error == noErr);

/* Get Thread Reference of Main Thread for the Profiler */
  profilerThreadID = ProfilerGetMainThreadRef();
#endif
}

SCSystemThread::SCSystemThread (const SCObjectType newType,
                                const SCObject* pParent):
  SCThread(newType, pParent)
{
/* Mac: OSErr NewThread()
              threadStyle (kCooperativeThread)
              threadEntry (SCSystemThread::Execute)
              threadParam (this)
              stackSize (0L == default)
              options (0)
              threadResult (NULL == none)
              threadMade (&threadID)
*/
  OSErr   error;

  error = NewThread (kCooperativeThread, (ThreadEntryProcPtr)Execute,
                     this, 0L, 0, NULL, (ThreadID *)&threadID);
  assert (error == noErr);

#if _SC_PROFILER_SUPPORT
/* We've created a new thread. Perform set up for the profiler.
 * First set the thread switching procs for it.
 */
  error = SetThreadSwitcher ((ThreadID)threadID, (ThreadSwitchProcPtr)SwitchIn,
                             this, true);
  assert (error == noErr);
  error = SetThreadSwitcher ((ThreadID)threadID, (ThreadSwitchProcPtr)SwitchOut,
                             this, false);
  assert (error == noErr);

/* Tell Profiler about this thread */
  {
    SCNatural stackSize;

    ThreadCurrentStackSpace ((ThreadID)threadID, &stackSize);
    assert (error == noErr);

    error = ProfilerCreateThread (stackSize / 64, stackSize, &profilerThreadID);
    assert (error == noErr);
  }
#endif
/* After creating the new thread, start it. It will be stopped when reaching the
 * Suspend-call in Execute.
 */
  error = YieldToThread (threadID);
  assert (error == noErr);
}


SCSystemThread::~SCSystemThread (void)
{
#if _SC_PROFILER_SUPPORT
/* We're about to delete this thread. Tell it to the Profiler. */
  ProfilerDeleteThread (profilerThreadID);
#endif
}


/*----- Member-Funktionen -----*/

void SCSystemThread::Cancel (void)
{
/* Mac: OSErr DisposeThread()
              threadToDump (threadID)
              threadResult (0L)
              recycleThread (false)
*/
  OSErr   error;

  if (ExecutingThreadID() == Identity())          // doing suicide ?
  {
    SCScheduler::GetScheduler()->Resume();        // activating Scheduler to
                                                  // prevent deadlock!
  }
  error = DisposeThread (threadID, 0L, false);
  assert (error == noErr);
}


void SCSystemThread::Join (void)
{
/* Mac: At this time no join mechanism */
}


SCThreadID SCSystemThread::ExecutingThreadID (void)
{
  ThreadID    currentThreadID;
  OSErr       error;

/* Mac: OSErr GetCurrentThread()
              currentThreadID
*/
  error = GetCurrentThread (&currentThreadID);
  if (error)
    currentThreadID = kNoThreadID;
  if (currentThreadID == kNoThreadID)
  {
    #if _SC_THREAD_DEBUG
      scThreadDebugLog << "WARNING: SCSystemThread::ExecutingThreadID: currentThreadID == kNoThreadID, error == "
           << error << endl;
    #else
      assert (currentThreadID != kNoThreadID);
    #endif
  }
  return (currentThreadID);
}


/*----- Statische Funktionen -----*/

void SCSystemThread::Initialize (void)
{
  MaxApplZone();
}


pascal void *SCSystemThread::Execute (SCSystemThread *thread)
{
  OSErr error;
                                                  // This is the start function of all
                                                  // newly created threads.
  if (thread->Identity() != 0)                    // may not be initialized yet!
    assert(ExecutingThreadID() == thread->Identity());

  error = SetThreadTerminator (thread->Identity(),
                               (ThreadTerminationProcPtr)CleanUp,
                               thread);
  assert (error == noErr);

  error = SetThreadState (thread->Identity(), kStoppedThreadState, kNoThreadID);
  assert (error == noErr);

  thread->Body();
  return (NULL);
}


pascal void *SCSystemThread::CleanUp (SCThreadID thread, SCSystemThread *toDelete)
{
  (void)thread;

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCSystemThread::CleanUp"
       << " (" << thread << ", " << toDelete->Identity() << ")" << endl;
#endif

  delete toDelete;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCSystemThread::CleanUp"
       << " (" << thread << ", ...)" << endl;
#endif

  return NULL;
}

#if _SC_PROFILER_SUPPORT

#pragma profile off

pascal void SCSystemThread::SwitchIn (
  SCThreadID       thread,
  SCSystemThread * myself)
{
  (void)thread;

  ProfilerSwitchToThread (myself->profilerThreadID);
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << thread << " in     "
       << "SCSystemThread::SwitchIn (void)" << endl;
#endif
}

pascal void SCSystemThread::SwitchOut (
  SCThreadID       thread,
  SCSystemThread * myself)
{
  (void)thread;
  (void)myself;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << thread << " in     "
       << "SCSystemThread::SwitchOut (void)" << endl;
#endif
}


#endif

#endif  // _SC_THREAD_TYPE == SCMacintoshThreads
