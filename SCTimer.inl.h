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
|    SCTimer    |   SCTimer.inl.h   | 24. Feb 1996  |   SCL             |       |
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


#ifndef __SCTIMER_INL__

#define __SCTIMER_INL__


_SCINLINE_ const SCTimerType *SCTimer::GetTimerType (void) const
{
  return (timerType);
}


_SCINLINE_ const SCNatural SCTimer::GetTimerID (void) const
{
  return (timerID);
}


_SCINLINE_ const SCTime SCTimer::GetTimeout (void) const
{
  return (when);
}


_SCINLINE_ class SCDataType * SCTimer::GetData (void) const
{
  return (data);
}


_SCINLINE_ SCProcessID SCTimer::GetProcessID (void) const
{
  return processID;
}


_SCINLINE_ SCTimerCons * SCTimer::GetContainer (void) const
{
  return container;
}


_SCINLINE_ void SCTimer::SetContainer (SCTimerCons *cont)
{
  container = cont;
}


_SCINLINE_ SCTimerTable * SCTimer::GetTimerTable(void)
{
  return timerTable;
}


_SCINLINE_ void SCTimer::SetTimerTable(SCTimerTable *newTable)
{
  timerTable = newTable;
}


_SCINLINE_ SCTimerCons * SCTimer::GetTableContainer (void) const
{
  return tableContainer;
}


_SCINLINE_ void SCTimer::SetTableContainer (SCTimerCons *cont)
{
  tableContainer = cont;
}


_SCINLINE_ SCTimeEventCons * SCTimer::GetTimeEventContainer (void) const
{
  return timeEventContainer;
}


_SCINLINE_ void SCTimer::SetTimeEventContainer (SCTimeEventCons *cont)
{
  timeEventContainer = cont;
}

#endif  // __SCTIMER_INL__
