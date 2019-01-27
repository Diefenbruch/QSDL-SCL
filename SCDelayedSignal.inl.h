/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|   SCDelayedSignal   |  SCDelayedSignal.inl.h  | 24. Feb 1996  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
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


#ifndef __SCDELAYEDSIGNAL_INL__

#define __SCDELAYEDSIGNAL_INL__


#include "SCProcessType.h"
#include "SCSignalType.h"
#include "SCScheduler.h"


_SCINLINE_ class SCProcess* SCDelayedSignal::GetReceiver (void) const
{
  return ((SCProcess*) SCScheduler::GetRunnableFromID(receiverID));
                                  // return a pointer to the receiver
}


_SCINLINE_ SCProcessID SCDelayedSignal::GetReceiverID (void) const
{
  return (receiverID);            // return the ID of the receiver
}


_SCINLINE_ void SCDelayedSignal::SetReceiverID (const SCProcessID pReceiverID)
{
  receiverID = pReceiverID;       // set the ID of the receiver
}


_SCINLINE_ const SCProcessType *SCDelayedSignal::GetReceiverType (void) const
{
  return (receiverType);          // return the type of receiver
}


_SCINLINE_ class SCSignal * SCDelayedSignal::RetrieveSignal (void)
{
  class SCSignal * tmp;
  
  tmp = signal;
  signal = NULL;
  
  return tmp;
}

#endif  // __SCDELAYEDSIGNAL_INL__
