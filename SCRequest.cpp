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
|   SCRequest   |   SCRequest.cc    | 02. Mar 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCRequest.h"
#include "SCScheduler.h"
#include "SCProcess.h"
#include "SCRequestType.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCNatural SCRequest::nextRequestID = 1;


#if _SC_NOINLINES
  #include "SCRequest.inl.h"
#endif


/*----- Konstruktor -----*/

SCRequest::SCRequest (const SCRequestType *pServiceType,
                      SCDuration pServiceAmount, // only for PS
                      SCNatural pPriority,
                      SCAutomaton *pCaller,
                      SCDuration pServiceTime,
                      const SCObject* father) :
  SCObject(SC_REQUEST, father),
  priority(pPriority),
  caller(pCaller),
  remainingTime(pServiceTime),
  remainingService(pServiceAmount), // only for PS
  container(NULL)
{
  requestID = nextRequestID++;
  serviceType = (SCRequestType *)pServiceType;

  assert(pCaller);
  callerID = caller->GetID();

  creationTime = SCScheduler::GetCurrentTime();
  waitStartTime = 0.0;
  serviceStartTime = 0.0;
}


SCRequest::SCRequest (SCMem &saveArea, const SCObject *father):
  SCObject (SC_REQUEST, father),
  container(NULL)
{
  Restore (saveArea);
}


void SCRequest::Size(SCSize *curSize) const
{
  curSize->size += (sizeof(SCNatural) +               // ID of service type
                    sizeof(SCNatural) +               // priority
                    sizeof(SCProcessID));             // callerID

  curSize->historySize += (sizeof(SCRequestType *) +  // serviceType
                           sizeof(SCNatural) +        // requestID
                           sizeof(SCTime) +           // creationTime
                           sizeof(SCTime) +           // waitStartTime
                           sizeof(SCTime) +           // serviceStartTime
                           sizeof(SCDuration) +       // remainingTime
                           sizeof(SCDuration));       // remainingService
}


SCBoolean SCRequest::Save (SCMem &saveArea) const
{
  SCNatural key;

  saveArea.HistoryStore (&requestID, sizeof (SCNatural));

  // nur Zeiger sichern, da statisches Objekt!
  saveArea.HistoryStore (&serviceType, sizeof (SCRequestType *));

  // zusaetzlich ID des service types speichern, da
  // zustandsrelevant:
  key = serviceType->GetID();
  saveArea.Store(&key, sizeof(SCNatural));

  saveArea.Store(&priority, sizeof (SCNatural));

  saveArea.Store(&callerID, sizeof (SCProcessID));

  saveArea.HistoryStore (&creationTime, sizeof (SCTime));
  saveArea.HistoryStore (&waitStartTime, sizeof (SCTime));
  saveArea.HistoryStore (&serviceStartTime, sizeof (SCTime));
  saveArea.HistoryStore (&remainingTime, sizeof (SCDuration));
  saveArea.HistoryStore (&remainingService, sizeof (SCDuration));

  return true;
}


SCBoolean SCRequest::Restore (SCMem &saveArea)
{
  SCNatural key;

  saveArea.HistoryRestore (&requestID, sizeof(SCNatural));
  
  // nur Zeiger restaurieren, da statisches Objekt!
  saveArea.HistoryRestore (&serviceType, sizeof(SCRequestType *));
  saveArea.Restore(&key, sizeof(SCNatural));  // dummy key

  saveArea.Restore (&priority, sizeof(SCNatural));

  saveArea.Restore (&callerID, sizeof(SCProcessID));
  caller = (SCProcess *)SCScheduler::GetRunnableFromID (callerID);
  // don't assert caller here!

  saveArea.HistoryRestore (&creationTime, sizeof (SCTime));
  saveArea.HistoryRestore (&waitStartTime, sizeof (SCTime));
  saveArea.HistoryRestore (&serviceStartTime, sizeof (SCTime));
  saveArea.HistoryRestore (&remainingTime, sizeof (SCDuration));
  saveArea.HistoryRestore (&remainingService, sizeof (SCDuration));

  return true;
}


SCBoolean SCRequest::StaticSave(SCMem &saveArea)
{
  saveArea.HistoryStore(&nextRequestID, sizeof(SCNatural));

  return true;
}


SCBoolean SCRequest::StaticRestore(SCMem &saveArea)
{
  saveArea.HistoryRestore(&nextRequestID, sizeof(SCNatural));

  return true;
}


void SCRequest::StaticSize(SCSize *curSize)
{
  curSize->historySize += sizeof(SCNatural);     // nextRequestID
}


SCStream& operator<< (SCStream& pStream, const SCRequest& pData)
{
  assert(pData.GetRequestType());

  pStream << "request <" << *pData.GetRequestType() << ">(#";
  pStream << pData.GetRequestID() << ")";

  return pStream;
}


SCStream& SCRequest::Display(SCStream& pStream) const
{
  return (pStream << *this);
}

