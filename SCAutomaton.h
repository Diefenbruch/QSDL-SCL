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
|  SCAutomaton  |  SCAutomaton.h    | 5. Jul 1994   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     05.07.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCAUTOMATON__

#define __SCAUTOMATON__

#include "SCBasicTypes.h"
#include "SCEnabledTransition.h"  // needed for template instantiattion!
#include "SCEnabledTransitionList.h"
#include "SCTransition.h"  // needed for template instantiattion!
#include "SCTransitionList.h"
#include "SCRunnable.h"
#include "SCAutomatonTable.h"

#include "SCTraceSensor.h"

class SCAutomaton : public SCRunnable
{
  public:
      /* Destruktor */
    virtual                  ~SCAutomaton (void);

    SCEnabledTransitionList * GetEnabledTransitions (void) const;
    void                      SetEnabledTransitions (SCEnabledTransitionList * pEnableTransitionList);
    SCEnabledTransitionList * RetrieveEnabledTransitions (void);
    SCBoolean                 IsInState(void) const;
    static SCAutomatonTable * GetAutomatonTable(void) { return automatonTable; }
    static void               SetAutomatonTable(SCAutomatonTable *newTable);
    class SCDataType *        GetParameters (void) const;
    SCTime                    GetNextWakeupTime (void) const { return nextWakeupTime; }
    SCTime                    GetCreationTime (void) const { return creationTime; }
    SCTime                    GetStateTime (void) const { return stateTime; }
    class SCStateType  *      GetLastState(void) const;
    SCBoolean                 HasVariableSize(void) const { return variableSize; }
    
    virtual void              SensorUpdate (const SCInteger pAction,
                                            const SCTraceSensorRole pRole,
                                            const struct SCTraceSensorUpdate &pInfo)
                                 { (void)pAction; (void)pRole; (void)pInfo; };

    virtual class SCProcess * GetOwner (void) const;
    virtual void              Stop (SCBoolean traceIt = true) = 0;

    static SCBoolean          Initialize (SCNatural pTableSize);
    static SCBoolean          Finish (void);

    SCBoolean                 operator== (const SCAutomaton & second) const;
    SCBoolean                 operator!= (const SCAutomaton & second) const;

  protected:
      /* Konstruktor */
                              SCAutomaton (class SCStateType *startState,
                                           class SCDataType *processParameters,
                                           const SCObjectType pObjectType,
                                           const SCBoolean varSize = false, // for SCString and SCCharstring
                                           const SCObject* pParent = NULL);
                              SCAutomaton (const SCObjectType pObjectType,
                                           const SCBoolean varSize = false,
                                           const SCObject *pParent = NULL);

    void                      SetEnablingConditions (class SCStateType *theState,
                                                     SCNatural numEC,
                                                     ...);    // #args := numEC
    void                      ClearEnablingConditions (void);
    SCTransitionID            State (class SCStateType * curState,
                                     const SCDuration awakeDelay,
                                     class SCSignalType **signalRead);

    SCTransitionID            Transition (class SCSignalType **signalRead);

    class SCDataType *        GetInputData (void) const;
    class SCDataType *        RetrieveInputData (void);

                            // Aktionen
    void                      Output (const SCProcessID destination,
                                      const class SCSignalType *const signalType,
                                      SCDataType *data = NULL,
                                      const SCDuration delay = 0.0);
    void                      Output (class SCProcessType * destination,
                                      const class SCSignalType *const signalType,
                                      SCDataType *data = NULL,
                                      const SCDuration delay = 0.0);
    void                      Output (class SCProcess * destination,
                                      const class SCSignalType *const signalType,
                                      SCDataType *data = NULL);
    void                      Request (class SCMachine *machine,
                                       const class SCRequestType *const requestType,
                                       const SCDuration serviceAmount,
                                       const SCNatural priority = 0);
    void                      Create (class SCProcessType *type,
                                      class SCDataType *actualParams = NULL);
    void                      Call (class SCProcedure *newProcedure);
    void                      SetTimer (const SCTime when,
                                        const class SCTimerType *const timerType,
                                        SCDataType *data = NULL);
    void                      ResetTimer (const class SCTimerType *const timerType,
                                          SCDataType *data = NULL,
                                          SCBoolean rescheduleTimerControl = true);
    SCNatural                 Decision (const SCNatural numOfArgs, ...) const;
    void                      Assert (const SCBoolean theAssertion) const;

    SCTime                    Now (void) const;

    void                      SetLastState (class SCStateType *state) { lastState = state; }
    SCEnabledTransitionList * EnabledTransitions (const SCBoolean useInputTail = false) const;

    friend SCStream &          operator<< (SCStream& pStream,
                                          const SCAutomaton& pData);
    virtual SCStream &         Display (SCStream& pStream) const;

    SCBoolean                 Save (SCMem &) const;
    SCBoolean                 Restore (SCMem &);
   
    void                      Size (SCSize *pSize) const;

  private:
    SCNatural                 SignalEnabledTransitions (SCEnabledTransitionList *selection,
                                                        const SCBoolean priorityInputs,
                                                        const SCBoolean useInputTail = false) const;
    SCNatural                 SpontaneousEnabledTransitions (SCTransitionList *transitions,
                                                             SCEnabledTransitionList *selection) const;
    void                      RemoveInputTimer (const class SCTimerType *const timerType,
                                                SCDataType *data);


      /* Alle Datenmember sind privat */
    class SCDataType *        parameters;             // Saved by generated code!
    class SCStateType *       lastState;
    SCBoolean                 recalculateAwakeDelay;  // Flag indicates if state
                                                      // method must recalculate
                                                      // awake delay. This is true
                                                      // if state changed or if
                                                      // delay has run down.
    class SCDataType *        lastInputData;          // temporarely used, don't save!
    SCTime                    nextWakeupTime;         // time of next activity
    SCTime                    creationTime;           // time of the creation
    SCNatural                 maxEC;                  // number of elements in ...
    SCBoolean *               enablingConditions;     // array for enabling conditions.
    SCTime                    stateTime;              // time of entering the current state
    SCBoolean                 isInState;              // indicates if a process
                                                      // is executing Request-method
                                                      // needed for SCIndetVal
    SCEnabledTransitionList * enabledTransitionList;  // currently enabled transitions
                                                      // don't save, they are stored
                                                      // by validation algorithm!
    SCBoolean                 variableSize;           // this flag is true if
                                                      // the automaton has
						      // SCString-transformations
						      // or SCCharstring variables;
						      // in this case we can't use
						      // the currentSize values of
						      // SCRunnable
    static SCAutomatonTable * automatonTable;         // list of all existing automatons
};


#if !_SC_NOINLINES
  #include "SCAutomaton.inl.h"
#endif


#endif  // __SCAutomaton__
