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
|  SCMachinePS  |   SCMachinePS.cpp | 11. Jul 1994  |   SCL             |      |
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

/* Maschine mit Strategie "Processor Sharing"
*  Alle Requests werden sofort bearbeitet. Die verfuegbaren Server werden ohne Be-
*  ruecksichtigung von Prioritaeten gleichmaessig auf die zu bearbeitenden Requests
*  aufgeteilt.
*/

#include <string.h>
#include "SCMachinePS.h"

#include <float.h>

#include "SCStream.h"
#include "SCScheduler.h"
#include "SCRequest.h"
#include "SCRequestType.h"

#include "SCTraceControl.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCMachinePS::SCMachinePS (SCRunnableID pMachineID,
                          SCNatural pServers,
                          SCNatural pServices,
                          const SCDuration *const pSpeeds,
                          const char *pName,
                          SCObject *father) :
  SCMachineInf (pMachineID, scDiscPS, pServers, pServices,
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


SCMachinePS::~SCMachinePS (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;
}


void SCMachinePS::NewRequest (SCAutomaton *caller,
                              const SCRequestType *serviceType,
                              SCDuration serviceAmount,
                              SCNatural priority)
{
  SCRequest * request;
  
  request = new SCRequest (serviceType,
                           serviceAmount,
                           priority,
                           caller,
                           0.0);
  assert (request);

#if _SC_VALIDATION_OPTIMIZE
  SetModified();
#endif

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceServiceRequest, this, request);

  /* Update all currently running requests before inserting the new one */
  UpdateWork();

  /* Remove finished requests */
  FinishRequests();
  
  /* Enter the new request into the machine */
  StartRequest (request);

  /* Recalculate real exec times for all requests */
  RecalcTimes();

  /* Determine next regular wakeup time */
  Reschedule();
}


/* Validation stack and display functions */

void SCMachinePS::Size(SCSize *curSize) const
{
  SCMachineInf::Size(curSize);                   // Get size of base class.
}


SCBoolean SCMachinePS::Save (SCMem& saveArea) const
{
  return SCMachineInf::Save (saveArea);
}


SCBoolean SCMachinePS::Restore (SCMem &saveArea)
{
  return SCMachineInf::Restore (saveArea);
}


SCStream& operator<< (SCStream& pStream,
                     const SCMachinePS& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachinePS::Display (SCStream& pStream) const
{
  pStream << "ps ";
  
  return SCMachineInf::Display(pStream);
}
