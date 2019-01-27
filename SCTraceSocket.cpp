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
| SCTraceSocket | SCTraceSocket.cc  |  3. Sep 1996  |   SCL             |      |
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include "SCTraceSocket.h"

#include "SCAutomaton.h"
#include "SCMachine.h"
#include "SCProcess.h"
#include "SCProcedure.h"
#include "SCSignal.h"
#include "SCSignalType.h"
#include "SCRequest.h"
#include "SCTimer.h"
#include "SCTransition.h"
#include "SCStateType.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCTraceSocket::SCTraceSocket (SCInteger pActionFlags,
                              const char *pHostName,
                              const SCNatural pPortNumber) :
  SCTrace (pActionFlags),
  SCSocket (pHostName, pPortNumber)
{
  Open();
}


SCTraceSocket::~SCTraceSocket (void)
{
  if (IsOpen())
    Close();
}


//
// Parameterless Actions (Scheduler start, stop, Deadlock)
//

void SCTraceSocket::LogEvent (const SCInteger pAction)
{
  SCSocket::Display(pAction);
}


//
// Time change
//

void SCTraceSocket::LogEvent (const SCInteger pAction, const SCTime newTime)
{
  SCSocket::Display(pAction);
  SCSocket::Display(newTime);
}


//
// Machine create & delete
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCMachine *machine)
{
  SCSocket::Display(pAction);
  SCSocket::Display(machine->GetID());
}


//
// Process delete
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
}


//
// Process create
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              SCProcess *creator)
{
  SCSocket::Display(pAction);
  
  if (process)
  {
    SCSocket::Display(process->GetID());
  }
  else
  {
    SCSocket::Display((SCInteger)kSCNoProcessID);  // creation failed !
  }

  if (creator)
  {
    SCSocket::Display(creator->GetID());
  }
  else
  {
    SCSocket::Display((SCInteger)kSCNoProcessID);
  }
}


//
// Procedure return
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcedure *procedure)
{
  SCSocket::Display(pAction);
  SCSocket::Display(procedure->GetID());
}


//
// Procedure call
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcedure *procedure,
                              SCAutomaton *caller)
{
  SCSocket::Display(pAction);
  SCSocket::Display(procedure->GetID());
  SCSocket::Display(caller->GetID());
}


//
// Request issue, start, stop, finish
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCMachine *machine,
                              const SCRequest *request)
{
  SCSocket::Display(pAction);
  SCSocket::Display(machine->GetID());
  SCSocket::Display(request->GetRequestID());
}


//
// Signal send
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *sender,
                              SCProcess *receiver,
                              const SCSignal *signal,
                              const SCDuration delay)
{
  (void)delay;

  SCSocket::Display(pAction);
  SCSocket::Display(sender->GetID());
  SCSocket::Display(receiver->GetID());
  SCSocket::Display(signal->GetSignalID());
}


//
// Signal not sent
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *sender,
                              const SCSignalType *signalType)
{
  SCSocket::Display(pAction);
  SCSocket::Display(sender->GetID());
  SCSocket::Display(signalType->GetID());
}


//
// Signal receive, save, drop, reject, Timer signal remove
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCSignal *signal)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(signal->GetSignalID());
}


//
// Signal consume
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCSignal *signal,
                              const SCTransition *transition)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(signal->GetSignalID());
  SCSocket::Display(transition->GetID());
}


//
// Spontaneous Transition, Continuous Signal
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTransition *transition)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(transition->GetID());
}


//
// Timer set & reset
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTimer *timer)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(timer->GetTimerID());
}


//
// Timer fire
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const SCTimer *timer,
                              const SCSignal *signal)
{
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(timer->GetTimerID());
  SCSocket::Display(signal->GetSignalID());
}


//
// State change
//

void SCTraceSocket::LogEvent (const SCInteger pAction,
                              SCProcess *process,
                              const class SCStateType *newState,
                              const SCDuration awakeDelay)
{
  (void)awakeDelay;
  
  SCSocket::Display(pAction);
  SCSocket::Display(process->GetID());
  SCSocket::Display(newState->GetID());
}

