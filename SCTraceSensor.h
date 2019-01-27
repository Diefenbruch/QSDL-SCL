/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCTraceSensor |  SCTraceSensor.h  |  3. Sep 1996  |   SCL             |       |
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

#ifndef __SCTRACESENSOR__
#define __SCTRACESENSOR__


#include "SCTrace.h"


struct SCTraceSensorUpdate
{
  const class SCAutomaton *  theActiveAutomaton;  // sender or parent or stopped
  const class SCAutomaton *  thePassiveAutomaton; // receiver or offspring
  const class SCMachine *    theMachine;
  const class SCStateType *  theState;
  const class SCSignal *    theSignal;
  const class SCRequest *    theRequest;
  const class SCTimer *      theTimer;
  const class SCTransition * theTransition;
  const class SCSignalType * theSignalType;
  const class SCProcessType *thePassiveProcType;    // delayed output to process type
};

// Prototyp fuer SensorUpdate (in SCProcess und SCMachine)
// virtual void SensorUpdate (const SCInteger pAction,
//                            const SCTraceSensorRole pRole,
//                            const struct SCTraceSensorUpdate &pInfo);


// some events (e.g. sending of a signal) are reported
// to more than one process/machine. So the process/machine
// must know, which role it plays in this event. We define
// some constants to define different roles:

enum SCTraceSensorRole
{
  scTSRoleNone = 0,
  scTSRoleSender,
  scTSRoleReceiver,
  scTSRoleParent,
  scTSRoleOffspring
};

class SCTraceSensor : public SCTrace
{
  public:
                    // Konstruktor etc.
                        SCTraceSensor (SCInteger pActionFlags = scfTraceAll);
                       ~SCTraceSensor (void);


    // Scheduler start, stop
    virtual void        LogEvent (const SCInteger pAction);

    // Simulation time change
    virtual void        LogEvent (const SCInteger pAction,
                                  const SCTime newTime);

    // Process delete
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process);

    // Process create
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *offspring,
                                  class SCProcess *parent);

    // Procedure return
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcedure *procedure);

    // Procedure call
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcedure *procedure,
                                  class SCAutomaton *caller);

    // Machine create & delete
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCMachine *machine);

    // Request issue, start, stop, finish
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCMachine *machine,
                                  const class SCRequest *request);

    // Signal send
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *sender,
                                  class SCProcess *receiver,
                                  const class SCSignal *signal,
                                  const SCDuration delay = 0.0);

    // Signal not sent
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *sender,
                                  const class SCSignalType *signalType);

    // Signal receive, save, drop, reject
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCSignal *signal);

    // Signal consume
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCSignal *signal,
                                  const class SCTransition *transition);

    // Spontaneous Transition, Continuous Signal
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTransition *transition);

    // Timer set & reset
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTimer *timer);

    // Timer fire
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTimer *timer,
                                  const class SCSignal *signal);

    // State change
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCStateType *newState,
                                  const SCDuration awakeDelay = kSCNoAwakeDelay);

  private:
    struct SCTraceSensorUpdate info;
};


#endif  //  __SCTRACESENSOR__
