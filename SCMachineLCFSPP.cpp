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
|SCMachineLCFSPP| SCMachineLCFSPP.cc| 13. Jul 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
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

/* Maschine mit Strategie "Last-Come First-Serve, Preemptive-Resuming"
*  Requests werden in umgekehrter Reihenfolge der Ankunft sortiert nach Prioritaeten
*  in eine Warteschlange eingereiht. Wenn ein Request mit hoeherer oder gleicher
*  Prioritaet ankommt, wird der Prozess mit der niedrigsten Prioritaet unterbrochen
*  und als erster seiner Prioritaet in die Warteschlange eingereiht. Wenn
*  mehrere Requests unterbrochen werden koennen, wird der aelteste Request unterbrochen.
*/

#include <string.h>
#include <float.h>

#include "SCStream.h"
#include "SCMachineLCFSPP.h"
#include "SCAutomaton.h"
#include "SCScheduler.h"
#include "SCRequest.h"
#include "SCRequestType.h"

#include "SCTraceControl.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachineLCFSPP::SCMachineLCFSPP (SCRunnableID pMachineID,
                                 SCNatural pServers,
                                 SCNatural pServices,
                                 const SCDuration *const pSpeeds,
                                 const char *pName,
                                 SCObject *father) :
  SCMachineLtd (pMachineID, scDiscLCFSPP, pServers, pServices,
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


SCMachineLCFSPP::~SCMachineLCFSPP (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;
}


void SCMachineLCFSPP::EnqueueRequest (SCRequest *request)
{
  SCRequestIter iter (*GetWaitingQueue());
  SCRequest *   curRequest;

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " is queueing ";
  scDebugLog << *request << std::endl;
#endif

  for (curRequest = iter++;
       curRequest;
       curRequest = iter++)
  {
    if (curRequest->GetPriority() <= request->GetPriority())
      break;
  }

  if (curRequest)
    GetWaitingQueue()->InsertBefore (request, curRequest->GetContainer());
  else
    GetWaitingQueue()->InsertAfter (request); // special case for append to queue

  request->SetWaitStartTime(SCScheduler::GetCurrentTime());
}


void SCMachineLCFSPP::NewRequest (SCAutomaton *caller,
                                  const SCRequestType *serviceType,
                                  SCDuration serviceAmount,
                                  SCNatural priority)
{
  SCRequest *request;
  
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
  EnqueueRequest (request);

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceServiceRequest, this, request);

  if (NumOfFreeServers() > 0 ||           // service room not full?
      InterruptRequest (request, true))   // or request with lower prio?
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

void SCMachineLCFSPP::Size(SCSize *curSize) const
{
  SCMachineLtd::Size(curSize);
}

SCBoolean SCMachineLCFSPP::Save (SCMem& saveArea) const
{
  SCMachineLtd::Save (saveArea);

  return true;
}


SCBoolean SCMachineLCFSPP::Restore (SCMem &saveArea)
{
  SCMachineLtd::Restore (saveArea);

  return true;
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachineLCFSPP& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachineLCFSPP::Display (SCStream& pStream) const
{
  pStream << "lcfs ";

  return SCMachineLtd::Display(pStream);
}
