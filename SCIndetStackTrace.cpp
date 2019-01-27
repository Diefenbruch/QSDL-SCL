/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-----------------+--------------------+---------------+------------------+----+
|   Module        |   File             |   Created     |   Project        |    |
+-----------------+--------------------+---------------+------------------+----+
|SCIndetStackTrace|SCIndetStackTrace.cc| 25. Apr 1996  |   SCL            |    |
+-----------------+--------------------+---------------+------------------+----+
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

#include "SCIndetStackTrace.h"
#include "SCProcess.h"
#include "SCMachine.h"
#include "SCTimerControl.h"
#include "SCTimeEvent.h"
#include "SCEnabledTransition.h"
#include "SCTraceControl.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCProcessTypeList.h"
#include "SCMachineList.h"
#include "SCProcessList.h"
#include "SCPath.h"
#include "SCStackElem.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCIndetStackTrace.inl.h"
#endif


SCIndetStackTrace::SCIndetStackTrace(SCIndetVal * indetVal,
                                     SCTrace * errTrace) :
  SCIndet(scIndetStackTrace, 0.0, true),
  theIndetVal(indetVal),
  curStackPos(NULL),
  curDepth(0),
  errorTracing(errTrace),
  finish1(false),
  finish2(false),
  finish3(false),
  lastElem(NULL)
{
  assert (errorTracing);
}


SCIndetStackTrace::~SCIndetStackTrace (void)
{
  if (lastElem)
  {
    delete lastElem;
  }
}


SCRunnable *SCIndetStackTrace::ChooseRunnable (SCTimeEventSaveList *activeQueue)
{
  SCRunnable *  lastRunnable = NULL;                // Initialize with NULL,
                                                    // in case of end of stack.
  SCTimeEvent * timeEvent;

  if (curStackPos == NULL)                          // called first time ?
  {
#if _SC_STACKTRACE_DEBUG
    scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() started " << std::endl;
#endif

    origVerbose = SCScheduler::IsVerbose();         // save original verbosity
    SCScheduler::SetIsVerbose(false);               // be quiet while error
                                                    // dumping
    StartMessage();                                 // print start message

    RestoreStartState();                            // reset system to its
                                                    // initial state (this can
                                                    // be a long jump!)
    assert (curStackPos);                           // is restored to start
                                                    // start now
  }

  // We look in the following stackElem to see
  // which runnable was executed last. This is
  // the runnable we must choose now
  // (last of next = current !) :

  curStackPos = curStackPos->GetNextElem();         // get successor state

action:  
  if (curStackPos)                                  // Not end of stack.
  {
    // test if we trace for cycle detection and if
    // we have reached the elem which closes the
    // cycle (this is an element an the first stack):
    if (theIndetVal->lastError == scErrorCycleDetected &&
        curStackPos == theIndetVal->cycleClosingElem)
    {
      errorTracing->LogEvent (scTraceCycleEnd);
    }

    lastRunnable = curStackPos->GetLastActiveRunnable();
    assert(lastRunnable);

    timeEvent = activeQueue->LookupRunnable(lastRunnable);
    assert(timeEvent);
  
    SCScheduler::SetCurrentTime(timeEvent->GetTime());
                                                    // adjust scheduler time if
                                                    // necessary

    delete activeQueue->Remove(timeEvent);

    curDepth++;
    
    if (!finish1 &&                                 // still running in first stack ?
        !curStackPos->GetNextElem() &&              // reached last element of stack ?
        theIndetVal->isInCycleDetection)            // cycle detection active?
    {
       if (theIndetVal->lastError == scErrorCycleDetected)
       {
         errorTracing->LogEvent (scTraceCycleStart);
       }
    }
  }
  else                                              // end of stack (i.e. we have
  {                                                 // reached the predecessor 
                                                    // state of the 'error' state
                                                    // now)
#if _SC_STACKTRACE_DEBUG
    scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() end of stack reached" << std::endl;
#endif

    if (!finish1 &&                                 // Validator run in cylce
        theIndetVal->isInCycleDetection)            // detection mode when dump
    {                                               // was started
       assert(theIndetVal->theSecondStack);

#if _SC_STACKTRACE_DEBUG
       scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() processing second stack..." << std::endl;
#endif
       curStackPos = theIndetVal->theSecondStack->Root();
       assert(curStackPos);
       
       // leave out first stack elem since it is
       // already stored as last element of first
       // stack:
       curStackPos = curStackPos->GetNextElem();
       
       finish1 = true;
       
       goto action;
    }
    
    if (!finish2 && theIndetVal->lastTransition)
    {
      // create stack elem for error state (this is not
      // yet done by validator)
      
#if _SC_STACKTRACE_DEBUG
      scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() creating last stack element" << std::endl;
#endif

      curStackPos = new SCStackElem(NULL, 0, NULL,
                                    theIndetVal->lastRunnableIsAutomaton,
                                    theIndetVal->lastRunnable,
                                    theIndetVal->lastRunnableID,
                                    new SCEnabledTransition(*theIndetVal->lastTransition),
                                    theIndetVal->lastSuccessor,
                                    SC_BS0);
      assert (curStackPos);
      finish2 = true;

      lastElem = (SCStackElem *)curStackPos;
      
      goto action;
    }

    // For deadlock errors (which occur inside ChooseRunnable() of
    // SCIndetVal) we must log the error here:
    if (theIndetVal->lastError == scErrorDeadlock)
    {
      errorTracing->LogEvent (scTraceDeadlock);
      assert(activeQueue->IsEmpty());
    }

    errorTracing->LogEvent (scTraceSchedulerStop);

    // now we must restore the things we have changed
    // at the start of the error dump:

    SCScheduler::SetIndet(theIndetVal);
    EndMessage();

    SCTraceControl::RemoveTrace (errorTracing);
    delete errorTracing;
    errorTracing = NULL;
    
    SCScheduler::SetIsVerbose (origVerbose);

#if _SC_STACKTRACE_DEBUG
    scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() finished " << std::endl;
    scStackTraceDebugLog.GetStream().flush();
#endif

    return theIndetVal->ChooseRunnable(activeQueue);
  }

#if _SC_STACKTRACE_DEBUG
  assert(curStackPos);
  
  if (lastRunnable)
  {
    scStackTraceDebugLog << "SCIndetStackTrace::ChooseRunnable() choose ";
    scStackTraceDebugLog << *lastRunnable << ", depth = " << curStackPos->depth << std::endl;
  }
#endif
 
  return lastRunnable;
}


SCEnabledTransition *SCIndetStackTrace::ChooseTransition (void)
{
  SCEnabledTransitionList * enabledTransitionList = NULL;
  SCEnabledTransition *     enabledTransition;
  SCAutomaton *             automaton;
  
  assert (curStackPos);
  assert (curStackPos->IsLastActiveRunnableAutomaton()); // Pointless, if it is a machine.
  assert (curStackPos->GetLastTransition());

  automaton = (SCAutomaton *)curStackPos->GetLastActiveRunnable();
  assert(automaton);

  // We must remove the enabled transitions of the automaton:
  // If we don't do this here the SCAutomaton::State() method
  // thinks that the automaton is still active because the
  // enabledTransitionList ist not NULL.

  if (automaton->GetEnabledTransitions())
  {
    enabledTransitionList = automaton->RetrieveEnabledTransitions();
    assert(enabledTransitionList);
    delete enabledTransitionList;

#if _SC_STACKTRACE_DEBUG
    scStackTraceDebugLog << "SCIndetStackTrace::ChooseTransition() enabled transitions retrieved" << std::endl;
#endif
  }

  // Use the transition stored on the stack:
  assert(curStackPos->GetLastTransition());
  enabledTransition = new SCEnabledTransition(*curStackPos->GetLastTransition());
  assert (enabledTransition);

#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::ChooseTransition() ";
  scStackTraceDebugLog << "executing " << *enabledTransition << std::endl;
#endif

  return enabledTransition;
}


void SCIndetStackTrace::RestoreStartState (void)
{
  SCStackElem *       stackElem;
  SCMem *             state;
  SCMachineIter *     iterM =NULL;
  SCProcessTypeIter * iterPT;
  SCProcessIter *     iterP;
  SCMachine *         machine;
  SCProcessType *     processType;
  SCProcess *         process;
  
#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() started" << std::endl;
#endif

  // get start state:
  stackElem = theIndetVal->theFirstStack->Root();

  state = stackElem->GetState();
  assert(state);

  // set modified flags of all runnables
  // (Reason: we want to disable all backtracking optimizations
  // since they work only for backtracking to previous
  // state and not over more than one state!)

#if _SC_VALIDATION_OPTIMIZE
//  SCProcessType::StaticSetModified(true); // why is this not the same
//  SCProcedureType::StaticSetModified();   // as StaticStop ?? MD
                                            // test tcp -vy r -ue to
                                            // see the difference!

  SCProcessType::StaticStop();
  SCProcedureType::StaticStop();

  SCMachine::StaticSetModified(true);

  if (SCTimerControl::GetTimerControl())
    SCTimerControl::GetTimerControl()->SetModified(true);
  if (SCPath::GetPath())
    SCPath::GetPath()->SetModified(true);
#endif
    
#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() all runnables set to modified" << std::endl;
#endif

  // Add MSC-Error-Tracing:

  assert(errorTracing);
  SCTraceControl::AddTrace(errorTracing);         // add error tracing
  errorTracing->LogEvent(scTraceSchedulerInit);    

#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() error tracing set " << std::endl;
#endif

  // Rebuilding queues of start state:

  theIndetVal->RestoreSystemState(state);

#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() initial state restored " << std::endl;
#endif

  // trace information about runnable creation:

  iterPT = new SCProcessTypeIter (*SCProcessType::GetProcessTypeList());
  assert (iterPT);

  for (processType = (*iterPT)++;
       processType;
       processType = (*iterPT)++)
  {
    iterP = new SCProcessIter (*processType->GetProcessList());
    assert (iterP);
  
    for (process = (*iterP)++;
         process;
         process = (*iterP)++)
    {
      process->TraceOn();
      errorTracing->LogEvent (scTraceProcessCreate, process, (SCProcess *)NULL);
    }
    
    delete iterP;
  }
  
  delete iterPT;

  iterM = new SCMachineIter (*SCMachine::GetMachineList());
  assert (iterM);

  for (machine = (*iterM)++;
       machine;
       machine = (*iterM)++)
  {
    errorTracing->LogEvent (scTraceMachineCreate, machine);
  }

  delete iterM;
  
#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() trace start infos generated " << std::endl;
#endif

  // Procedure runnables cannot exist at system startup!
  
  curStackPos = stackElem;

#if _SC_STACKTRACE_DEBUG
  scStackTraceDebugLog << "SCIndetStackTrace::RestoreStartState() finished " << std::endl;
#endif
}


SCErrorCode SCIndetStackTrace::IsErrorState (SCTimeEventSaveList *activeQueue)
{
  (void)activeQueue;

  return scErrorNoError;
}


SCBoolean SCIndetStackTrace::IsLimitingState (SCTimeEventSaveList *activeQueue)
{
  (void)activeQueue;

  return false;
}

