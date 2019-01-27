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
|SCTraceEventMSC|SCTraceEventMSC.cc | 21. Mai 1995  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     21.05.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#define _SYNC_OUTPUT

#include <string.h>

#include "SCStream.h"
#include "SCTraceEventMSC.h"

#include "SCDataType.h"
#include "SCSignal.h"
#include "SCMachine.h"
#include "SCProcess.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCSignalType.h"
#include "SCStateType.h"
#include "SCRequest.h"
#include "SCRequestType.h"
#include "SCTimer.h"
#include "SCTimerType.h"


#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#define MSC_END ";" << std::endl

#ifdef  _SYNC_OUTPUT
  #define SYNC    out->GetStream().flush()
#else
  #define SYNC
#endif

SCTraceEventMSC::SCTraceEventMSC (SCStream *pOut,
                                  SCInteger pActionFlags,
                                  SCBoolean pWithData,
                                  const char *pMscName) :
  SCTrace (pActionFlags),
  out(pOut),
  printData(pWithData)
{
  if (pMscName)
  {
    mscName = new char[strlen(pMscName) + 1];
    assert (mscName);
    strcpy (mscName, pMscName);
  }
  else
    mscName = NULL;
}


SCTraceEventMSC::~SCTraceEventMSC (void)
{
  if (mscName)
    delete[] mscName;

  if (out)
  {
    out->GetStream().flush();
    delete out;
  }
}


void SCTraceEventMSC::Instance (const SCProcessType *processType,
                                const SCProcessID processID)
{
  assert(processType);
  (*out) << processType->GetName() << "_" << processID;
}


void SCTraceEventMSC::Instance (const SCProcess *process)
{
  assert(process);
  (*out) << process->GetType()->GetName() << "_" << process->GetID();
}


void SCTraceEventMSC::Instance (const SCMachine *machine)
{
  assert(machine);
  (*out) << machine->GetName() << "__" << -machine->GetID();
}


void SCTraceEventMSC::SharedAll (const char *conditionName,
                                 const SCTime newTime)
{
  if (SCProcessType::GetProcessTypeList())
  {
    SCProcessTypeIter ptiter(*SCProcessType::GetProcessTypeList());
    SCProcessType *pType;
    
    for (pType = ptiter++;
         pType;
         pType = ptiter++)
    {
      if (pType->GetProcessList())
      {
        SCProcessIter piter(*pType->GetProcessList());
        SCProcess *process;
 
        for (process = piter++;
             process;
             process = piter++)
        {
          Instance(process);
          (*out) << ": condition " << conditionName << " shared all";
          if (newTime)
          {
            (*out) << " comment '" << newTime << "'";
          }
          (*out) << MSC_END;
        }
      }
    }
  }

  if (SCMachine::GetMachineList())
  {
    SCMachineIter miter(*SCMachine::GetMachineList());
    SCMachine *machine;
    
    for (machine = miter++;
         machine;
         machine = miter++)
    {
      Instance(machine);
      (*out) << ": condition " << conditionName << " shared all";
      if (newTime)
      {
        (*out) << " comment '" << newTime << "'";
      }
      (*out) << MSC_END;
    }
  }
}

//
// Scheduler init, Scheduler stop, Deadlock, Zyklus Beginn, Zyklus Ende
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction)
{
  switch (pAction)
  {
    case scTraceSchedulerInit:
      if (mscName)
        (*out) << "msc " << mscName << MSC_END;
      else
        (*out) << "msc QSDL_Trace" << MSC_END;
      break;

    case scTraceSchedulerStop:
      (*out) << "text 'QSDL-Trace\ngenerated by\nSimulation Class Library'";
      (*out)  << MSC_END << "endmsc" << MSC_END;
      break;

    case scTraceDeadlock:
      SharedAll("Deadlock");
      break;

    case scTraceCycleStart:
      SharedAll("CycleStartState");
      break;

    case scTraceCycleEnd:
      SharedAll("CycleJoinState");
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (sched init/stop)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Time change
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                const SCTime newTime)
{
  switch (pAction)
  {
    case scTraceTimeChange:
      SharedAll("TimeChanged", newTime);
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (time chg)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Machine create & delete
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCMachine *machine)
{
  switch (pAction)
  {
    case scTraceMachineCreate:
      Instance (machine);
      (*out) << ": instancehead process " << machine->GetName() << MSC_END;
      break;

    case scTraceMachineStop:
      Instance (machine);
      (*out) << ": stop" << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (mach crea/del)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Process delete
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process)
{
  switch (pAction)
  {
    case scTraceProcessStop:
      Instance (process);
      (*out) << ": stop" << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (proc delete)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Process create
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                SCProcess *creator)
{
  switch (pAction)
  {
    case scTraceProcessCreate:
      if (!process)    // process creation failed ?
        break;
      Instance (process);
      (*out) << ": instancehead process " << process->GetType()->GetName();
      (*out) << MSC_END;
      if (creator)
      {
        Instance (creator);
        (*out) << ": create ";
        Instance (process);
        (*out) << " (";
        if (printData && process->GetParameters())
          process->GetParameters()->Display (*out);
        (*out) << ")" << MSC_END;
      }
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (proc create)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Procedure return
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcedure *procedure)
{
  (void)procedure;
  
  switch (pAction)
  {
    case scTraceProcedureReturn:
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (proc delete)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Procedure call
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcedure *procedure,
                                SCAutomaton *caller)
{
  (void)procedure;
  (void)caller;
  
  switch (pAction)
  {
    case scTraceProcedureCall:
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (proc create)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Service request, start, interrupt, finish
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCMachine *machine,
                                const SCRequest *request)
{
  assert(request);
  assert(request->GetCaller());

  switch (pAction)
  {
    case scTraceServiceRequest:
      Instance (request->GetCaller()->GetOwner());
      (*out) << ": out " << request->GetRequestType()->GetName() << ", req_";
      (*out) << request->GetRequestID() << " (" << request->GetRemainingService() << ", ";
      (*out) << request->GetPriority() << ") to ";
      Instance (machine);
      (*out) << MSC_END;

      Instance (machine);
      (*out) << ": in " << request->GetRequestType()->GetName() << ", req_";
      (*out) << request->GetRequestID() << " (" << request->GetRemainingService() << ", ";
      (*out) << request->GetPriority() << ") from ";
      Instance (request->GetCaller()->GetOwner());
      (*out) << MSC_END;

      break;

    case scTraceServiceStart:
      break;

    case scTraceServiceInterrupt:
      break;

    case scTraceServiceFinish:
      Instance (machine);
      (*out) << ": out " << request->GetRequestType()->GetName() << ", req_";
      (*out) << request->GetRequestID() << " to ";
      Instance (request->GetCaller()->GetOwner());
      (*out) << MSC_END;
      Instance (request->GetCaller()->GetOwner());
      (*out) << ": in " << request->GetRequestType()->GetName() << ", req_";
      (*out) << request->GetRequestID() << " from ";
      Instance (machine);
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (request)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Signal send
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *sender,
                                SCProcess *receiver,
                                const SCSignal *signal,
                                const SCDuration delay)
{
  switch (pAction)
  {
    case scTraceSignalSend:
      Instance (sender);
      (*out) << ": out " << signal->GetSignalType()->GetName() << ", sig_";
      (*out) << signal->GetSignalID() << " (";
      if (printData && signal->GetData())
        signal->GetData()->Display (*out);
      (*out) << ") to ";
      Instance (receiver);
      if (delay)
      {
        (*out) << " comment 'delay: ";
        (*out) << delay << "'";
      }
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (sig send)'" << MSC_END;
     break;
  }
  SYNC;
}


//
// Signal not sent
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *sender,
                                const SCSignalType *signalType)
{
  (void)sender;
  (void)signalType;

  switch (pAction)
  {
    case scTraceSignalNotSent:
      Instance (sender);
      (*out) << ": condition NoSignalReceiver comment '" << *signalType;
      (*out) << "'" << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (sig not sent)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Signal receive, save, drop, reject
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCSignal *signal)
{
  switch (pAction)
  {
    case scTraceSignalReceive:
      break;

    case scTraceSignalSave:
      break;

    case scTraceSignalDrop:
      Instance (process);
      (*out) << ": condition ImplicitSignalConsumption comment '" << *signal;
      (*out) << "'" << MSC_END;
      break;

    case scTraceSignalLose:
      Instance (process);
      (*out) << ": condition InputBufferOverflow" << MSC_END;
      break;

    case scTraceTimerRemove:
      Instance (process);
      (*out) << ": reset " << signal->GetSignalType()->GetName();
      (*out) << ", tim_" << signal->GetTimerID();
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (sig rcv)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Signal consume
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCSignal *signal,
                                const SCTransition *transition)
{
  (void)transition;

  switch (pAction)
  {
    case scTraceSignalConsume:
      Instance (process);
      if (signal->GetTimerID() > 0)
      {
        (*out) << ": timeout " << signal->GetSignalType()->GetName();
        (*out) << ", tim_" << signal->GetTimerID();
      }
      else
      {
        (*out) << ": in " << signal->GetSignalType()->GetName() << ", sig_";
        (*out) << signal->GetSignalID() << " (";
        if (printData && signal->GetData())
          signal->GetData()->Display (*out);
        (*out) << ") from ";
        Instance (signal->GetSenderType(), signal->GetSenderID());
      }
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (consume)'" << MSC_END;
      break;
  }
}


//
// Spontaneous Transition, Continuous Signal
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCTransition *transition)
{
  assert(process);
  
  (void)transition;

  switch (pAction)
  {
    case scTraceSpontTrans:
      assert(process->GetLastState());
      
      if (!process->GetLastState()->IsIntermediate())
      {
        Instance (process);
        (*out) << ": out none, sig_none () to ";
        Instance (process);
        (*out) << MSC_END;
        Instance (process);
        (*out) << ": in none, sig_none () from ";
        Instance (process);
        (*out) << MSC_END;
      }
      break;

    case scTraceContSignal:
      assert(process->GetLastState());
      
      if (!process->GetLastState()->IsIntermediate())
      {
        Instance (process);
        (*out) << ": out cont, sig_cont () to ";
        Instance (process);
        (*out) << MSC_END;
        Instance (process);
        (*out) << ": in cont, sig_cont () from ";
        Instance (process);
        (*out) << MSC_END;
      }
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (spont)'" << MSC_END;
      break;
  }
}


//
// Timer set & reset
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCTimer *timer)
{
  switch (pAction)
  {
    case scTraceTimerSet:
      Instance (process);
      (*out) << ": set " << timer->GetTimerType()->GetName();
      (*out) << ", tim_" << timer->GetTimerID();
      (*out) << " (" << timer->GetTimeout();
      if (printData && timer->GetData())
      {
        (*out) << ", ";
        timer->GetData()->Display (*out);
      }
      (*out) << ")" << MSC_END;
      break;

    case scTraceTimerReset:
      Instance (process);
      (*out) << ": reset " << timer->GetTimerType()->GetName();
      (*out) << ", tim_" << timer->GetTimerID();
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (timer set/rst)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// Timer fire
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCTimer *timer,
                                const SCSignal *signal)
{
  (void)process; (void)timer; (void)signal;

  switch (pAction)
  {
    case scTraceTimerFire:
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (timer fire)'" << MSC_END;
      break;
  }
  SYNC;
}


//
// State change
//

void SCTraceEventMSC::LogEvent (const SCInteger pAction,
                                SCProcess *process,
                                const SCStateType *newState,
                                const SCDuration awakeDelay)
{
  assert(process);
  assert(newState);
  
  switch (pAction)
  {
    case scTraceStateChange:
      Instance (process);
      (*out) << ": condition " << newState->GetName();
      if (awakeDelay != kSCNoAwakeDelay)
      {
        (*out) << " comment 'awake: ";
        (*out) << SCScheduler::GetCurrentTime() + awakeDelay << "'";
      }
      (*out) << MSC_END;
      break;

    default:
      (*out) << "comment 'Error: Derouted Action #" << pAction;
      (*out) << " (state)'" << MSC_END;
      break;
  }
  SYNC;
}

