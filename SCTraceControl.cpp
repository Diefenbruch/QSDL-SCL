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
| SCTraceControl| SCTraceControl.cc | 23. Sep 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     23.09.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCTraceControl.h"
#include "SCTrace.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCTraceList * SCTraceControl::traceList = NULL;


void SCTraceControl::Initialize (void)
{
  traceList = new SCTraceList(true);
  assert(traceList);
}


void SCTraceControl::Finish (void)
{
  if (traceList)
    delete traceList;
}


void SCTraceControl::AddTrace (SCTrace *pTrace)
{
  SCTraceCons *cons;
  
  assert(traceList);
  assert(pTrace);

  cons = traceList->InsertAfter(pTrace);
  assert(cons);
  
  pTrace->SetContainer(cons);
}


void SCTraceControl::RemoveTrace (SCTrace *pTrace)
{
  assert(traceList);
  assert(pTrace);
  assert(pTrace->GetContainer());
  
  traceList->Remove(pTrace->GetContainer());  
}


/* LogEvent-Dispatcher */

// Scheduler init, Scheduler stop, End, Deadlock
void SCTraceControl::LogEvent (const SCInteger pAction)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction);
  }
}


// Simulation-Time change
void SCTraceControl::LogEvent (const SCInteger pAction,
                               const SCTime newTime)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, newTime);
  }
}


// Machine create & delete
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCMachine *machine)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, machine);
  }
}


// Process delete
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process);
  }
}


// Process create
void SCTraceControl::LogEvent (const SCInteger pAction,
  														 SCProcess *process,
  														 SCProcess *creator)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, creator);
  }
}


// Procedure return
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcedure *procedure)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, procedure);
  }
}


// Procedure call
void SCTraceControl::LogEvent (const SCInteger pAction,
  														 SCProcedure *procedure,
  														 SCAutomaton *caller)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, procedure, caller);
  }
}


// Request issue, start, stop, finish
void SCTraceControl::LogEvent (const SCInteger pAction,
  														 SCMachine *machine,
  														 const SCRequest *request)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, machine, request);
  }
}


// Signal send
void SCTraceControl::LogEvent (const SCInteger pAction,
  														 SCProcess *sender,
  														 SCProcess *receiver,
  														 const SCSignal *signal,
                               const SCDuration delay)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, sender, receiver, signal, delay);
  }
}


// Signal not sent
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *sender,
                               const SCSignalType *signalType)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, sender, signalType);
  }
}


// Signal receive, save, drop, reject
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process,
                               const SCSignal *signal)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, signal);
  }
}


// Signal consume
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process,
                               const SCSignal *signal,
                               const SCTransition *transition)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, signal, transition);
  }
}


// Spontaneous Transition, Continuous Signal
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process,
                               const SCTransition *transition)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, transition);
  }
}


// Timer set & reset
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process,
                               const SCTimer *timer)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, timer);
  }
}


// Timer fire
void SCTraceControl::LogEvent (const SCInteger pAction,
                               SCProcess *process,
                               const SCTimer *timer,
                               const SCSignal *signal)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, timer, signal);
  }
}


// State change
void SCTraceControl::LogEvent (const SCInteger pAction,
  														 SCProcess *process,
  														 const class SCStateType *newState,
  														 const SCDuration awakeDelay)
{
  SCTraceIter  iter(*traceList);
  SCTrace * trace;
  
  for (trace = iter++;
       trace;
       trace = iter++)
  {
    if (trace->GetActionFlags() & pAction)
      trace->LogEvent (pAction, process, newState, awakeDelay);
  }
}
