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
| SCTraceSocket |  SCTraceSocket.h  |  3. Sep 1996  |   SCL             |       |
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

#ifndef __SCTRACESOCKET__
#define __SCTRACESOCKET__


#include "SCTrace.h"
#include "SCSocket.h"

class SCTraceSocket : public SCTrace, public SCSocket
{
  public:
                    // Konstruktor etc.
                        SCTraceSocket (SCInteger pActionFlags,
                                       const char * pHostName,
                                       const SCNatural pPortNumber);
                       
                       ~SCTraceSocket (void);

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
};


#endif  //  __SCTRACESOCKET__
