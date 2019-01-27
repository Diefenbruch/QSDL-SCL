/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch                                               |
|       Universitaet Essen                                                     |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCIndetVal    | SCIndetVal.cc     | 25. Apr 1996  |   SCL             |      |
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

#include <fstream>

#include "SCStream.h"
#include "SCIndetVal.h"
#include "SCActiveRunnable.h"
#include "SCActiveRunnableList.h"
#include "SCStateSpace.h"
#include "SCSignal.h"
#include "SCSensor.h"
#include "SCProcess.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCMachine.h"
#include "SCRandom.h"
#include "SCRequest.h"
#include "SCRunnable.h"
#include "SCScheduler.h"
#include "SCEnabledTransition.h"
#include "SCStack.h"
#include "SCStackElem.h"
#include "SCTimer.h"
#include "SCTransition.h"
#include "SCSystem.h"
#include "SCTimerControl.h"
#include "SCPath.h"
#include "SCMem.h"
#include "SCTraceEventMSC.h"

#if _SC_VALIDATION_DEBUG || _SC_DEBUGOUTPUT
  #include "SCDebug.h"
#endif

#include "SCIndetStackTrace.h"
#include "SCTraceEventMSC.h"
#include "SCTraceControl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCIndetVal.inl.h"
#endif

SCIndetVal::SCIndetVal (SCTime               maxTime,
                        SCBoolean            errors,
                        SCNatural            numErrors,
                        SCBoolean            deadCode,
                        SCBoolean            doCycleDetection,
                        SCStateSpace *       stateSpace,
                        SCStateSpace *       cycleStateSpace,
                        SCNatural            maxValDepth,
                        SCNatural            maxCreatedInstances,
                        SCName2PropFuncPtr * bAtom,
                        SCIndetType          pIndetType):
  SCIndet(pIndetType, maxTime, true, errors, numErrors, deadCode, bAtom),
  theStateSpace(stateSpace),
  theCycleStateSpace(cycleStateSpace),
  fulfilledAcceptanceSets(SC_BS0),
  flipCounter(0),
  lastError(scErrorNoError),
  createdInstances(NULL),
  numOfCreatedInstances(0),
  maxNumOfCreatedInstances(maxCreatedInstances),
  truncatedPathes(0),
  cycleDetection(doCycleDetection),
  isInCycleDetection(false),
  lastRunnableIsAutomaton(false),
  lastRunnableID(kSCNoProcessID),
  lastRunnable(NULL),
  lastTransition(NULL),
  lastSuccessor(0),
  theIndetStack(NULL),
  minStateSize(ULONG_MAX),
  maxStateSize(0),
  cycleClosingElem(NULL),  // needed for SCIndetStackTrace
  numOfCycleStartStates(0) // number of nested DFS
{
  theFirstStack = new SCStack(maxValDepth, cycleDetection);
  assert(theFirstStack);

  if (cycleDetection)
  {
    theSecondStack = new SCStack(maxValDepth, false);
    assert(theSecondStack);
  }
  else
  {
    theSecondStack = NULL;
  }
  
  currentStack = theFirstStack;
  currentStateSpace = theStateSpace;
}


SCIndetVal::~SCIndetVal (void)
{
  if (theFirstStack)
    delete theFirstStack;
  if (theStateSpace)
    delete theStateSpace;

  if (theSecondStack)
    delete theSecondStack;
  if (theCycleStateSpace)
    delete theCycleStateSpace;
  
  if (theIndetStack)
    delete theIndetStack;

  if (lastTransition)
    delete lastTransition;
}


void SCIndetVal::SystemStateSize(SCSize *size) const
{
  SCIndetVal* mutable_this = (SCIndetVal*) this;
  
  assert(size);
  
  SCProcessType::StaticSize(size);
  SCProcedureType::StaticSize(size);
  SCMachine::StaticSize(size);
  if (SCPath::GetPath())
  {
    SCPath::GetPath()->Size(size);
  }
  if (SCTimerControl::GetTimerControl())
  {
    SCTimerControl::GetTimerControl()->Size(size);
  }
  SCScheduler::StaticSize(size);
  SCSignal::StaticSize(size);
  SCRequest::StaticSize(size);
  SCTimer::StaticSize(size);
  SCRandom::StaticSize(size);
  SCSensor::StaticSize(size);
  assert(SCSystem::GetSystem());
  SCSystem::GetSystem()->SystemSize(size);  // virtual method, jumps in 
                                            // generated code (for sensors)

  if (size->size > maxStateSize)
    mutable_this->maxStateSize = size->size;
  else if (size->size < minStateSize)
    mutable_this->minStateSize = size->size;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCIndetVal::SystemStateSize(): System State Size:";
  scValidationDebugLog << size->size << std::endl;
  scValidationDebugLog << "SCIndetVal::SystemStateSize(): History State Size:";
  scValidationDebugLog << size->historySize << std::endl;
#endif
}


SCBoolean SCIndetVal::StoreSystemState(SCMem *state) const
{
  assert(state);
  
  // alle relevanten Statischen Elemente auf dem History-State sichern:
  SCProcessType::StaticSave(*state);
  SCProcedureType::StaticSave(*state);
  SCMachine::StaticSave(*state);
  if (SCPath::GetPath())
  {
    SCPath::GetPath()->Save(*state);
  }
  if (SCTimerControl::GetTimerControl())
  {
    SCTimerControl::GetTimerControl()->Save(*state);
  }
  SCScheduler::StaticSave(*state);
  SCSignal::StaticSave(*state);
  SCRequest::StaticSave(*state);
  SCTimer::StaticSave(*state);
  SCRandom::StaticSave(*state);
  SCSensor::StaticSave(*state);
  assert(SCSystem::GetSystem());
  SCSystem::GetSystem()->SystemSave(*state); // virtual method, jumps in 
                                             // generated code (for sensors)

  
#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCIndetVal::StoreSystemState(): System State Size used:";
  scValidationDebugLog << state->GetOffset() << std::endl;
  scValidationDebugLog << "SCIndetVal::StoreSystemState(): History State Size used:";
  scValidationDebugLog << state->GetHistoryOffset() << std::endl;
#endif

//  assert(state->GetSize() == state->GetOffset());
//  assert(state->GetHistorySize() == state->GetHistoryOffset());

  return true;
}


SCBoolean SCIndetVal::RestoreSystemState(SCMem *state) const
{
  state->SetOffset(0);
  state->SetHistoryOffset(0);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCIndetVal::RestoreSystemState(): System State Size:";
  scValidationDebugLog << state->GetSize() << std::endl;
  scValidationDebugLog << "SCIndetVal::RestoreSystemState(): History State Size:";
  scValidationDebugLog << state->GetHistorySize() << std::endl;
#endif

  // alle relevanten Statischen Elemente aus dem History-State holen:
  SCProcessType::StaticRestore(*state);
  SCProcedureType::StaticRestore(*state);
  SCMachine::StaticRestore(*state);
  if (SCPath::GetPath())
  {
    SCPath::GetPath()->Restore(*state);
  }
  if (SCTimerControl::GetTimerControl())
  {
    SCTimerControl::GetTimerControl()->Restore(*state);
  }
  SCScheduler::StaticRestore(*state);
  SCSignal::StaticRestore(*state);
  SCRequest::StaticRestore(*state);
  SCTimer::StaticRestore(*state);
  SCRandom::StaticRestore(*state);
  SCSensor::StaticRestore(*state);
  assert(SCSystem::GetSystem());
  SCSystem::GetSystem()->SystemRestore(*state); // virtual method, jumps in 
                                                // generated code (for sensors)

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCIndetVal::RestoreSystemState(): System State Size used:";
  scValidationDebugLog << state->GetOffset() << std::endl;
  scValidationDebugLog << "SCIndetVal::RestoreSystemState(): History State Size used:";
  scValidationDebugLog << state->GetHistoryOffset() << std::endl;
#endif

  return true;
}

                                         
SCRunnable *SCIndetVal::ChooseRunnable (SCTimeEventSaveList *activeQueue)
{
  SCSize                 size;
  SCMem *                state = NULL;
  SCRunnable *           runnable;
  SCStackElem *          stackElem;
  SCActiveRunnable *     activeRunnable;
  SCActiveRunnableList * activeRunnableList;
  SCAutomaton *          automaton;
  SCNatural              i;
  SCTimeEvent *          timeEvent;
  SCBoolean              isAnalyzed;
  
  ///////////////////////////////////////////////////////////////
  // 1. Check if this method is called from                    //
  //    SCIndetStackTrace::ChooseRunnable() after stack trace  //
  //    generation for a detected cycle is complete (in this   //
  //    case we must bring the control flow to the point where //
  //    the stack trace has started):                          //
  ///////////////////////////////////////////////////////////////

  if (lastError == scErrorCycleDetected)
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "switched back to indet val" << std::endl;
#endif

    lastError = scErrorNoError;

    if (lastTransition)
    {
      delete lastTransition;
      lastTransition = NULL;
    }

    goto cycle_done;
  }

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "processing new state" << std::endl;
#endif

  ///////////////////////////////////////////////////////
  // 2. Check if state is faulty:                      //
  ///////////////////////////////////////////////////////

  if (IsErrorState(activeQueue))
  {
    if (LogError(lastError))
    {
#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
      scValidationDebugLog << "switching to indet stack trace" << std::endl << std::endl;
#endif
      assert(theIndetStack);
      return theIndetStack->ChooseRunnable (activeQueue);
    }

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "switched back to indet val" << std::endl;
#endif

    delete lastTransition;
    lastTransition = NULL;

    goto backtracking;
  }

  ///////////////////////////////////////////////////////
  // 3. Check if any backtracking conditions are true: //
  ///////////////////////////////////////////////////////

  if (IsLimitingState(activeQueue))          // redefined by verifier!
  {
    delete lastTransition;
    lastTransition = NULL;

    goto backtracking;
  }

  ///////////////////////////////////////////
  // 4. Calculate set of active runnables: //
  ///////////////////////////////////////////

  activeRunnableList = ActiveRunnables(activeQueue);
  assert(!activeRunnableList->IsEmpty());

  ///////////////////////////////////////////
  // 5. Store state:                       //
  ///////////////////////////////////////////

  SystemStateSize(&size);
                                             // calculate system state
  state = new SCMem(size);
  assert(state);

  StoreSystemState(state);

  isAnalyzed = currentStateSpace->IsStateAnalyzed(state);

  //////////////////////////////////////////////////////////
  // 6. Check if this states closes a cycle in the graph: //
  //    NOTE: This must be done before 7. otherwise one-  //
  //          state-cycles are not detected! The problem  //
  //          is that IsAnalyzed must be called _before_  //
  //          IsCycleEndState so we have put it above     //
  //////////////////////////////////////////////////////////

  if (cycleDetection &&                 // cycle detection wanted ?
      isInCycleDetection &&             // cycle detection active ?
      !theSecondStack->IsEmpty() &&     // the first state reached by
                                        // the second DFS is always
                                        // on first stack, because
                                        // it is the state we have
                                        // started the second search at
      IsCycleEndState(state))           // this state closes the cycle ?
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "found cycle!!" << std::endl;
#endif

    delete activeRunnableList;
    activeRunnableList = NULL;
    
    // only for bit state hashing the state is obsolete.
    // if using full state space hashing the state is
    // already stored in the hash table by the call of
    // IsStateAnalyzed() above!
    
    if (currentStateSpace->GetType() != scFullStateSpace)
    {
      delete state;                       // free new state
      state = NULL;
    }

    if (LogError(scErrorCycleDetected))
    {
#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
      scValidationDebugLog << "switching to indet stack trace" << std::endl << std::endl;
#endif
      assert(theIndetStack);
      return theIndetStack->ChooseRunnable (activeQueue);
    }

    // the next four lines are only executed if no
    // stack trace for cycle detection is produced:
    delete lastTransition;
    lastTransition = NULL;

cycle_done:

    SwitchToOrigDFS(true);              // true = cycle found!

    goto pop;                           // pop the state, the second search
                                        // has started at (it has been
                                        // completely expanded in the first
                                        // search)
  }

  ////////////////////////////////////////////
  // 7. Check if state is already analyzed: //
  ////////////////////////////////////////////

  if (isAnalyzed)                       // reached analyzed state?
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "found analyzed state!!" << std::endl;
#endif

    delete activeRunnableList;
    activeRunnableList = NULL;
    delete state;                       // free new state
    state = NULL;
    delete lastTransition;
    lastTransition = NULL;

    goto backtracking;
  }

  ///////////////////////////////////////////////////
  // 8. Push new system state on validation stack: //
  ///////////////////////////////////////////////////

  stackElem = new SCStackElem(state,
                              currentStateSpace->GetCurrentKey(),
                              activeRunnableList,
                              lastRunnableIsAutomaton,
                              lastRunnable,
                              lastRunnableID,
                              lastTransition,      // set in ChooseTransition
                              lastSuccessor,
                              fulfilledAcceptanceSets, // only for verification
                              flipCounter,             //  "    "       "
                              numOfCreatedInstances,
                              createdInstances);
  assert(stackElem);
  currentStack->Push(stackElem);

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): pushing state (depth = ";
  scValidationDebugLog << currentStack->CurrentDepth() << ", created processes = ";
  scValidationDebugLog << numOfCreatedInstances << ")" << std::endl;
#endif

  lastTransition = NULL;      // clear variable
  createdInstances = NULL;    // variables are updated by SCIndetVal::Create
  numOfCreatedInstances = 0;  // (see below) which is called by
                              // SCScheduler::Create to keep track of
                              // created process instances during
                              // the execution of the last transition

  ////////////////////////////////////////////////////////////////////////
  // 9. Choose next possible runnable from the set of active runnables: //
  ////////////////////////////////////////////////////////////////////////

exit:

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): passing label exit";
  scValidationDebugLog << std::endl;
#endif

  if (activeRunnableList->IsEmpty())      // no active runnable left?
  {                                       // all possible successor states
                                          // are visited!
    if (cycleDetection &&                 // cycle detection wanted ?
        !isInCycleDetection &&            // cycle detection not already active?
        IsCycleStartState())              // is this state relevant for
    {                                     // cycle detection (e.g. an accepting
                                          // state)?
      numOfCycleStartStates++;
      SwitchToNestedDFS();

      return ChooseRunnable(activeQueue); // start nested DFS for cycle
                                          // detection; nested DFS
                                          // will terminate if all successors
                                          // of the current state (which is
                                          // the nested DFS start state)
                                          // are explored or
                                          // if a state that is a cycle
                                          // end state is reached (cycle end
                                          // states are states already stored
                                          // in stack of _first_ DFS, see
                                          // SCIndetVal::IsCycleEndState())
    }

pop:    
    stackElem = currentStack->Pop();      // pop top stack elem
    assert(stackElem);
    numOfCreatedInstances = stackElem->NumOfCreatedInstances();
    createdInstances = stackElem->GetCreatedInstances();
                                          // adjust variables for created
                                          // instances
                                          // needed for backtracking (see below)
    delete stackElem;                     // delete state elem

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "active runnable list is empty -> poping state (depth = ";
    scValidationDebugLog << currentStack->CurrentDepth() << ", left over processes = ";
    scValidationDebugLog << numOfCreatedInstances << ") -> backtracking" << std::endl;
#endif

    goto backtracking;
  }

  activeRunnable = (*activeRunnableList->Head())(); // choose first runnable
  assert(activeRunnable);                           // from active runnable list
  runnable = activeRunnable->GetRunnable();
  assert(runnable);

  if (activeRunnable->GetEnabledTransitionList() == NULL)
  {                                          // is it a machine or a timer or
                                             // a process executing the Request method?
#if _SC_VALIDATION_DEBUG
    if (!runnable->IsAutomaton())            // is it a machine or the
    {                                        // timer process?
      scValidationDebugLog << "SCIndetVal::ChooseRunnable(): choose ";
      scValidationDebugLog << *runnable << std::endl;
    }
    else                                     // it is a requesting process!
    {
      scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
      scValidationDebugLog << "choose requesting or calling ";
      scValidationDebugLog << *runnable << "." << std::endl;
    }
#endif

    lastSuccessor = activeRunnable->GetNextSuccessor();
    
    if (!activeRunnable->HasNextSuccessor()) // all Buechi-successors explored?
    {
      delete activeRunnableList->Remove();   // remove runnable without transition
                                             // list from active runnable list 
                                             // (we have no choice of action here)
                                             // and delete it (note: the activeRunnable
                                             // is deleted but not the embedded runnable,
                                             // which is stored in the variable 'runnable'!)
    }
    else              
    {
      // this code is only used for verifcation!
      if (type != scIndetVerification)
      {
        assert(false);  
      }

#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
      scValidationDebugLog << "leaving ";
      scValidationDebugLog << *runnable << " in active runnable list ";
      scValidationDebugLog << "(further buechi successors exists)." << std::endl;
#endif
    }
    executedTransitions++;                   // count pseudo-transitions
    lastTransition = NULL;

  }
  else                                       // runnable is a process executing
  {                                          // the state method!
    assert(!activeRunnable->GetEnabledTransitionList()->IsEmpty());

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): choose ";
    scValidationDebugLog << *runnable << "." << std::endl;
#endif
  }

  // don't remove runnable from activeQueue here, because its queues
  // (inputQueue, requestQueue, timerQueue) must be set to unmodified
  // even if the runnable itself will be set to modified (see below)

#if _SC_VALIDATION_OPTIMIZE
  SCProcessType::StaticUnsetModified(true);
  SCProcedureType::StaticUnsetModified();
  SCMachine::StaticUnsetModified(true);
  if (SCTimerControl::GetTimerControl())
  {
    SCTimerControl::GetTimerControl()->UnsetModified(true);
  }
  if (SCPath::GetPath())
  {
    SCPath::GetPath()->UnsetModified(true);
  }
#endif
 
#if _SC_VALIDATION_OPTIMIZE
  runnable->SetModified();                   // the choosen runnable will
                                             // always be modified (heuristic)
#endif

  timeEvent = activeQueue->LookupRunnable(runnable);
  assert(timeEvent);
  delete activeQueue->Remove(timeEvent);     // remove timeEvent of choosen
                                             // runnable from activeQueue

  // set variables which are used for stackElem
  // creation (see above):
  lastRunnableIsAutomaton = runnable->IsAutomaton();
  lastRunnable = runnable;                          
  lastRunnableID = runnable->GetID();               
  errorDumpActive = false;

  return runnable;                           // return runnable to the
                                             // scheduler

  //////////////////////////////////////////////////////////////////////////
  // 10. For some reason we can't run deeper into the reachability graph: //
  //     Reasons: 1. max Validation depth reached                         //
  //              2. reached an analyzed state                            //
  //              3. no (active) runnable in current state (deadlock)     //
  //              4. all possible successor state are visited             //
  //////////////////////////////////////////////////////////////////////////

backtracking:

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "passing label backtracking" << std::endl;
#endif

  stackElem = currentStack->Top();           // retrieve top element of stack
  if (stackElem == NULL)                     // back at start state of
  {                                          // current DFS
    // now we must distinguish if the first or the
    // second search is terminating:

    if (isInCycleDetection)                  // nested search running?
    {
      SwitchToOrigDFS(false);                // false = no cycle found!

      goto pop;                              // pop cycle test state
    }

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "empty stack -> validation ending." << std::endl;
#endif

    return NULL;
  }

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "backtracking!!!" << std::endl;
#endif

  // Delete left over process instances. These instances were generated
  // by the transition which led to the current state and stored
  // in an array of process IDs (createdInstances):
  if (createdInstances != NULL)
  {
    for (i = 0;
         i < numOfCreatedInstances;
         i++)
    {
      automaton = (SCAutomaton *)SCScheduler::GetRunnableFromID(createdInstances[i]);
      assert(automaton);                     // get left over process
                                             // or procedure instance

#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
      scValidationDebugLog << "stopping left over ";
      scValidationDebugLog << *automaton << std::endl;
#endif

      automaton->Stop(false);                // false = no tracing of
                                             // stop event!
    }
    delete[] createdInstances;               // free array of created instances
    numOfCreatedInstances = 0;               // clean variables wich keep
    createdInstances = NULL;                 // track of generated instances
  }

  // Restore system state. This implies reconstruction of missing instances
  // which have been destroyed by the choosen transition:
  state = stackElem->GetState();
  assert(state);

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "restoring system state" << std::endl;
#endif

  RestoreSystemState(state);

  assert(activeQueue->NumOfElems() != 0);

  // Restore list of active runnables:
  activeRunnableList = stackElem->GetActiveRunnableList();
  assert(activeRunnableList);

  // restore two members which are important for verification:
  fulfilledAcceptanceSets = stackElem->GetFulfilledAcceptanceSets();
  flipCounter = stackElem->GetFlipCounter();
  
  goto exit;                                 // lets try this state...
}


_SCINLINE_ SCEnabledTransition *SCIndetVal::ChooseTransition (void)
{
  SCEnabledTransition *     enabledTransition = NULL;
  SCEnabledTransitionList * enabledTransitionList;
  SCStackElem *             stackElem;
  SCActiveRunnable *        activeRunnable;
  SCActiveRunnableList *    activeRunnableList;
#if _SC_VALIDATION_DEBUG
  SCRunnable *              runnable;               // only for display purpose
  SCNatural                 numOfTransitionsLeft;   // only for display purpose
#endif

  stackElem = currentStack->Top();
  assert(stackElem);

  activeRunnableList = stackElem->GetActiveRunnableList();
  assert(activeRunnableList);
  assert(!activeRunnableList->IsEmpty());

  activeRunnable = (*activeRunnableList->Head())();
  assert(activeRunnable);

#if _SC_VALIDATION_DEBUG
  runnable = activeRunnable->GetRunnable();         // store for later display
#endif
  
  enabledTransitionList = activeRunnable->GetEnabledTransitionList();
  assert(enabledTransitionList);

  lastSuccessor = activeRunnable->GetNextSuccessor();
  
  if (!activeRunnable->HasNextSuccessor())        // all successors choosen?
  {
    enabledTransition = enabledTransitionList->Remove(); // remove first elem
    activeRunnable->ResetSuccessors();
  }
  else
  {
    // this code is used only for verification!

    if (type != scIndetVerification)
      assert(false);
      
    // make a copy of first element, because the return value 'enabledTransition'
    // will be destroyed by SCAutomaton::Transition():

    enabledTransition = new SCEnabledTransition(*(*enabledTransitionList->Head())());
                                                         // copy first elem

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseTransition(): ";
    scValidationDebugLog << "leaving ";
    scValidationDebugLog << *enabledTransition << " in enabled transition list (further buechi successors)." << std::endl;
#endif

  }

  assert(enabledTransition);

#if _SC_VALIDATION_DEBUG
  numOfTransitionsLeft = enabledTransitionList->NumOfElems();
                                                    // store for later display
  if (type == scIndetVerification)
  {
    scValidationDebugLog << "SCIndetVal::ChooseTransition(): ";
    scValidationDebugLog << "using ";
    scValidationDebugLog << lastSuccessor;
    scValidationDebugLog << ". buechi successor" << std::endl;
  }
#endif

  if (enabledTransitionList->IsEmpty())             // no enabled transitions left?
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseTransition(): ";
    scValidationDebugLog << "remove exhausted ";
    scValidationDebugLog << *runnable;
    scValidationDebugLog << " from active runnable list" << std::endl;
#endif

    delete activeRunnableList->Remove();     // remove "exhausted" runnable
                                             // from active runnable list
                                             // and delete it (NOTE: this
                                             // is an implicit invalidation
                                             // of activeRunnable, so
                                             // activeRunnable should not
                                             // be accessed in the sequel)
  }

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCIndetVal::ChooseTransition(): ";
  scValidationDebugLog << *runnable;
  scValidationDebugLog << " executing " << *enabledTransition << std::endl;
  scValidationDebugLog << "SCIndetVal::ChooseTransition(): ";
  scValidationDebugLog << numOfTransitionsLeft;
  scValidationDebugLog << " enabled transitions left" << std::endl;
#endif

  executedTransitions++;

  lastTransition = new SCEnabledTransition(*enabledTransition);
                                             // store last executed
                                             // transition for error tracing
  return (enabledTransition);                // give it back to Transition()
                                             // method of SCProcess
}


SCBoolean SCIndetVal::LogError (SCErrorCode errorCode) // deadlock, failed assertion
{
  SCTraceEventMSC *errorTracing;
  char errorName[80];
  char errorFile[80];

  if (!FindErrors() ||                       // want to find errors?
      NumOfErrors() >= MaxNumOfErrors() ||   // max number of errors reported?
      errorDumpActive)                       // don't go recursive!
  {
    return false;
  }

  lastError = errorCode;
  numOfErrors++;

  // if this is the last error, we can deactivate cycleDetection
  // because it doesn't matter any more:
  if (numOfErrors == MaxNumOfErrors() &&
      cycleDetection)
  {
    cycleDetection = false;
  }
  
  if (theIndetStack)      // left over from previous error ?
    delete theIndetStack;

  switch (errorCode)
  {
    case scErrorDeadlock:
      numOfDeadlocks++;
      sprintf(errorName, "Deadlock%ld", numOfDeadlocks);
      sprintf(errorFile, "Deadlock%ld.mpr", numOfDeadlocks);
      break;

    case scErrorAssertionFailed:
      numOfFailedAssertions++;
      sprintf(errorName, "FailedAssertion%ld", numOfFailedAssertions);
      sprintf(errorFile, "FailedAssertion%ld.mpr", numOfFailedAssertions);
      break;

    case scErrorCycleDetected:
      numOfCycleDetections++;
      sprintf(errorName, "CycleDetection%ld", numOfCycleDetections);
      sprintf(errorFile, "CycleDetection%ld.mpr", numOfCycleDetections);
      break;

    case scErrorSignalDrop:
      numOfSignalDrops++;
      sprintf(errorName, "SignalDrop%ld", numOfSignalDrops);
      sprintf(errorFile, "SignalDrop%ld.mpr", numOfSignalDrops);
      break;

    case scErrorSignalLose:
      numOfSignalLosses++;
      sprintf(errorName, "QueueOverflow%ld", numOfSignalLosses);
      sprintf(errorFile, "QueueOverflow%ld.mpr", numOfSignalLosses);
      break;

    case scErrorNoSignalReceiver:
      numOfMissingReceivers++;
      sprintf(errorName, "MissingReceiver%ld", numOfMissingReceivers);
      sprintf(errorFile, "MissingReceiver%ld.mpr", numOfMissingReceivers);
      break;

    case scErrorProcessCreationFailed:
      numOfFailedProcessCreations ++;
      sprintf(errorName, "FailedProcessCreation%ld", numOfFailedProcessCreations);
      sprintf(errorFile, "FailedProcessCreation%ld.mpr", numOfFailedProcessCreations);
      break;

    default:
      return false;
  }

  errorTracing = new SCTraceEventMSC(new SCStream(errorFile), // trace stream
                                     scfTraceAll,             // all events !
                                     true,                    // with data !
                                     errorName);              // title of MSC
  assert (errorTracing);

  theIndetStack = new SCIndetStackTrace(this, errorTracing);
  assert (theIndetStack);

  SCScheduler::SetIndet(theIndetStack);

  errorDumpActive = true;
    
  return true;
}


SCBoolean SCIndetVal::LogError (SCErrorCode errorCode,    // signal drop or
                                const SCProcess *process, // signal rejected
                                const SCSignal *signal)
{
  (void)process;
  (void)signal;

  return SCIndetVal::LogError(errorCode);
}


SCBoolean SCIndetVal::LogError (SCErrorCode errorCode,  // no receiver
                                const SCProcess *process,
                                const SCSignalType *signal)
{
  (void)process;
  (void)signal;

  return SCIndetVal::LogError(errorCode);
}


SCBoolean SCIndetVal::LogError (SCErrorCode errorCode,    // process creation
                                const SCProcess *pParent, // failed
                                const SCProcessType *offspringType)
{
  (void)pParent;
  (void)offspringType;

  return SCIndetVal::LogError(errorCode);
}


SCErrorCode SCIndetVal::IsErrorState (SCTimeEventSaveList *activeQueue)
{
  if (activeQueue->IsEmpty())
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::IsErrorState(): ";
    scValidationDebugLog << "no runnables left (deadlock)" << std::endl;
#endif

    SCTraceControl::LogEvent (scTraceDeadlock);

    lastError = scErrorDeadlock;

    return lastError;
  }

  return scErrorNoError;
}


SCBoolean SCIndetVal::IsLimitingState (SCTimeEventSaveList *activeQueue)
{
  (void)activeQueue;

  if (currentStack->MaxDepth() &&                   // max depth given?
      currentStack->CurrentDepth() == currentStack->MaxDepth())
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::IsLimitingState(): ";
    scValidationDebugLog << "reached max validation depth!!" << std::endl;
#endif
    truncatedPathes++;
    
    return true;
  }

  if (hasStopTime &&                                // stop time given?
      SCScheduler::GetCurrentTime() >= stopTime)    // stop time reached?
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::IsLimitingState(): ";
    scValidationDebugLog << "reached max validation time!!" << std::endl;
#endif
    return true;
  }

  if (stopCondition && stopCondition->Evaluate() == true)
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::IsLimitingState(): ";
    scValidationDebugLog << "reached state with fulfilled backtracking condition!!" << std::endl;
#endif
    return true;
  }

  return false;
}


SCBoolean SCIndetVal::IsCycleStartState (void) const
{
  return true;
}


SCBoolean SCIndetVal::IsCycleEndState (SCMem *state)
{
  assert(theFirstStack);
  assert(currentStateSpace == theCycleStateSpace);

  cycleClosingElem = theFirstStack->Find(currentStateSpace->GetCurrentKey(), state);
  
  if (cycleClosingElem)
  {
    return true;
  }
   
  return false;
}


SCStream& operator<< (SCStream& pStream,
                     const SCIndetVal& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCIndetVal::Display (SCStream& pStream) const
{
  if (executedTransitions == 0) // called for start message?
  {
    if (type == scIndetValidation)
    {
      pStream << std::endl << "This is QVAL version " << SCL_VALIDATOR_VERSION;
#ifndef _PRODUCTION_RELEASE
      pStream << " (Optimization Level " << _SC_VALIDATION_OPTIMIZE << ")";
#endif
      pStream << std::endl << std::endl;
    }

    SCIndet::Display(pStream);

    if (currentStack->MaxDepth() == 0)
      pStream << "Max. depth:                 unlimited" << std::endl;
    else
      pStream << "Max. depth:                 " << currentStack->MaxDepth() << std::endl;

    if (stopCondition)
    {
      pStream << "State filter:               " << stopCondition->name << std::endl;
    }

    pStream << *theStateSpace << std::endl;

    pStream << "Cycle detection:            ";
    if (cycleDetection)
      pStream << "activated" << std::endl;
    else
      pStream << "not activated" << std::endl;

    if (type == scIndetValidation)
    {
      pStream << std::endl;
    }
  }
  else  // called for end message!
  {
    if (type == scIndetValidation)
    {
      pStream << std::endl << "Validator ";
    }

    SCIndet::Display(pStream);

    pStream << "Analyzed states:          ";
    pStream << theStateSpace->GetNumOfAnalyzedStates();
    if (theCycleStateSpace)
    {
      pStream << " / " << theCycleStateSpace->GetNumOfAnalyzedStates();
    }
    pStream << std::endl;

    if (realExecTime > 0)
    {
      pStream << "States per second:        ";
      if (theCycleStateSpace)
      {
        pStream << ((theStateSpace->GetNumOfAnalyzedStates() +
//                     theCycleStateSpace->GetNumOfAnalyzedStates()) / totalSeconds);
                     (float)theCycleStateSpace->GetNumOfAnalyzedStates()) / ((float)realExecTime / 1000.0));
      }
      else
      {
//        pStream << (theStateSpace->GetNumOfAnalyzedStates() / totalSeconds);
        pStream << ((float)theStateSpace->GetNumOfAnalyzedStates() / ((float)realExecTime / 1000.0));
      }
      pStream << std::endl;
    }

    pStream << "Reached validation depth: " << theFirstStack->ReachedDepth();
    if (theSecondStack)
    {
      pStream << " / " << theSecondStack->ReachedDepth();
    }
    pStream << std::endl;

    pStream << "Truncated pathes:         " << truncatedPathes << std::endl;

    pStream << "Hash table utilization:   ";
    pStream << (double)theStateSpace->GetNumOfAnalyzedStates() / (double)(theStateSpace->GetStateSpaceSize()) * 100.0 << " %";
    if (theCycleStateSpace)
    {
      pStream << " / ";
      pStream << (double)theCycleStateSpace->GetNumOfAnalyzedStates() / (double)(theCycleStateSpace->GetStateSpaceSize()) * 100.0 << " %";
    }
    pStream << std::endl;
    if (theStateSpace->GetNumOfAnalyzedStates() > 0)
    {
      pStream << "Min. state size:          " << minStateSize << " bytes" << std::endl;
      pStream << "Max. state size:          " << maxStateSize << " bytes" << std::endl;
    }

    if (numOfCycleStartStates)
    {
      pStream << "Started nested DFS:       " << numOfCycleStartStates << std::endl;
    }
    
    if (type == scIndetValidation)
    {
      pStream << std::endl;
    }
  }

  return pStream;
}


void SCIndetVal::SwitchToNestedDFS (void)
{
  assert(isInCycleDetection == false);
  
  assert(currentStack == theFirstStack);
  assert(currentStateSpace == theStateSpace);

  currentStateSpace = theCycleStateSpace;
  currentStack = theSecondStack;
  isInCycleDetection = true;
  
  lastRunnableIsAutomaton = false;  // produce a 'clean' start state!
  lastRunnableID = kSCNoProcessID;
  lastRunnable = NULL;
  lastTransition = NULL;
  lastSuccessor = 1;

#if _SC_DEBUGOUTPUT
  scDebugLog << std::endl << "Switching to nested DFS!" << std::endl << std::endl;
#endif

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "switching to nested DFS." << std::endl << std::endl;
#endif
}


void SCIndetVal::SwitchToOrigDFS (SCBoolean cycleFound)
{
  assert(isInCycleDetection == true);

  assert(currentStack == theSecondStack);
  assert(currentStateSpace == theCycleStateSpace);

  currentStack = theFirstStack;
  currentStateSpace = theStateSpace;
  isInCycleDetection = false;
 
  if (cycleFound)
  {
    theSecondStack->Empty();            // empty the stack for cycle detection   

    // set modified flags of all runnables
    // (Reason: we want to disable all backtracking optimizations
    // since they work only for backtracking to previous
    // state and not over more than one state!)

#if _SC_VALIDATION_OPTIMIZE
    SCProcessType::StaticSetModified(true);
    SCProcedureType::StaticSetModified();
//    SCProcessType::StaticStop();
//    SCProcedureType::StaticStop();

    SCMachine::StaticSetModified(true);

    if (SCTimerControl::GetTimerControl())
      SCTimerControl::GetTimerControl()->SetModified(true);
    if (SCPath::GetPath())
      SCPath::GetPath()->SetModified(true);
 
//    SCScheduler::activeQueue->SetModified();
 
#endif
  }
  else  // no cycle found, we're back at start state of nested DFS!
  {
    assert(theSecondStack->IsEmpty());

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndetVal::ChooseRunnable(): ";
    scValidationDebugLog << "empty cycle stack -> no cycles found." << std::endl;
#endif
  }

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << std::endl << "SCIndetVal::ChooseRunnable(): ";
  scValidationDebugLog << "switching back to original DFS." << std::endl << std::endl;
#endif
}
