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
|   SCTrace     |   SCTrace.h       | 23. Sep 1994  |   SCL             |      |
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

#ifndef __SCTRACEOBJ__

#define __SCTRACEOBJ__


#include "SCBasicTypes.h"
#include "SCTraceTypes.h"
#include "SCObject.h"
#include "SCTraceList.h"

class SCTrace: public SCObject
{
  public:
                        // Konstruktor & Destruktor
                        SCTrace (SCInteger pActionFlags = scfTraceAll,
                                 const SCObject* pParent = NULL);
    virtual            ~SCTrace (void);

    SCInteger           GetActionFlags (void) const;
    
    SCTraceCons *       GetContainer (void) const;
    void                SetContainer (SCTraceCons *cont);

    // Scheduler start, stop
    virtual void        LogEvent (const SCInteger pAction) = 0;

    // Simulation time change
    virtual void        LogEvent (const SCInteger pAction,
                                  const SCTime newTime) = 0;

    // Process delete
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process) = 0;

    // Process create
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *offspring,
                                  class SCProcess *parent) = 0;

    // Procedure return
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcedure *procedure) = 0;

    // Procedure call
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcedure *procedure,
                                  class SCAutomaton *caller) = 0;

    // Machine create & delete
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCMachine *machine) = 0;

    // Request issue, start, stop, finish
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCMachine *machine,
                                  const class SCRequest *request) = 0;

    // Signal send
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *sender,
                                  class SCProcess *receiver,
                                  const class SCSignal *signal,
                                  const SCDuration delay = 0.0) = 0;

    // Signal not sent
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *sender,
                                  const class SCSignalType *signalType) = 0;

    // Signal receive, save, drop, reject
    // Timer remove
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCSignal *signal) = 0;

    // Signal consume
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCSignal *signal,
                                  const class SCTransition *transition) = 0;

    // Spontaneous Transition, Continuous Signal
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTransition *transition) = 0;

    // Timer set & reset
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTimer *timer) = 0;

    // Timer fire
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCTimer *timer,
                                  const class SCSignal *signal) = 0;

    // State change
    virtual void        LogEvent (const SCInteger pAction,
                                  class SCProcess *process,
                                  const class SCStateType *newState,
                                  const SCDuration awakeDelay = kSCNoAwakeDelay) = 0;

    virtual SCStream &  Display (SCStream &pStream) const { return pStream; }

  protected:
    SCInteger           actionFlags;
    SCTraceCons *       container;              // actual SCListCons elem
                                                // in which trace is
                                                // embedded (do not save!)
};


#if !_SC_NOINLINES
  #include "SCTrace.inl.h"
#endif


#endif  //  __SCTRACEOBJ__
