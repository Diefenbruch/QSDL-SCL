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
|   SCIndet     |   SCIndet.cc      | 25. Apr 1996  |   SCL             |      |
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

#include <stdio.h>
#include <time.h>
#include <chrono>

#include "SCStream.h"
#include "SCIndet.h"
#include "SCRunnable.h"
#include "SCScheduler.h"
#include "SCProcess.h"
#include "SCTimeEvent.h"
#include "SCActiveRunnable.h"
#include "SCDebug.h"
#include "SCStateType.h"
#include "SCTransition.h"
#include "SCProcessType.h"
#include "SCProcessTypeList.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

using namespace std::chrono;

SCIndet::SCIndet (SCIndetType pIndetType,
                  SCTime theStopTime,
                  SCBoolean preEnabTrans,
                  SCBoolean errors,
                  SCNatural numErrors,
                  SCBoolean deadCode,
                  SCName2PropFuncPtr * stopAtom) :
  SCObject(SC_INDET, NULL),
  type(pIndetType),
  prefetchEnabledTransitions(preEnabTrans),
  findErrors(errors),
  maxNumOfErrors(numErrors),
  numOfErrors(0),
  reportDeadCode(deadCode),
  executedTransitions(0),
  hasStopTime(theStopTime != 0.0),
  stopTime(theStopTime),
  numOfDeadlocks(0),
  numOfSignalDrops(0),
  numOfSignalLosses(0),
  numOfFailedProcessCreations(0),
  numOfMissingReceivers(0),
  numOfFailedAssertions(0),
  numOfCycleDetections(0),
  errorDumpActive(false),
  stopCondition(stopAtom)
{
  realStartTime = std::chrono::high_resolution_clock::now(); // remember start time for run time calculation is Display method

  if (findErrors && maxNumOfErrors == 0)
    maxNumOfErrors = UINT_MAX;
}


SCIndet::~SCIndet (void)
{
}


SCActiveRunnableList * SCIndet::ActiveRunnables(SCTimeEventSaveList *activeQueue) const
{
  SCTimeEventIter           iter(*activeQueue);
  SCTimeEvent *             timeEvent;
  SCTime                    currentTime;
  SCEnabledTransitionList * enabledTransitionList;
  SCActiveRunnable *        activeRunnable;
  SCActiveRunnableList *    activeRunnableList;
  SCAutomaton *             automaton;

  timeEvent = iter++;                               // timeEvent holds now first
                                                    // element of activeQueue
  assert(timeEvent);

  currentTime = timeEvent->GetTime();               // currentTime = time of
                                                    // first timeEvent in activeQueue
  assert(currentTime >= SCScheduler::GetCurrentTime());

  SCScheduler::SetCurrentTime(currentTime);         // adjust scheduler time if
                                                    // necessary
  activeRunnableList = new SCActiveRunnableList(true, NULL);
  assert(activeRunnableList);

  // Search all runnables with minimal wakeup time and store
  // them in activeRunnableList:
  while (timeEvent &&                               // events left?
         timeEvent->GetTime() == currentTime)       // event time minimal?
  {
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCIndet::ActiveRunnables(): checking " << *timeEvent << std::endl;
#endif

    if (timeEvent->IsAutomaton())                   // runnable is a process
    {                                               // or procedure?
      automaton = (SCAutomaton *)timeEvent->GetRunnable();

      assert(automaton);

      if (automaton->IsInState() && prefetchEnabledTransitions)
      {                                             // process executing State() method?
        enabledTransitionList = automaton->RetrieveEnabledTransitions();
                                                    // retrieve or calculate set of
                                                    // enabled transitions
        assert (enabledTransitionList);             // if process is in activeQueue
                                                    // it MUST have enabled transitions

#if _SC_VALIDATION_DEBUG
        scValidationDebugLog << "SCIndet::ActiveRunnables(): adding ";
        scValidationDebugLog << *automaton << " with ";
        scValidationDebugLog << enabledTransitionList->NumOfElems();
        scValidationDebugLog << " enabled transitions";
#endif

#if _SC_SIMULATION_DEBUG
        scSimulationDebugLog << "SCIndet::ActiveRunnables(): adding ";
        scSimulationDebugLog << *automaton << std::endl;
#endif
      }
      else                                          // process executing Request method!
      {
        enabledTransitionList = NULL;               // empty transition list marks
                                                    // process in Request method!

#if _SC_VALIDATION_DEBUG
        scValidationDebugLog << "SCIndet::ActiveRunnables(): ";
        scValidationDebugLog << "adding requesting or calling ";
        scValidationDebugLog << *automaton;
#endif

#if _SC_SIMULATION_DEBUG
        scSimulationDebugLog << "SCIndet::ActiveRunnables(): ";
        scSimulationDebugLog << "adding ";
        scSimulationDebugLog << *automaton << "." << std::endl;
#endif
      }
    }
    else                                            // runnable is machine, timer or path!
    {
#if _SC_VALIDATION_DEBUG || _SC_SIMULATION_DEBUG
      SCRunnable * runnable = timeEvent->GetRunnable();

      assert(runnable);
#endif

#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << "SCIndet::ActiveRunnables(): adding ";
      scValidationDebugLog << *runnable;
#endif

#if _SC_SIMULATION_DEBUG
      scSimulationDebugLog << "SCIndet::ActiveRunnables(): adding ";
      scSimulationDebugLog << *runnable << "." << endl;
#endif

      enabledTransitionList = NULL;
    }
#if _SC_VALIDATION_DEBUG
    if (type == scIndetVerification)
      scValidationDebugLog << " (" << NumOfSuccessors() << " buechi successors)." << std::endl;
    else
      scValidationDebugLog << "." << std::endl;
#endif
    activeRunnable = new SCActiveRunnable(timeEvent,
                                          enabledTransitionList,
                                          NumOfSuccessors());
                                                    // an active runnable consists
                                                    // of a runnable and a set of
    assert(activeRunnable);                         // enabled transitions
    activeRunnableList->InsertAfter(activeRunnable);
                                                    // insert active runnable in
                                                    // active runnable list
    timeEvent = iter++;
  } // while

  return activeRunnableList;
}


SCStream& operator<< (SCStream& pStream, const SCIndet& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCIndet::Display(SCStream& pStream) const
{
  SCProcessTypeList * processTypeList = SCProcessType::GetProcessTypeList();
  SCTransition *      transition;
  SCProcessType *     processType;
  SCStateType *       stateType;

  if (executedTransitions == 0)             // Called for start message?
  {
    if (hasStopTime)
    {
      pStream << "Max. model time:            " << stopTime << std::endl;
    }
    pStream << "Error detection:            ";
    if (findErrors)
    {
      pStream << "activated";
      if (maxNumOfErrors < UINT_MAX)
      {
        pStream << " (limited to " << maxNumOfErrors << " errors)";
      }
      pStream << std::endl;
    }
    else
      pStream << "not activated" << std::endl;

    return pStream;
  }

  realEndTime = std::chrono::high_resolution_clock::now();
  realExecTime = std::chrono::duration_cast<milliseconds>( realEndTime - realStartTime ).count();

  pStream << "real exec time: " << realExecTime << " ms" << std::endl;

  pStream << "Executed transitions:     " << executedTransitions << std::endl;

  if (realExecTime > 0)
  {
    pStream << "Transitions per second:   " << (float)executedTransitions / ((float)realExecTime / 1000.0) << std::endl;
  }

  if (maxNumOfErrors)
  {
    pStream << std::endl << "Error report:" << std::endl;

    if (numOfErrors > 0)
    {
      pStream << "  Deadlocks:                    " << numOfDeadlocks << std::endl;
      pStream << "  Implicit signal consumptions: " << numOfSignalDrops << std::endl;
      pStream << "  Overflows of input queues:    " << numOfSignalLosses << std::endl;
      pStream << "  Derouted outputs:             " << numOfMissingReceivers << std::endl;
      pStream << "  Failed process creations:     " << numOfFailedProcessCreations << std::endl;
      pStream << "  Violated assertions:          " << numOfFailedAssertions << std::endl;
      pStream << "  Detected cycles:              " << numOfCycleDetections << std::endl << std::endl;
    }
    else
    {
      pStream << "  No errors found." << std::endl;
    }
  }

  assert (processTypeList);

  if (reportDeadCode)
  {
    SCProcessTypeIter piter(*processTypeList);
    SCBoolean foundDeadCode = false;

    pStream << std::endl << "Dead code report:" << std::endl;

    for (processType = piter++;
         processType;
         processType = piter++)
    {
      SCStateTypeIter siter(*processType->GetStateList());

      for (stateType = siter++;
           stateType;
           stateType = siter++)
      {
        SCTransitionIter titer(*stateType->GetNormalInputs());

        if (!stateType->IsReached())
        {
          pStream << "  Process " << *processType << ", " << *stateType;
          pStream << " not reached." << std::endl;
          foundDeadCode = true;
          continue;
        }

        for (transition = titer++;
             transition;
             transition = titer++)
        {
          if (!transition->IsExecuted())
          {
            pStream << "  Process " << *processType << ", " << *stateType;
            pStream << " " << *transition << " not executed." << std::endl;
            foundDeadCode = true;
          }
        }
      }
    }
    if (!foundDeadCode)
      pStream << "  No dead code found." << std::endl;
    pStream << std::endl;
  }
  
  return pStream;
}
