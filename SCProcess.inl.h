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
|   SCProcess   |  SCProcess.inl.h  | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCPROCESS_INL__

#define __SCPROCESS_INL__


_SCINLINE_ class SCProcessType *SCProcess::GetType (void) const
{
  return (procType);
}


_SCINLINE_ SCProcessID SCProcess::Sender (void) const
{
  return (senderID);
}


_SCINLINE_ SCProcessID SCProcess::Parent (void) const
{
  return (parentID);
}


_SCINLINE_ SCProcessID SCProcess::Offspring (void) const
{
  return (offspringID);
}


_SCINLINE_ SCProcessID SCProcess::Self (void) const
{
  return (GetID()); // selfID);
}


_SCINLINE_ void SCProcess::SetSender (const SCProcessID pSender)
{
  senderID = pSender;
}


_SCINLINE_ void SCProcess::SetOffspring (const SCProcessID pOffspring)
{
  offspringID = pOffspring;
}


_SCINLINE_ SCBoolean SCProcess::GetCallScheduler (void) const
{
  return (callScheduler);
}


_SCINLINE_ void SCProcess::SetCallScheduler (SCBoolean pCallScheduler)
{
  callScheduler = pCallScheduler;
}


#endif  // __SCPROCESS_INL__
