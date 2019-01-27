/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       Universitaet Essen                                                     |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCIndetSim    | SCIndetSim.cc     | 25. Apr 1996  |   SCL             |      |
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

#include "SCBasicTypes.h"
#include "SCStream.h"
#include "SCIndetSim.h"
#include "SCProcess.h"
#include "SCSignal.h"
#include "SCProcessType.h"
#include "SCActiveRunnable.h"
#include "SCTimeEvent.h"
#include "SCDebug.h"

#if _SC_SIMULATION_DEBUG
  #include "SCEnabledTransition.h"
#endif

#include "SCTraceControl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCIndetSim.inl.h"
#endif


SCIndetSim::SCIndetSim (SCTime maxTime,
                        SCBoolean errors,
                        SCNatural numErrors,
                        SCBoolean deadCode,
                        SCNatural depth,
                        SCName2PropFuncPtr * qAtom):
  SCIndet(scIndetSimulation, maxTime, false,
          errors, numErrors, deadCode, qAtom),
  maxDepth(depth),
  enabledTransitionList(NULL)
{
  randomizer = new SCRandUniform (0.0, 1.0);
  assert(randomizer);
}


SCIndetSim::~SCIndetSim (void)
{
  if (randomizer)
    delete randomizer;

  if (enabledTransitionList)
    delete enabledTransitionList;
}


SCRunnable *SCIndetSim::ChooseRunnable (SCTimeEventSaveList *activeQueue)
{
  SCRunnable *           runnable;
  SCAutomaton *          automaton;
  SCActiveRunnable *     activeRunnable;
  SCActiveRunnableList * activeRunnableList;
  SCNatural              choice;
  SCTimeEvent *          timeEvent;
  
  ///////////////////////////////////////////////////////
  // 1. Check if state is faulty:                      //
  ///////////////////////////////////////////////////////

  if (IsErrorState(activeQueue))
  {
    return NULL;
  }

  ///////////////////////////////////////////////////////
  // 2. Check if any backtracking conditions are true: //
  ///////////////////////////////////////////////////////

  if (IsLimitingState(activeQueue))
  {
    return NULL;
  }

  activeRunnableList = ActiveRunnables(activeQueue);
  assert(!activeRunnableList->IsEmpty());

  choice = NonDetChoice (activeRunnableList->NumOfElems());

  activeRunnable = activeRunnableList->Remove(choice);
  assert(activeRunnable);

  delete activeRunnableList;

  runnable = activeRunnable->GetRunnable();
  assert (runnable);

  delete activeRunnable;
  
  if (!runnable->IsAutomaton())                     // machines or timer are
  {                                                 // always a good choice!
#if _SC_SIMULATION_DEBUG
    scSimulationDebugLog << "SCIndetSim::ChooseRunnable(): choose ";
    scSimulationDebugLog << *runnable << std::endl;
#endif

    executedTransitions++;                          // count pseudo-transitions
  }
  else                                              // runnable IS automaton!
  {
    automaton = (SCAutomaton *)runnable;

    // if the automaton is suspended in its State() method
    // we need the set of enabled transitions for
    // the ChooseTransition() method (in simulation we don't
    // determine the enabled transition in method ActiveRunnables()
    // like in validation, because we want to avoid the
    // overhead produced by calcualting the enabled transitions
    // for all processes if we finally uses only one process!):

    if (automaton->IsInState())
    {
#if _SC_SIMULATION_DEBUG
      scSimulationDebugLog << "SCIndetSim::ChooseRunnable(): choose ";
      scSimulationDebugLog << *automaton << "." << std::endl;
#endif

      enabledTransitionList = automaton->RetrieveEnabledTransitions();
      
      // if we have continuous signals (whose enabling conditions
      // are all false) in timed states, it is possible that
      // enabledTransitionList _IS_ NULL!

//      assert (enabledTransitionList);
    }
#if _SC_SIMULATION_DEBUG
    else
    {
      scSimulationDebugLog << "SCIndetSim::ChooseRunnable(): ";
      scSimulationDebugLog << "choose requesting or calling ";
      scSimulationDebugLog << *automaton << "." << std::endl;
    }
#endif
  }

  timeEvent = activeQueue->LookupRunnable(runnable);
  if (!timeEvent)
  {
    // if this happens then something is wrong
    // with SCTable::Find()!
    
    SCScheduler::outputStream << std::endl << "Can't find time event in active queue for runnable ";
    SCScheduler::outputStream << *runnable << std::endl;

    assert(timeEvent);
    
    return NULL;
  }
  
  delete activeQueue->Remove(timeEvent);            // remove runnable of choosen active
                                                    // runnable from activeQueue

  return runnable;                                  // return runnable to the Scheduler
}


_SCINLINE_ SCEnabledTransition *SCIndetSim::ChooseTransition (void)
{
  SCEnabledTransition * enabledTransition = NULL;
  SCNatural             choice;
#if _SC_SIMULATION_DEBUG
  SCNatural             numOfTransitionsLeft;   // only for display purpose
#endif

  if (!enabledTransitionList) // possible for continuous signals
  {                           // in timed states (see above)
    return NULL;
  }
  
  assert(enabledTransitionList);
  assert(!enabledTransitionList->IsEmpty());
  
  choice = NonDetChoice (enabledTransitionList->NumOfElems());
  enabledTransition = enabledTransitionList->Remove(choice);
                                                    // take random elem
  assert(enabledTransition);

#if _SC_SIMULATION_DEBUG
  numOfTransitionsLeft = enabledTransitionList->NumOfElems();
                                                    // store for later display
#endif

  delete enabledTransitionList;
  enabledTransitionList = NULL;
  
#if _SC_SIMULATION_DEBUG
  scSimulationDebugLog << "SCIndetSim::ChooseTransition(): executing ";
  scSimulationDebugLog << *enabledTransition;
  scSimulationDebugLog << " (";
  scSimulationDebugLog << numOfTransitionsLeft;
  scSimulationDebugLog << " enabled transitions left)" << std::endl;
#endif

  executedTransitions++;
    
  return (enabledTransition);                       // give it back to Transition()
                                                    // method of SCProcess
}


_SCINLINE_ SCBoolean SCIndetSim::LogError (SCErrorCode errorCode)  // deadlock or
{                                                                  // violated assertion
  if (!FindErrors() || NumOfErrors() >= MaxNumOfErrors())
    return false;

  numOfErrors++;
  
  switch (errorCode)
  {
    case scErrorDeadlock:
      SCScheduler::errorStream << std::endl << "DEADLOCK DETECTED ! " << std::endl;
      numOfDeadlocks++;
      break;

    case scErrorAssertionFailed:
      SCScheduler::errorStream << std::endl << "VIOLATED ASSERTION ! " << std::endl;
      numOfFailedAssertions++;
      break;

    default:
      break;
  }
  return true;
}


_SCINLINE_ SCBoolean SCIndetSim::LogError (SCErrorCode errorCode,    // signal drop or
                                           const SCProcess *process, // signal rejected
                                           const SCSignal *signal)
{
  if (!FindErrors() || NumOfErrors() >= MaxNumOfErrors())
    return false;

  numOfErrors++;

  switch (errorCode)
  {
    case scErrorSignalDrop:
      SCScheduler::errorStream << std::endl << "PROCESS " << *process << " " << *process->GetLastState() << " DROPPED " << *signal << std::endl;
      numOfSignalDrops++;
      break;

    case scErrorSignalLose:
      SCScheduler::errorStream << std::endl << "PROCESS " << *process << " REJECTED " << *signal << std::endl;
      numOfSignalLosses++;
      break;

    default:
      break;
  }
  return true;
}


_SCINLINE_ SCBoolean SCIndetSim::LogError (SCErrorCode errorCode,  // no receiver
                                           const SCProcess *process,
                                           const SCSignalType *signal)
{
  if (!FindErrors() || NumOfErrors() >= MaxNumOfErrors())
    return false;

  numOfErrors++;

  switch (errorCode)
  {
    case scErrorNoSignalReceiver:
      SCScheduler::errorStream << std::endl << "PROCESS " << *process << " CAN'T SEND " << *signal << std::endl;
      numOfMissingReceivers++;
      break;

    default:
      break;
  }
  return true;
}


_SCINLINE_ SCBoolean SCIndetSim::LogError (SCErrorCode errorCode,   // process creation
                                           const SCProcess *father, // failed
                                           const SCProcessType *offspringType)
{
  if (!FindErrors() || NumOfErrors() >= MaxNumOfErrors())
    return false;

  numOfErrors++;

  switch (errorCode)
  {
    case scErrorProcessCreationFailed:
      SCScheduler::errorStream << std::endl << "INSTANCE CREATION OF PROCESS TYPE " << *offspringType << " FAILED, CREATOR = " << *father << std::endl;
      numOfFailedProcessCreations++;
      break;

    default:
      break;
  }
  return true;
}


SCErrorCode SCIndetSim::IsErrorState (SCTimeEventSaveList *activeQueue)
{
  if (activeQueue->IsEmpty())                       // deadlock ?
  {
    SCTraceControl::LogEvent (scTraceDeadlock);

    LogError(scErrorDeadlock);

    return scErrorDeadlock;
  }

  return scErrorNoError;
}


SCBoolean SCIndetSim::IsLimitingState (SCTimeEventSaveList *activeQueue)
{
  (void)activeQueue;

  if (hasStopTime &&                                // time limit given?
      SCScheduler::GetCurrentTime() >= stopTime)    // time limit reached?
  {
    return true;
  }

  if (maxDepth &&                                   // max depth given?
      executedTransitions >= maxDepth)              // max depth reached?
  {
    return true;
  }

  if (stopCondition && stopCondition->Evaluate() == true)
  {
    SCScheduler::outputStream << std::endl << "Stop condition '" << stopCondition->name;
    SCScheduler::outputStream << "' is fulfilled!" << std::endl;
    return true;
  }
  
  return false;
}


SCStream& operator<< (SCStream& pStream,
                     const SCIndetSim& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCIndetSim::Display (SCStream& pStream) const
{
  if (executedTransitions == 0) // called for start message?
  {
    if (type == scIndetSimulation)
    {
      pStream << std::endl << "This is QSIM version ";
      pStream << SCL_SIMULATOR_VERSION << std::endl << std::endl;
    }

    SCIndet::Display(pStream);

    if (stopCondition)
    {
      pStream << "Stop condition:             " << stopCondition->name << std::endl;
    }

    switch (SCScheduler::GetProgress())
    {
      case kSCMaximumProgress:
        pStream << "Simulation progress:        maximum" << std::endl << std::endl;
        break;
      case kSCFairProgress:
        pStream << "Simulation progress:        fair" << std::endl << std::endl;
        break;
      default:
        assert(false);
    }
  }
  else  // called for end message!
  {
    if (type == scIndetSimulation)
    {
      pStream << std::endl << "Simulator ";
    }

    SCIndet::Display(pStream);
    
    pStream << "Reached model time:       " << SCScheduler::GetCurrentTime();
    pStream << std::endl << std::endl;
  }
  
  return pStream;
}
