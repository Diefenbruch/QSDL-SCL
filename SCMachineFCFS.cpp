/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+------------------+-------+
|   Module      |   File            |   Created     |   Project        |       |
+---------------+-------------------+---------------+------------------+-------+
| SCMachineFCFS | SCMachineFCFS.cpp | 13. Jul 1994  |   SCL            |       |
+---------------+-------------------+---------------+------------------+-------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     11.07.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

/* Maschine mit Strategie "First-Come First-Serve"
*  NewRequest() setzt die Prioritaet immer auf 0, damit werden die Prioritaeten unter-
*  drueckt.
*
*  Die Requests werden nach ihrer Prioritaet sortiert in eine Warteschlange einge-
*  reiht. Wenn ein Bediener frei ist, wird das erste Element der Warteschlange
*  bearbeitet. Die Bearbeitung eines Request kann nicht durch einen anderen an-
*  kommenden Request mit hoeherer Prioritaet unterbrochen werden.
*/

#include <string.h>

#include "SCMachineFCFS.h"
#include "SCAutomaton.h"
#include "SCScheduler.h"
#include "SCRequest.h"
#include "SCRequestType.h"

#include "SCTraceControl.h"

#include <float.h>

#include "SCStream.h"
#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachineFCFS::SCMachineFCFS (SCRunnableID pMachineID,
                              SCNatural pServers,
                              SCNatural pServices,
                              const SCDuration *const pSpeeds,
                              const char *pName,
                              SCObject *father) :
  SCMachineLtd (pMachineID, scDiscFCFS, pServers, pServices,
                pSpeeds, pName,
                father)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Creating " << *this << std::endl;

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " created" << std::endl;
#endif
}


SCMachineFCFS::~SCMachineFCFS (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;
}


void SCMachineFCFS::EnqueueRequest (SCRequest *request)
{
#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " is queueing ";
  scDebugLog << *request << std::endl;
#endif

  GetWaitingQueue()->InsertAfter (request);

  request->SetWaitStartTime(SCScheduler::GetCurrentTime());
}


void SCMachineFCFS::NewRequest (SCAutomaton *caller,
                                const SCRequestType *serviceType,
                                SCDuration serviceAmount,
                                SCNatural priority)
{
  SCRequest * request;

  priority = 0;                              // this machine type does
                                             // not support priorities!
  request = new SCRequest (serviceType,
                           serviceAmount,
                           priority,
                           caller,
                           serviceAmount / Speed (serviceType));
  assert (request);

#if _SC_VALIDATION_OPTIMIZE
  SetModified();
#endif

    /* Enter the new request into the waiting room of the machine */
  EnqueueRequest (request);                // request must wait!
  
  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceServiceRequest, this, request);

  if (NumOfFreeServers() > 0)               // free server available ?
  {
    /* Update all currently running requests before inserting the new one */
    UpdateWork();

    /* Remove finished requests */
    FinishRequests();

    /* Enter the new request into the service room of the machine */
    StartRequest();

    /* Determine next regular wakeup time */
    Reschedule();
  }
}


/* Validation stack and display functions */

void SCMachineFCFS::Size(SCSize *curSize) const
{
  SCMachineLtd::Size(curSize);
}


SCBoolean SCMachineFCFS::Save (SCMem& saveArea) const
{
  SCMachineLtd::Save (saveArea);

  return true;
}


SCBoolean SCMachineFCFS::Restore (SCMem &saveArea)
{
  SCMachineLtd::Restore (saveArea);

  return true;
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachineFCFS& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachineFCFS::Display (SCStream& pStream) const
{
  pStream << "fcfs ";
  
  return SCMachineLtd::Display(pStream);
}
