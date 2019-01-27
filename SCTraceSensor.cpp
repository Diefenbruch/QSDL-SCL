/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCTraceSensor | SCTraceSensor.cc  |  3. Sep 1996  |   SCL             |      |
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

#include <string.h>
#include "SCTraceSensor.h"

#include "SCAutomaton.h"
#include "SCMachine.h"
#include "SCProcess.h"
#include "SCProcedure.h"
#include "SCSignal.h"
#include "SCRequest.h"
#include "SCTimer.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCTraceSensor::SCTraceSensor (SCInteger pActionFlags) :
  SCTrace (pActionFlags)
{
}


SCTraceSensor::~SCTraceSensor (void)
{
}


//
// Parameterless Actions (Scheduler start, stop, Deadlock)
//

void SCTraceSensor::LogEvent (const SCInteger pAction)
{
  (void)pAction;
}


//
// Time change
//

void SCTraceSensor::LogEvent (const SCInteger pAction, const SCTime newTime)
{
  (void)pAction; (void)newTime;
}


//
// Machine create & delete
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCMachine *machine)
{
  switch (pAction)
  {
    case scTraceMachineCreate:
    case scTraceMachineStop:
      machine->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Process stop
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process)
{
  switch (pAction)
  {
    case scTraceProcessStop:
      info.theActiveAutomaton = process;
      process->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Process create
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              SCProcess *creator)
{
  switch (pAction)
  {
    case scTraceProcessCreate:
      info.theActiveAutomaton = creator;
      info.thePassiveAutomaton = process;
      if (process) // NULL if creation failed!
        process->SensorUpdate (pAction, scTSRoleOffspring, info);
      if (creator)
        creator->SensorUpdate (pAction, scTSRoleParent, info);
      break;

    default:
      break;
  }
}


//
// Procedure return
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcedure *procedure)
{
  switch (pAction)
  {
    case scTraceProcedureReturn:
      info.theActiveAutomaton = procedure;
      procedure->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Procedure call
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcedure *procedure,
                              SCAutomaton *caller)
{
  switch (pAction)
  {
    case scTraceProcedureCall:
      info.theActiveAutomaton = caller;
      info.thePassiveAutomaton = procedure;
      procedure->SensorUpdate (pAction, scTSRoleOffspring, info);
      caller->SensorUpdate (pAction, scTSRoleParent, info);
      break;

    default:
      break;
  }
}


//
// Request issue, start, stop, finish
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCMachine *machine,
                              const SCRequest *request)
{
  SCProcess *caller;

  switch (pAction)
  {
    case scTraceServiceRequest:
    case scTraceServiceFinish:
      info.theRequest = request;
      machine->SensorUpdate (pAction, scTSRoleNone, info);
      caller = request->GetCaller()->GetOwner();
      if (caller)
        caller->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    case scTraceServiceStart:
    case scTraceServiceInterrupt:
      info.theRequest = request;
      machine->SensorUpdate (pAction, scTSRoleNone, info);
      break;      

    default:
      break;
  }
}


//
// Signal send
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *sender,
                              SCProcess *receiver,
                              const SCSignal *signal,
                              const SCDuration delay)
{
  (void)delay;

  switch (pAction)
  {
    case scTraceSignalSend:
      info.theActiveAutomaton = sender;
      info.thePassiveAutomaton = receiver;
      info.theSignal = signal;
      info.theTimer = NULL;
      sender->SensorUpdate (pAction, scTSRoleSender, info);
      receiver->SensorUpdate (pAction, scTSRoleReceiver, info);
      break;

    default:
      break;
  }
}


//
// Signal not sent
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *sender,
                              const SCSignalType *signalType)
{
  switch (pAction)
  {
    case scTraceSignalNotSent:
      info.theActiveAutomaton = sender;
      info.thePassiveAutomaton = NULL;
      info.theSignalType = signalType;
      sender->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Signal receive, save, drop, reject
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCSignal *signal)
{
  SCProcess *sender;

  switch (pAction)
  {
    case scTraceSignalReceive:
    case scTraceSignalSave:
    case scTraceSignalDrop:
    case scTraceSignalLose:
    case scTraceTimerRemove:
      sender = signal->GetSender();
      info.theActiveAutomaton = sender;
      info.thePassiveAutomaton = process;
      info.theSignal = signal;
      process->SensorUpdate (pAction, scTSRoleReceiver, info);
      if (info.theActiveAutomaton && info.theActiveAutomaton != process)
        sender->SensorUpdate (pAction, scTSRoleSender, info);
      break;

    default:
      break;
  }
}


//
// Signal consume
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCSignal *signal,
                              const SCTransition *transition)
{
  SCProcess *sender;

  switch (pAction)
  {
    case scTraceSignalConsume:
      sender = signal->GetSender();
      info.theActiveAutomaton = sender;
      info.thePassiveAutomaton = process;
      info.theSignal = signal;
      info.theTransition = transition;
      info.theState = process->GetLastState(); // important for WaitTimeState!
      process->SensorUpdate (pAction, scTSRoleReceiver, info);
      if (info.theActiveAutomaton && info.theActiveAutomaton != process)
        sender->SensorUpdate (pAction, scTSRoleSender, info);
      break;

    default:
      break;
  }
}


//
// Spontaneous Transition, Continuous Signal
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTransition *transition)
{
  switch (pAction)
  {
    case scTraceContSignal:
    case scTraceSpontTrans:
      info.theActiveAutomaton = process;
      info.theTransition = transition;
      info.theState = process->GetLastState(); // important for WaitTimeState!
      process->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Timer set & reset
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTimer *timer)
{
  switch (pAction)
  {
    case scTraceTimerSet:
    case scTraceTimerReset:
      info.theSignal = NULL;
      info.theActiveAutomaton =
      info.thePassiveAutomaton = process;
      info.theTimer = timer;
      process->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// Timer fire
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTimer *timer,
                              const SCSignal *signal)
{
  switch (pAction)
  {
    case scTraceTimerFire:
      info.theActiveAutomaton =
      info.thePassiveAutomaton = process;
      info.theSignal = signal;
      info.theTimer = timer;
      process->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}


//
// State change
//

void SCTraceSensor::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const class SCStateType *newState,
                              const SCDuration awakeDelay)
{
  (void)awakeDelay;
  
  switch (pAction)
  {
    case scTraceStateChange:
      info.theState = newState;
      process->SensorUpdate (pAction, scTSRoleNone, info);
      break;

    default:
      break;
  }
}

