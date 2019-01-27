/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCStackElem  |  SCStackElem.cc   | 30. Apr 1996  |   SCL             |      |
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

#include "SCStackElem.h"
#include "SCMem.h"
#include "SCActiveRunnableList.h"
#include "SCAutomaton.h"
#include "SCEnabledTransition.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCBoolean SCStackElem::deleteState = true;

SCStackElem::SCStackElem (class SCMem *          state,
                          SCInteger              hashKey,
                          SCActiveRunnableList * runList,
                          SCBoolean              activeWasAutomaton,
                          SCRunnable *           activeRunnable,
                          SCProcessID            activeRunnableID,
                          SCEnabledTransition  * pTransition,
                          SCNatural              pSuccessor,  // Buechi-Successor
                          SCBitSet               pFulfilledAcceptanceSets,
                          SCNatural              pFlipCounter,
                          SCNatural              numOfInstances,
                          SCProcessID *          instances):
  systemState(state),
  primaryHashKey(hashKey),
  activeRunnableList(runList),
  lastActiveRunnableWasAutomaton(activeWasAutomaton),
  lastActiveRunnable(activeRunnable),
  lastActiveRunnableID(activeRunnableID),
  lastTransition(pTransition),
  lastSuccessor(pSuccessor),
  fulfilledAcceptanceSets(pFulfilledAcceptanceSets),
  flipCounter(pFlipCounter),  
  numOfCreatedInstances(numOfInstances),
  createdInstances(instances),
#if _SC_STACKTRACE_DEBUG
  depth(0),    // depth of this elem in its stack
#endif
  prev(NULL),
  next(NULL)
{
}


SCStackElem::~SCStackElem (void)
{
  if (deleteState)
    delete systemState;

  delete activeRunnableList;

  if (lastTransition)
    delete lastTransition;
}


SCRunnable *SCStackElem::GetLastActiveRunnable(void) const
{
  SCRunnable *runnable;
  
  // Bei Maschinen und Timer existiert Runnable
  // auf jeden Fall:
  if (!lastActiveRunnableWasAutomaton)
    return lastActiveRunnable;

  runnable = (SCRunnable *)SCScheduler::GetRunnableFromID(lastActiveRunnableID);
  assert(runnable);

  return runnable;
}


SCStream& operator<< (SCStream& pStream, const SCStackElem& pData)
{
  (void)pData;

  return pStream;
}
