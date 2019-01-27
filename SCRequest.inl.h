/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-----------------------+---------------+-------------------+---+
|   Module      |       File            |   Created     |   Project         |   |
+---------------+-----------------------+---------------+-------------------+---+
|   SCRequest   |    SCRequest.inl.h    | 02. Mar 1996  |   SCL             |   |
+---------------+-----------------------+---------------+-------------------+---+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     --.--.--    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCREQUEST_INL__

#define __SCREQUEST_INL__

#include "SCScheduler.h"


_SCINLINE_ SCNatural SCRequest::GetPriority (void) const
{
  return priority;
}


_SCINLINE_ const class SCRequestType *SCRequest::GetRequestType (void) const
{
  return serviceType;
}


_SCINLINE_ SCNatural SCRequest::GetRequestID (void) const
{
  return requestID;
}


_SCINLINE_ SCTime SCRequest::GetCreationTime (void) const
{
  return creationTime;
}


_SCINLINE_ SCTime SCRequest::GetWaitStartTime (void) const
{
  return waitStartTime;
}


_SCINLINE_ void SCRequest::SetWaitStartTime (SCTime newValue)
{
  waitStartTime = newValue;
}


_SCINLINE_ SCTime SCRequest::GetServiceStartTime (void) const
{
  return serviceStartTime;
}


_SCINLINE_ void SCRequest::SetServiceStartTime (SCTime newValue)
{
  serviceStartTime = newValue;
}


_SCINLINE_ SCDuration SCRequest::GetRemainingTime (void) const
{
  return remainingTime;
}


_SCINLINE_ SCDuration SCRequest::GetRemainingService (void) const
{
  return remainingService;
}


_SCINLINE_ SCRequestCons * SCRequest::GetContainer (void) const
{
  return container;
}


_SCINLINE_ void SCRequest::SetContainer (SCRequestCons *cont)
{
  container = cont;
}


_SCINLINE_ SCAutomaton * SCRequest::GetCaller (void) const
{
  SCRequest *mutable_this = (SCRequest *)this;

  if (!caller)
  {
    mutable_this->caller = (SCAutomaton *)SCScheduler::GetRunnableFromID(callerID);
  }

  return caller;
}


#endif  // __SCREQUEST_INL__
