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
|    SCIndet    |    SCIndet.h      | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCINDET__

#define __SCINDET__


enum SCErrorCode
{
  scErrorNoError = 0,
  scErrorDeadlock,
  scErrorSignalDrop,
  scErrorSignalLose,
  scErrorProcessCreationFailed,
  scErrorNoSignalReceiver,
  scErrorAssertionFailed,
  scErrorCycleDetected,
  scErrorMax
};

typedef enum
{
  scIndetSimulation,
  scIndetValidation,
  scIndetVerification,
  scIndetStackTrace
} SCIndetType;


#if _SC_PLATFORM_UNIX
  #include <sys/times.h>
#else
  #include <time.h>
#endif

#include <chrono>

#include "SCBasicTypes.h"
#include "SCActiveRunnableList.h"
#include "SCTimeEvent.h"  // needed for template instantiation in generated
                          // code!
#include "SCTimeEventList.h"

class SCIndet: public SCObject
{
  public:
                               SCIndet (SCIndetType pIndetType,
                                        SCTime stopTime = 0.0,
                                        SCBoolean preEnabTrans = false,
                                        SCBoolean errors = false,
                                        SCNatural numErrors = 0,
                                        SCBoolean deadCode = false,
                                        SCName2PropFuncPtr * stopAtom = NULL);
    virtual                   ~SCIndet (void);

    virtual SCNatural          NonDetChoice (const SCNatural range) = 0;
    virtual SCNatural          ChooseOneTrue (const SCNatural choiceSize,
                                              const SCNatural numOfTrues,
                                              const SCNatural singleTrue,
                                              const SCBoolean *choiceSet) = 0;
    virtual class SCEnabledTransition *ChooseTransition (void) = 0;
    virtual SCRunnable *       ChooseRunnable (SCTimeEventSaveList *activeQueue) = 0;
		virtual SCBoolean          Create (SCProcessID offspring) = 0;
    virtual void               StartMessage (void) = 0;
    virtual void               EndMessage (void) = 0;

    // The following three methods classify the reached system
    // state:

    virtual SCErrorCode        IsErrorState (SCTimeEventSaveList *activeQueue) = 0;
    virtual SCBoolean          IsLimitingState (SCTimeEventSaveList *activeQueue) = 0;
    virtual SCNatural          NumOfSuccessors (void) const {return 1; } // important for TL

    virtual SCBoolean          LogError (SCErrorCode errorCode) = 0; // deadlock
    virtual SCBoolean          LogError (SCErrorCode errorCode,      // signal drop or
                                         const class SCProcess *process,   // signal rejected
                                         const class SCSignal *signal) = 0;
    virtual SCBoolean          LogError (SCErrorCode errorCode,      // no receiver
                                         const class SCProcess *process,
                                         const class SCSignalType *signal) = 0;
    virtual SCBoolean          LogError (SCErrorCode errorCode,      // process creation
                                         const class SCProcess *parent,    // failed
                                         const class SCProcessType *offspringType) = 0;

    SCBoolean                  FindErrors (void) const { return findErrors; }
    SCNatural                  MaxNumOfErrors (void) const { return maxNumOfErrors; }
    SCNatural                  NumOfErrors (void) const { return numOfErrors; }
    SCBoolean                  ReportDeadCode (void) const { return reportDeadCode; }
    virtual SCNatural          CurrentDepth (void) const { return executedTransitions; }

    friend SCStream&           operator<< (SCStream& pStream,
                                           const SCIndet& pData);
    SCStream &                 Display (SCStream& pStream) const;

  protected:
    // very important method for
    // determination of activated runnables:
    SCActiveRunnableList *     ActiveRunnables (SCTimeEventSaveList *activeQueue) const;

    SCIndetType                type;

    SCBoolean                  prefetchEnabledTransitions;
                                                    // should ActiveRunnables()
                                                    // determine enabled transition
                                                    // simulation: false
                                                    // validation: true
    SCBoolean                  findErrors;          // search for functional errors?
    SCNatural                  maxNumOfErrors;      // maximal number of errors
                                                    // after evaluation should stop
    SCNatural                  numOfErrors;         // number of errors found so far
    SCBoolean                  reportDeadCode;
    SCNatural                  executedTransitions; // num of transitions
                                                    // executed so far
    SCBoolean                  hasStopTime;         // stop time given?
    SCTime                     stopTime;            // stop time for analyzer

    mutable std::chrono::high_resolution_clock::time_point            realStartTime;       // for calculation
    mutable std::chrono::high_resolution_clock::time_point            realEndTime;         // of run time
    mutable unsigned long   realExecTime;        // and speed values

    SCNatural                  numOfDeadlocks;      // number of deadlocks
    SCNatural                  numOfSignalDrops;    // number of signal drops
    SCNatural                  numOfSignalLosses;   // number of signal losses
    SCNatural                  numOfFailedProcessCreations;
                                                    // number of failed process creations
    SCNatural                  numOfMissingReceivers;
                                                    // number of missing receivers
    SCNatural                  numOfFailedAssertions;
                                                    // number of failed assertions
    SCNatural                  numOfCycleDetections;
                                                    // number of failed temporal
                                                    // assertions
    SCBoolean                  errorDumpActive;

    SCName2PropFuncPtr *       stopCondition;

};

#endif  // __SCINDET__
