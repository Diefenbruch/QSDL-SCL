/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+-------------------+-------------------+---------------+-------------------+---+
|       Module      |   File            |   Created     |   Project         |   |
+-------------------+-------------------+---------------+-------------------+---+
| SCActiveRunnable  |SCActiveRunnable.h | 04. Mai 1996  |   SCL             |   |
+-------------------+-------------------+---------------+-------------------+---+
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


#ifndef __SCACTIVERUNNABLE__

#define __SCACTIVERUNNABLE__


#include "SCBasicTypes.h"
#include "SCEnabledTransition.h"  // needed for template instantiation!
#include "SCEnabledTransitionList.h"
#include "SCTimeEvent.h"

class SCActiveRunnable: public SCTimeEvent
{
  public:
                               SCActiveRunnable (class SCTimeEvent *event,
                                                 SCEnabledTransitionList *etlist = NULL,
                                                 const SCNatural activities = 1);
                              ~SCActiveRunnable (void);

    SCEnabledTransitionList *  GetEnabledTransitionList(void) const;

    SCNatural                  GetCurrentSuccessor(void) const { return currentSuccessor; }
    SCNatural                  GetNextSuccessor(void) { assert(currentSuccessor < numOfSuccessors);
                                                        return ++currentSuccessor; }
    SCBoolean                  HasNextSuccessor(void) const { if (currentSuccessor < numOfSuccessors)
                                                                return true;
                                                              return false; } 
    void                       ResetSuccessors (void) { currentSuccessor = 0; }

  private:
    SCEnabledTransitionList  * enabledTransitionList;
    SCNatural                  numOfSuccessors;       // important for verification.
                                                      // tells SCIndetVal how often
                                                      // this runnable can be choosen
                                                      // before it is exhausted. This
                                                      // depends on the number of successors
                                                      // of the Buechi automaton. This
                                                      // number is calculated by the
                                                      // virtual NumOfSuccessors() method.
    SCNatural                  currentSuccessor;
};

#endif  //  __SCACTIVERUNNABLE__
