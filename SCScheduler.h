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
|  SCScheduler  |  SCScheduler.h    | 12. Jul 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     05.07.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCSCHEDULER__

#define __SCSCHEDULER__

#include <signal.h>

#include "SCBasicTypes.h"
#include "SCSchedulerTypes.h"

class SCScheduler : public SCSystemScheduler
{
  public:
                                  SCScheduler (class SCIndet *indetType,
                                               SCBoolean fairness = false);
                                 ~SCScheduler (void);

    static SCBoolean              Initialize (class SCIndet *pIndetType,
                                              class SCSocket *pSocket,
                                              SCBoolean pFairness = false,
                                              SCBoolean pIsVerbose = false,
                                              SCNatural tableSize = kSCDefaultTableSize,
                                              SCReal pDisplayInterval = 10.0);
    static void                   Finish (void);

    static SCProcessID            Create (class SCProcessType * procType,
                                          class SCProcess * creator = NULL,
                                          class SCDataType * actualParams = NULL);
    static SCProcessID            Call (class SCProcedure * newProcedure,
                                        class SCAutomaton * caller);
    static SCRunnableID           NewProcessID (void);               // Freie Prozess-ID holen

    static void                   Schedule (class SCRunnable *runnable); // Runnable starten
    static void                   Run (void);                        // Scheduler ausfuehren
    static void                   Stop (void);                       // Scheduler stoppen
    static void                   Shutdown (int signalID = SIGINT);  // Scheduler beenden
    static void                   Status (int signalID = SIGINT);    // Scheduler Status

    static SCTime                 GetCurrentTime (void);             // get current model time
    static void                   SetCurrentTime (SCTime newTime);   // set current model time
    static SCDuration             GetEpsilon (void);                 // get current epsilon value

    static const SCRunnable *     GetRunnableFromID (SCRunnableID id);

    static class SCIndet *        GetIndet (void);
    static void                   SetIndet (class SCIndet *newIndet);
    static class SCSocket *       GetSocket (void);
    static void                   SetSocket (class SCSocket *newSocket);

    static SCBoolean              IsVerbose (void) { return verbose; }
    static void                   SetIsVerbose (SCBoolean value) { verbose = value; }
    static SCProgress             GetProgress (void) { return progress; }
    static SCBoolean              IsShuttingDown (void) { return shutdown; }
    
    friend SCStream &              operator<< (SCStream &pStream,
                                              const SCScheduler &pData);
    virtual SCStream &             Display (SCStream& pStream) const;

    static SCBoolean              StaticSave(SCMem &);
    static SCBoolean              StaticRestore(SCMem &);
    static void                   StaticSize(SCSize *pSize);

    static SCScheduler*           GetScheduler (void);
    static void                   SetScheduler (SCScheduler* newScheduler);

  private:
    void                          Body (void);

    static SCScheduler *          scheduler;
    static class SCTimeEventSaveList *activeQueue; 
    static class SCRunnable *     current;
    static class SCIndet *        indet;
    static class SCSocket *       socket;    // communication socket for visualization
                                             // and sensor communication
    static SCTime                 schedulerTime;
    static SCRunnableID           nextRunnableID;

    static SCBoolean              shutdown;  // flag indicates if scheduler
                                             // should schutdown next time
                                             // when scheduler thread is active
                                             // used bye PEV and for SIGINT
                                             // handling!
    static SCBoolean              verbose;
    static SCProgress             progress;

  public:
    static SCStream               errorStream;
    static SCStream               outputStream;
};


#if !_SC_NOINLINES
  #include "SCScheduler.inl.h"
#endif


#endif  //  __SCSCHEDULER__
