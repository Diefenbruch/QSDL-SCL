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
|   SCSignal    |  SCSignal.inl.h   | 24. Feb 1996  |   SCL             |       |
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


#ifndef __SCSIGNAL_INL__

#define __SCSIGNAL_INL__


#include "SCProcessType.h"
#include "SCSignalType.h"
#include "SCScheduler.h"

_SCINLINE_ const SCSignalType *SCSignal::GetSignalType (void) const
{
  return (signalType);
}


_SCINLINE_ const SCProcessType *SCSignal::GetSenderType (void) const // type of sender
{
  return (senderType);
}


_SCINLINE_ SCNatural SCSignal::GetSignalID (void) const
{
  return (signalID);
}


_SCINLINE_ SCNatural SCSignal::GetTimerID (void) const
{
  return (timerID);
}


_SCINLINE_ class SCProcess* SCSignal::GetSender (void) const
{
  return ((SCProcess*) SCScheduler::GetRunnableFromID(senderID));
                               // return a pointer to the sender
}


_SCINLINE_ SCProcessID SCSignal::GetSenderID (void) const
{
  return (senderID);           // return the ID of the sender
}


_SCINLINE_ class SCDataType *SCSignal::GetData (void) const
{
  return (data);
}


_SCINLINE_ SCTime SCSignal::GetCreationTime (void) const
{
  return (creationTime);
}


_SCINLINE_ SCSignalCons * SCSignal::GetContainer (void) const
{
  return container;
}


_SCINLINE_ void SCSignal::SetContainer (SCSignalCons *cont)
{
  container = cont;
}


_SCINLINE_ SCSignalTable * SCSignal::GetSignalTable(void)
{
  return signalTable;
}


_SCINLINE_ void SCSignal::SetSignalTable(SCSignalTable *newTable)
{
  signalTable = newTable;
}


#endif  // __SCSIGNAL_INL__
