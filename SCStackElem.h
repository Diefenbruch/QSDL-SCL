/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Axel Hirche                                   |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|  SCStackElem  |  SCStackElem.h    | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCSTACKELEM__

#define __SCSTACKELEM__

#include "SCBasicTypes.h"
#include "SCActiveRunnableList.h"
#include "SCStackElemList.h"

class SCStackElem
{
  public:
    SCStackElem (class SCMem *state,
                 SCInteger hashKey,
                 SCActiveRunnableList *runList,
                 SCBoolean lastActiveWasAutomaton,
                 class SCRunnable * lastActive,
                 SCProcessID lastActiveID,                // ID of process of last transition
                 class SCEnabledTransition * pTransition, // Last executed transition.
                 SCNatural pSuccessor,
                 SCBitSet pAcceptanceBitField = SC_BS0,
                 SCNatural pFlipCounter = 0,
                 SCNatural numOfCreatedInstances = 0,
                 SCProcessID *createdInstances = NULL);
   ~SCStackElem (void);
   
    class SCMem *               GetState (void) const { return systemState; }
    SCActiveRunnableList *      GetActiveRunnableList (void) const { return activeRunnableList;};
    SCBoolean                   IsLastActiveRunnableAutomaton (void) const { return lastActiveRunnableWasAutomaton; }
    SCProcessID                 GetLastActiveRunnableID (void) const { return lastActiveRunnableID; }
    class SCRunnable *          GetLastActiveRunnable (void) const;
    class SCEnabledTransition * GetLastTransition (void) const { return lastTransition; }
    SCNatural                   GetLastSuccessor(void) const { return lastSuccessor;}
    SCNatural                   NumOfCreatedInstances(void) const { return numOfCreatedInstances;}
    SCProcessID *               GetCreatedInstances(void) const { return createdInstances; }
    SCInteger                   GetHashKey(void) const { return primaryHashKey; }
    SCStackElem *               GetNextElem (void) const { return next; }
    SCStackElem *               GetPrevElem (void) const { return prev; }
    SCBitSet                    GetFulfilledAcceptanceSets (void) const { return fulfilledAcceptanceSets; }
    SCNatural                   GetFlipCounter (void) const { return flipCounter; }

    static void                 SetDeleteState (SCBoolean value) { deleteState = value; }

    friend SCStream&             operator<< (SCStream &pStream,
                                            const SCStackElem &pData);
  private:
    class SCMem *               systemState;
    SCNatural                   primaryHashKey;           // m value of SCHash
    SCActiveRunnableList *      activeRunnableList;

    // Information for creation of error traces:
    SCBoolean                   lastActiveRunnableWasAutomaton;
                                                          // To distinguish between
                                                          // automatons and
                                                          // machines or timer
    class SCRunnable *          lastActiveRunnable;       // Timer and machines don't change.
    SCProcessID                 lastActiveRunnableID;     // Only IDs of processes are stored
    class SCEnabledTransition * lastTransition;           // last executed
                                                          // transition
    SCNatural                   lastSuccessor;            // Buechi-Successor
    SCBitSet                    fulfilledAcceptanceSets;  // bit-field which
                                                          // has a bit for each
                                                          // acceptance sets of
                                                          // the buechi automaton
                                                          // if a bit is 1, a state
                                                          // of the regarding set has
                                                          // been visited earlier
                                                          // on the stack
    SCNatural                   flipCounter;              // if all relvant bits
                                                          // of fulfilledAcceptanceSets
                                                          // are set to 1, they are
                                                          // reseted to 0 and flipCounter
                                                          // is incremented
    SCNatural                   numOfCreatedInstances;
    SCProcessID *               createdInstances;

#if _SC_STACKTRACE_DEBUG
    SCNatural                   depth;
#endif

    SCStackElem *               prev;
    SCStackElem *               next;
    SCStackElemCons *           container;              // actual SCListCons elem
                                                        // in which process is
                                                        // embedded (only for cycle detection)
    static SCBoolean            deleteState;            // delete systemState
                                                        // member in destructor
                                                        // true for bit state
                                                        // false for full state
  friend class SCStack;

  friend class SCIndetStackTrace;
};


#endif  // __SCSTACKELEM__
