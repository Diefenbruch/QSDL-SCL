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
| SCPosixThread |  SCPosixThread.cc | 7. Mai 1995   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
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

#if (_SC_THREAD_TYPE == SCMacintoshThreads)

#include <Threads.h>

#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_THREAD_DEBUG
  #include "SCDebug.h"
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
  OSErr error;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCSystemRunnable::Suspend"
       << " (" << Identity() << ")" << endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() == Identity());            // Only the owner thread
                                                  // may run in this code!
  assert(interactionThread);                    // Wake up scheduler or main thread
  interactionThread->Resume();                  // to prevent deadlock!

  error = SetThreadState (threadID, kStoppedThreadState, kNoThreadID);
  assert (error == noErr);

#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCSystemRunnable::Suspend"
       << " (" << Identity() << ")" << endl;
#endif

}


void SCSystemRunnable::Resume (void)
{
  OSErr error;
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " enters "
       << "SCSystemRunnable::Resume"
       << " (" << Identity() << ")" << endl;
#endif

  assert(SCSystemThread::ExecutingThreadID() != Identity());
                                                  // Owner thread must NEVER
                                                  // run in this code!
  
  error = SetThreadState (threadID, kReadyThreadState, kNoThreadID);
  assert (error == noErr);

  assert(interactionThread);
  interactionThread->Suspend();                   // Set scheduler or
                                                  // main thread to sleep if its
                                                  // not already resumed
#if _SC_THREAD_DEBUG
  scThreadDebugLog << "ThreadID #" << ExecutingThreadID() << " exits  "
       << "SCSystemRunnable::Resume"
       << " (" << Identity() << ")" << endl;
#endif

}

/* static members */

_SCINLINE_ SCThread *SCSystemRunnable::GetInteractionThread(void)
{
  return interactionThread;
}

_SCINLINE_ void SCSystemRunnable::SetInteractionThread(SCThread *iThread)
{
  assert(iThread);
  interactionThread = iThread;
}

#endif
