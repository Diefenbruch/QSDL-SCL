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
|  SCScheduler  | SCScheduler.inl.h | 27. Jul 1995  |   SCL             |       |
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


#ifndef __SCSCHEDULER_INL__

#define __SCSCHEDULER_INL__

#include <float.h>


_SCINLINE_ SCRunnableID SCScheduler::NewProcessID (void)
{
  return nextRunnableID;
}


_SCINLINE_ SCTime SCScheduler::GetCurrentTime (void)
{
  return schedulerTime;
}


_SCINLINE_ SCDuration SCScheduler::GetEpsilon (void)
{
  return schedulerTime * DBL_EPSILON;
}


_SCINLINE_ class SCIndet *SCScheduler::GetIndet (void)
{
  return (indet);
}


_SCINLINE_ void SCScheduler::SetIndet (class SCIndet *newIndet)
{
  indet = newIndet;
}


_SCINLINE_ class SCSocket *SCScheduler::GetSocket (void)
{
  return (socket);
}


_SCINLINE_ void SCScheduler::SetSocket (class SCSocket *newSocket)
{
  socket = newSocket;
}


_SCINLINE_ void SCScheduler::SetScheduler(SCScheduler* newScheduler)
{
  scheduler = newScheduler;
}


_SCINLINE_ SCScheduler* SCScheduler::GetScheduler(void)
{
  return scheduler;
}

#endif  // __SCSCHEDULER_INL__
