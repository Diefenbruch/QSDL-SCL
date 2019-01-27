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
| SCMachineInf  |  SCMachineInf.cpp | 03. Nov 1996  |   SCL             |      |
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
#include "SCMachineInf.h"

#include <float.h>

#include "SCStream.h"
#include "SCScheduler.h"
#include "SCRequest.h"
#include "SCMem.h"

#include "SCTraceControl.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachineInf::SCMachineInf (SCRunnableID pMachineID,
                            SCDiscipline pDiscipline,
                            SCNatural pServers,
                            SCNatural pServices,
                            const SCDuration *const pSpeeds,
                            const char *pName,
                            SCObject *father) :
  SCMachine (pMachineID, pDiscipline,
             pServers, pServices, pSpeeds, pName,
             father),
  relativeSpeed(1.0)
{
}


SCMachineInf::~SCMachineInf (void)
{
}


void SCMachineInf::StartRequest (SCRequest * request)
{
#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " is starting ";
  scDebugLog << *request << ", amount: " << request->GetRemainingService();
  scDebugLog << ", speed: " << Speed(request->GetRequestType()) << std::endl;
#endif

  request->serviceStartTime = SCScheduler::GetCurrentTime();

  GetServiceQueue()->InsertAfter(request);

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceServiceStart, this, request);
}


void SCMachineInf::UpdateWork (void)
{
  SCDuration    timeSpent;
  SCDuration    timeWorked;
  SCRequestIter iter (*GetServiceQueue());
  SCRequest *   request;

  timeSpent = SCScheduler::GetCurrentTime() - lastTime;
  timeWorked = timeSpent * relativeSpeed;

  for (request = iter++;
       request;
       request = iter++)
  {
    request->remainingTime -= timeWorked;
    request->remainingService -= timeWorked * Speed (request->GetRequestType());
  }

  lastTime = SCScheduler::GetCurrentTime();

#if _SC_VALIDATION_OPTIMIZE >= 2
  GetServiceQueue()->SetModified(kSCQueueCorrupted);
#endif
}


void SCMachineInf::RecalcTimes (void)
{
  SCRequestIter iter (*GetServiceQueue());
  SCRequest *   request;

  if (NumOfServers() != kSCInfiniteServers &&            // PS Machine and not
      GetServiceQueue()->NumOfElems() > NumOfServers())  // enough servers for
  {                                                      // all requests?
    relativeSpeed = (SCDuration)NumOfServers() /         // share servers
                    (SCDuration)GetServiceQueue()->NumOfElems();
  }
  else  // IS Machine or each request has an own server
  {
    relativeSpeed = 1.0;
  }

  for (request = iter++;
       request;
       request = iter++)
  {
    request->remainingTime = request->GetRemainingService() /
                             (Speed (request->GetRequestType()) * relativeSpeed);
  }

#if _SC_VALIDATION_OPTIMIZE >= 2
  GetServiceQueue()->SetModified(kSCQueueCorrupted);
#endif
}


void SCMachineInf::Body (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Executing " << *this << std::endl;

  while (true)
  {
    /* Update all currently running requests */
    UpdateWork();

    /* Remove finished requests */
    FinishRequests();

    /* Recalculate work for remaining requests */
    if (!GetServiceQueue()->IsEmpty())
      RecalcTimes();

    /* Determine next regular wakeup time */
    Reschedule();
    
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCMachineInf::Body(): " << *this;
    scValidationDebugLog << " suspending" << std::endl;
#endif

    Suspend();

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCMachineInf::Body(): " << *this;
    scValidationDebugLog << " resuming" << std::endl;
#endif
  }
}


/* Validation stack and display functions */

void SCMachineInf::Size(SCSize *curSize) const
{
  SCMachine::Size(curSize);                   // Get size of base class.

  curSize->historySize += sizeof(SCReal);     // Member relativeSpeed.
}


SCBoolean SCMachineInf::Save (SCMem& saveArea) const
{
  SCMachine::Save (saveArea);
  saveArea.HistoryStore(&relativeSpeed, sizeof(SCReal));

  return true;
}


SCBoolean SCMachineInf::Restore (SCMem &saveArea)
{
  SCMachine::Restore (saveArea);
  saveArea.HistoryRestore(&relativeSpeed, sizeof(SCReal));

  return true;
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachineInf& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachineInf::Display (SCStream& pStream) const
{
  return SCMachine::Display(pStream);
}
