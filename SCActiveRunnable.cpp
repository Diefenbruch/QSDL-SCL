/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+-------------------+---------------+------------------+---+
|       Module      |   File            |   Created     |   Project        |   |
+-------------------+-------------------+---------------+------------------+---+
| SCActiveRunnable  |SCActiveRunnable.cc| 16. Aug 1994  |   SCL            |   |
+-------------------+-------------------+---------------+------------------+---+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     16.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCActiveRunnable.h"
#include "SCEnabledTransitionList.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCActiveRunnable::SCActiveRunnable (SCTimeEvent *event,
                                    SCEnabledTransitionList *etlist,
                                    const SCNatural successors) :
  SCTimeEvent(*event),
  enabledTransitionList(etlist),
  numOfSuccessors(successors),
  currentSuccessor(0)
{
  assert(numOfSuccessors > 0);
}


SCActiveRunnable::~SCActiveRunnable (void)
{
  if (enabledTransitionList)
    delete enabledTransitionList;
}


SCEnabledTransitionList *SCActiveRunnable::GetEnabledTransitionList(void) const
{
  return enabledTransitionList;
}
