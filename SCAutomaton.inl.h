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
| SCAutomaton   | SCAutomaton.inl.h | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCAUTOMATON_INL__

#define __SCAUTOMATON_INL__


#include "SCScheduler.h"

_SCINLINE_ void SCAutomaton::ClearEnablingConditions (void)
{
  if (enablingConditions)
  {
    delete[] enablingConditions;
    enablingConditions = NULL;

#if _SC_VALIDATION_OPTIMIZE
    currentHistorySize -= (sizeof(SCBoolean) * maxEC);
#endif
  }
  maxEC = 0;
}


_SCINLINE_ class SCDataType *SCAutomaton::GetInputData (void) const
{
  return (lastInputData);
}


_SCINLINE_ class SCDataType *SCAutomaton::RetrieveInputData (void)
{
  class SCDataType *temp;

  temp = lastInputData;
  lastInputData = NULL;
  return (temp);
}


_SCINLINE_ SCEnabledTransitionList *SCAutomaton::GetEnabledTransitions (void) const
{
  return enabledTransitionList;
}


_SCINLINE_ void SCAutomaton::SetEnabledTransitions (SCEnabledTransitionList *pEnabledTransitionList)
{
  enabledTransitionList = pEnabledTransitionList;
}


_SCINLINE_ SCEnabledTransitionList *SCAutomaton::RetrieveEnabledTransitions (void)
{
  SCEnabledTransitionList *temp;

  if (enabledTransitionList != NULL)
  {
    temp = enabledTransitionList;
    enabledTransitionList = NULL;
    return (temp);
  }
  else
  {
    return EnabledTransitions ();
  }
}


_SCINLINE_ class SCDataType *SCAutomaton::GetParameters (void) const
{
  return (parameters);
}


_SCINLINE_ class SCStateType *SCAutomaton::GetLastState (void) const
{
  return (lastState);
}


_SCINLINE_ SCTime SCAutomaton::Now (void) const
{
  return (SCScheduler::GetCurrentTime());
}


_SCINLINE_ SCBoolean SCAutomaton::IsInState (void) const
{
  if (!(isInState == true || isInState == false))
    assert(false);

  return (isInState);
}


_SCINLINE_ void SCAutomaton::SetAutomatonTable (SCAutomatonTable *table)
{
  automatonTable = table;
}

#endif  // __SCAutomaton_INL__
