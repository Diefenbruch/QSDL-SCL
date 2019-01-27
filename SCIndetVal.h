/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCIndetVal   |  SCIndetVal.h     | 25. Apr 1996  |   SCL             |      |
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


#ifndef __SCINDETVAL__

#define __SCINDETVAL__

#include "SCBasicTypes.h"
#include "SCIndet.h"
#include "SCActiveRunnableList.h"

class SCIndetVal : public SCIndet
{
  public:
                         SCIndetVal (SCTime    stopTime,
                                     SCBoolean errors,
                                     SCNatural maxNumOfErrors,
                                     SCBoolean deadCode,
                                     SCBoolean doCycleDetection,
                                     class SCStateSpace *stateSpace,
                                     class SCStateSpace *cycleStateSpace,
                                     SCNatural maxValDepth = kSCMaxDepth,
                                     SCNatural maxNumOfCreatedInstances = kSCMaxCreatedInstances,
                                     SCName2PropFuncPtr * qAtom = NULL,
                                     SCIndetType = scIndetValidation);
    virtual             ~SCIndetVal (void);

    SCNatural            NonDetChoice (const SCNatural range);
    SCNatural            ChooseOneTrue (const SCNatural choiceSize,
                                        const SCNatural numOfTrues,
                                        const SCNatural singleTrue,
                                        const SCBoolean *choiceSet);

    class SCRunnable  *  ChooseRunnable (SCTimeEventSaveList *activeQueue);
    class SCEnabledTransition * ChooseTransition (void);

    void                 StartMessage (void);
    void                 EndMessage (void);

    void                 SwitchToNestedDFS (void);
    void                 SwitchToOrigDFS (SCBoolean cycleFound);

    // The following three methods classify the reached system
    // state:
    
    SCErrorCode          IsErrorState (SCTimeEventSaveList *activeQueue);
    SCBoolean            IsLimitingState (SCTimeEventSaveList *activeQueue);
    virtual SCBoolean    IsCycleStartState (void) const;
    virtual SCBoolean    IsCycleEndState (SCMem *state);

		SCBoolean            Create (SCProcessID offspring);

    class SCStack *      GetStack (void) const;
    class SCStack *      GetFirstStack (void) const;
    class SCStack *      GetSecondStack (void) const;
    class SCStateSpace * GetCurrentStateSpace (void) const;
    class SCStateSpace * GetStateSpace (void) const;
    class SCStateSpace * GetCycleStateSpace (void) const;
    SCErrorCode          GetLastError (void) const;
    SCNatural            GetLastSuccessor (void) const;
    class SCStackElem *  GetCycleClosingElem (void) const;

    // Virtual method for reporting current exploration depth
    // default implementation in SCIndet return number of executed
    // transition, but SCIndetVal must redefine it to report
    // current stack length (-1):

    SCNatural           CurrentDepth (void) const;
    
    SCBoolean           LogError (SCErrorCode errorCode);     // deadlock
    SCBoolean           LogError (SCErrorCode errorCode,      // signal drop or
                                  const class SCProcess *process, // signal rejected
                                  const class SCSignal *signal);
    SCBoolean           LogError (SCErrorCode errorCode,      // no receiver
                                  const class SCProcess *process,
                                  const class SCSignalType *signal);
    SCBoolean           LogError (SCErrorCode errorCode,      // process creation
                                  const class SCProcess *parent,    // failed
                                  const class SCProcessType *offspringType);

    friend SCStream &   operator<< (SCStream& pStream,
                                    const SCIndetVal& pData);
    virtual SCStream &  Display (SCStream &pStream) const;

  protected:

    virtual void         SystemStateSize (SCSize *pSize) const;
    virtual SCBoolean    StoreSystemState(class SCMem *state) const;
    virtual SCBoolean    RestoreSystemState(class SCMem *state) const;

    class SCStack *      currentStack;
    class SCStack *      theFirstStack;
    class SCStack *      theSecondStack;
    class SCStateSpace * currentStateSpace;
    class SCStateSpace * theStateSpace;
    class SCStateSpace * theCycleStateSpace;

    SCBitSet             fulfilledAcceptanceSets; // This bit-field has one bit for every
                                                  // set of acceptance states.
                                                  // If all bits are set, it means
                                                  // that at least one state of
                                                  // every acceptance has been
                                                  // explored. In this
                                                  // case, all bits are reset to
                                                  // zero. This way, the set
                                                  // "counts modulo". At a given
                                                  // state one can store the
                                                  // current value of
                                                  // fulfilledAcceptanceSets, before it
                                                  // is updated. If a state with the
                                                  // same value for leftObligations
                                                  // is reached, this
                                                  // means that in the meantime, at
                                                  // least one state out of every
                                                  // acceptance state set has been
                                                  // reached. Useful for cycle
                                                  // detection.
    SCNatural flipCounter;                        // counts how often the bits
                                                  // of fulfilledAcceptanceSets are
                                                  // reset to zero

    SCErrorCode          lastError;               // last occured error
  
  private:

    /* Alle weiteren Datenmember sind privat */

    SCProcessID *        createdInstances;         // IDs of created processes
    SCNatural            numOfCreatedInstances;    // num of already created Inst.
    SCNatural            maxNumOfCreatedInstances; // max num of created Inst.
    SCNatural            truncatedPathes;          // num of truncated pathes
    SCBoolean            cycleDetection;           // detect cycles in reachability
                                                   // graph?
    SCBoolean            isInCycleDetection;       // currently doing nested DFS for
                                                   // cycle detection?
    SCBoolean            lastRunnableIsAutomaton;  // active runnable is automaton
    SCProcessID          lastRunnableID;           // stores ID of the runnable
                                                   // which executed the transition
                                                   // leading to the current state
                                                   // (for process)
    class SCRunnable *   lastRunnable;             // stores pointer to the runnable
                                                   // which executed the transition
                                                   // leading to the current state
                                                   // (for timer or machine)
    class SCEnabledTransition * lastTransition;    // stores pointer to the transition
                                                   // leading to the current state
                                                   // together with signal ID
    SCNatural            lastSuccessor;            // Buechi-Successor
    class SCIndetStackTrace * theIndetStack;       // special indet object for
                                                   // trace generation
    SCNatural            minStateSize;
    SCNatural            maxStateSize;
    
    class SCStackElem *  cycleClosingElem;

    SCNatural            numOfCycleStartStates;    // number of nested DFS

  friend class SCIndetStackTrace;

};


#if !_SC_NOINLINES
  #include "SCIndetVal.inl.h"
#endif


#endif  // __SCINDETVAL__
