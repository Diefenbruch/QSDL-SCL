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
| SCTraceControl| SCTraceControl.h  | 23. Sep 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     23.09.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCTRACECONTROL__

#define __SCTRACECONTROL__


#include "SCBasicTypes.h"
//#include "SCTraceTypes.h"
#include "SCTraceList.h"
#include "SCObject.h"

class SCTraceControl: public SCObject
{
  public:
                        // Zu dieser Klasse gibt es keinen Konstruktor oder
                        // Destruktor

    static void          Initialize (void);
    static void          Finish (void);
    
    static SCTraceList * GetTraceList (void) { return traceList; }
    
    // Traces in den Dispatcher einfuegen oder entfernen
    static void          AddTrace (class SCTrace *pTrace);
    static void          RemoveTrace (class SCTrace *pTrace);

    // Scheduler start, stop
    static void          LogEvent (const SCInteger pAction);

    // Simulation time change
    static void          LogEvent (const SCInteger pAction,
                                   const SCTime newTime);

    // Process delete
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process);

    // Process create
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   class SCProcess *creator);

    // Procedure return
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcedure *procedure);

    // Procedure call
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcedure *procedure,
                                   class SCAutomaton *caller);

    // Machine create & delete
    static void          LogEvent (const SCInteger pAction,
                                   class SCMachine *machine);

    // Request issue, start, stop, finish
    static void          LogEvent (const SCInteger pAction,
                                   class SCMachine *machine,
                                   const class SCRequest *request);

    // Signal send
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *sender,
                                   class SCProcess *receiver,
                                   const class SCSignal *signal,
                                   const SCDuration delay = 0.0);

    // Signal not sent
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *sender,
                                   const class SCSignalType *signalType);

    // Signal receive, save, drop, reject
    // Timer remove
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCSignal *signal);

    // Signal consume
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCSignal *signal,
                                   const class SCTransition *transition);

    // Spontaneous Transition, Continuous Signal
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCTransition *transition);

    // Timer set & reset
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCTimer *timer);

    // Timer fire
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCTimer *timer,
                                   const class SCSignal *signal);

    // State change
    static void          LogEvent (const SCInteger pAction,
                                   class SCProcess *process,
                                   const class SCStateType *newState,
                                   const SCDuration awakeDelay = kSCNoAwakeDelay);

  private:
    static SCTraceList * traceList;
};


#endif  //  __SCTRACECONTROL__
