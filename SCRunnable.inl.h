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
|   SCRunnable  | SCRunnable.inl.h  | 27. Jul 1995  |   SCL             |       |
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


#ifndef __SCRUNNABLE_INL__

#define __SCRUNNABLE_INL__

#include "SCScheduler.h"

#include "SCTrace.h"


_SCINLINE_ SCTime SCRunnable::GetWakeupTime (void) const
{
  return (wakeupTime);
}


_SCINLINE_ SCBoolean SCRunnable::IsBlocked (void) const
{
  return (blocked);
}


_SCINLINE_ SCBoolean SCRunnable::IsWaiting (void) const
{
  return (waiting);
}


_SCINLINE_ SCBoolean SCRunnable::IsProcess (void) const
{
  return (this->GetType() == SC_PROCESS);
}


_SCINLINE_ SCBoolean SCRunnable::IsProcedure (void) const
{
  return (GetType() == SC_PROCEDURE);
}


_SCINLINE_ SCBoolean SCRunnable::IsMachine (void) const
{
  return (GetType() == SC_MACHINE);
}


_SCINLINE_ SCBoolean SCRunnable::IsAutomaton (void) const
{
  return (GetType() == SC_PROCESS ||
          GetType() == SC_PROCEDURE);
}


_SCINLINE_ SCRunnableID SCRunnable::GetID (void) const
{
  return (runnableID);
}


_SCINLINE_ SCBoolean SCRunnable::IsVerbose (void) const
{
  return SCScheduler::IsVerbose();
}


_SCINLINE_ void SCRunnable::TraceOn (void)
{
  traceOn = true;
}


_SCINLINE_ void SCRunnable::TraceOff (void)
{
  traceOn = false;
}


_SCINLINE_ SCBoolean SCRunnable::IsTraceOn (void) const
{
  return (traceOn);
}


#endif  // __SCRUNNABLE_INL__
