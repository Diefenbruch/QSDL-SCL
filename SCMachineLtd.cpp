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
| SCMachineLtd  |   SCMachineLtd.cc | 03. Nov 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     03.11.96    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include <string.h>
#include "SCMachineLtd.h"

#include <float.h>

#include "SCStream.h"
#include "SCAutomaton.h"
#include "SCScheduler.h"
#include "SCRequest.h"
#include "SCMem.h"

#include "SCTraceControl.h"

#if _SC_DEBUGOUTPUT
  #include "SCDebug.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachineLtd::SCMachineLtd (SCRunnableID pMachineID,
                            SCDiscipline pDiscipline,
                            SCNatural pServers,
                            SCNatural pServices,
                            const SCDuration *const pSpeeds,
                            const char *pName,
                            SCObject *father) :
  SCMachine (pMachineID, pDiscipline,
             pServers, pServices, pSpeeds, pName,
             father)
{
  waitingQueue = new SCRequestSaveList(true, this);
  assert(waitingQueue);
}


SCMachineLtd::~SCMachineLtd (void)
{
  if (waitingQueue)
    delete waitingQueue;
}


void SCMachineLtd::StartRequest (void)
{
  // We always take the first request in
  // the waiting queue, independent of
  // the service discipline of the machine.
  // This is due to the fact, that the
  // coorect ordering of requests in the
  // waiting room is done by EnqueueRequest() !

  SCRequest * request = GetWaitingQueue()->Remove(); // take first elem

  assert(request);

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " is starting ";
  scDebugLog << *request << ", amount: " << request->GetRemainingService();
  scDebugLog << ", speed: " << Speed(request->GetRequestType()) << std::endl;
#endif

  assert(NumOfFreeServers() > 0);

  request->serviceStartTime = SCScheduler::GetCurrentTime();

  GetServiceQueue()->InsertAfter(request);

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceServiceStart, this, request);
}


void SCMachineLtd::UpdateWork (void)
{
  SCDuration    timeSpent;
  SCRequest *   request;
  SCRequestIter iter (*GetServiceQueue());

  timeSpent = SCScheduler::GetCurrentTime() - lastTime;

  for (request = iter++;
       request;
       request = iter++)
  {
    request->remainingTime -= timeSpent;
  }

  lastTime = SCScheduler::GetCurrentTime();

#if _SC_VALIDATION_OPTIMIZE >= 2
  GetServiceQueue()->SetModified(kSCQueueCorrupted);
#endif
}


SCBoolean SCMachineLtd::InterruptRequest (SCRequest * request, SCBoolean lcfs)
{
  SCRequest *     curRequest;
  SCRequest *     interrupted = NULL;
  SCRequestIter iter (*GetServiceQueue());

  // find request with minimum priority:
  for (curRequest = iter++;
       curRequest;
       curRequest = iter++)
  {
    if (!interrupted ||
        curRequest->GetPriority() < interrupted->GetPriority())
    {
      interrupted = curRequest;
    }
  }

  if (interrupted->GetPriority() < request->GetPriority() ||
      (lcfs && interrupted->GetPriority() == request->GetPriority()))
  {
    GetServiceQueue()->Remove(interrupted);

    EnqueueRequest (interrupted);

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceServiceInterrupt, this, interrupted);

    return true;
  }
  else
    return false;
}


void SCMachineLtd::Body (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Executing " << *this << std::endl;

  while (true)
  {
    // 1. Decrement remaining service times:
    UpdateWork();

    // 2. Test if some requests have gained their complete service time:
    FinishRequests();

    // 3. Allow requests from waiting room to enter a free server (if
    //    possible)
    while (!GetWaitingQueue()->IsEmpty() &&
           NumOfFreeServers() > 0)
    {
      StartRequest();       // start first element in waiting room!
    }

    // 4. Calculate the next time the machines gets active:
    Reschedule();

    // 5. Suspending machine (scheduler gets active):
    Suspend();

  } // while(true)
}


/* Validation stack and display functions */

void SCMachineLtd::Size (SCSize *curSize) const
{
  SCMachine::Size(curSize);

  waitingQueue->Size(curSize);
}


SCBoolean SCMachineLtd::Save (SCMem& saveArea) const
{
  SCMachine::Save (saveArea);

  assert(waitingQueue);
  waitingQueue->Save(saveArea);

  return true;
}


SCBoolean SCMachineLtd::Restore (SCMem &saveArea)
{
  SCMachine::Restore (saveArea);

  assert(waitingQueue);
  waitingQueue->Restore (saveArea);

  return true;
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachineLtd& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachineLtd::Display (SCStream& pStream) const
{
  return SCMachine::Display(pStream);
}
