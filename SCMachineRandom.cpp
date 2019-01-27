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
|SCMachineRandom|SCMachineRandom.cpp| 13. Jul 1994  |   SCL             |      |
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

/* Maschine mit Bearbeitungs-Strategie "First-Come First-Serve", und zufaelliger
*  Reihenfolge in der Warteschlage
*
*  Die Requests werden zufaellig in eine Warteschlange eingereiht. Wenn ein Bediener
*  frei ist, wird das erste Element der Warteschlange bearbeitet.
*/

#include <string.h>
#include <float.h>

#include "SCStream.h"
#include "SCMachineRandom.h"
#include "SCIndet.h"
#include "SCAutomaton.h"
#include "SCRandUniform.h"
#include "SCRequest.h"
#include "SCScheduler.h"
#include "SCRequestType.h"

#include "SCTraceControl.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachineRandom::SCMachineRandom (SCRunnableID pMachineID,
                                  SCNatural pServers,
                                  SCNatural pServices,
                                  const SCDuration *const pSpeeds,
                                  const char *pName,
                                  SCObject *father) :
  SCMachineLtd (pMachineID, scDiscRandom, pServers, pServices,
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


SCMachineRandom::~SCMachineRandom (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;
}


void SCMachineRandom::EnqueueRequest (SCRequest * request)
{
  SCRequestCons * loop;
  SCNatural       insertPos;

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
  scDebugLog << " is queueing ";
  scDebugLog << *request << std::endl;
#endif

  if (!GetWaitingQueue()->IsEmpty())
  {
    loop = NULL;
    insertPos = SCScheduler::GetIndet()->NonDetChoice (GetWaitingQueue()->NumOfElems());
    if (insertPos)
    {
      for (loop = GetWaitingQueue()->Head();
           loop && --insertPos;
           loop = loop->Next()) {}
    }
    GetWaitingQueue()->InsertBefore(request, loop);
  }
  else
  {
    GetWaitingQueue()->InsertAfter(request);
  }

  request->SetWaitStartTime(SCScheduler::GetCurrentTime());
}


void SCMachineRandom::NewRequest (SCAutomaton *caller,
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
  EnqueueRequest (request);
  
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

void SCMachineRandom::Size(SCSize *curSize) const
{
  SCMachineLtd::Size(curSize);
}


SCBoolean SCMachineRandom::Save (SCMem& saveArea) const
{
  SCMachineLtd::Save (saveArea);

  return true;
}


SCBoolean SCMachineRandom::Restore (SCMem &saveArea)
{
  SCMachineLtd::Restore (saveArea);

  return true;
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachineRandom& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachineRandom::Display (SCStream& pStream) const
{
  pStream << "random ";
  
  return SCMachineLtd::Display(pStream);
}
