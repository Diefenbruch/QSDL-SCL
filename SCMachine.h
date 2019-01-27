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
|   SCMachine   |   SCMachine.h     | 5. Jul 1994   |   SCL             |       |
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


#ifndef __SCMACHINE__

#define __SCMACHINE__

#include <float.h>
#include "SCBasicTypes.h"
#include "SCRunnable.h"
#include "SCMachineList.h"
#include "SCRequestList.h"

#include "SCTraceSensor.h"

enum SCDiscipline
{
  scDiscUndefined = 0,
  scDiscPS,               // Process Sharing, cur = max / requests
  scDiscIS,               // Infinite Server, cur = max, 
  scDiscFCFS,             // First Come First Serve
  scDiscFCFSPP,           // First Come First Serve, Priorities, Preemptive
  scDiscFCFSPNP,          // First Come First Serve, Priorities, Non-Preemptive
  scDiscLCFSPP,           // Last Come First Serve, Priorities, Preemptive
  scDiscRandom            // FCFS mit Random Queueing
};


class SCMachine : public SCRunnable
{
  public:
                          SCMachine (SCRunnableID pMachineID,
                                     SCDiscipline pDiscipline,
                                     SCNatural pServers,
                                     SCNatural pServices,
                                     const SCDuration *const pSpeeds,
                                     const char *pName,
                                     SCObject *parent = NULL);
    virtual              ~SCMachine (void);

    virtual void          NewRequest (class SCAutomaton *caller,
                                      const class SCRequestType *serviceType,
                                      SCDuration serviceAmount,
                                      SCNatural priority = 0) = 0;
    virtual SCBoolean     HasWaitingQueue (void) const = 0;
    
    SCDiscipline          GetDiscipline (void) const;
    SCNatural             NumOfServers (void) const;
    SCNatural             NumOfFreeServers (void) const;
    SCNatural             NumOfServices (void) const;
    SCDuration *          GetSpeeds (void) const;
    const char *          GetName (void) const;
    SCTime                GetLastTime (void) const { return lastTime; }
    SCRequestSaveList *   GetServiceQueue (void) const;

    virtual void          SensorUpdate (const SCInteger pAction,
                                        const SCTraceSensorRole pRole,
                                        const struct SCTraceSensorUpdate &pInfo)
                                { (void)pAction; (void)pRole; (void)pInfo; };

       /* static Methods */
    static SCBoolean      Initialize (void);
    static SCBoolean      Finish (void);
    static SCMachineList *GetMachineList (void) { return machineList; }
    static void           SetMachineList (SCMachineList *newMachineList) {machineList = newMachineList; }

    friend SCStream &      operator<< (SCStream& pStream,
                                      const SCMachine& pData);
    virtual SCStream &     Display (SCStream& pStream) const;

    SCBoolean             Save (SCMem &storage) const;
    SCBoolean             Restore (SCMem &storage);
    void                  Size (SCSize *pSize) const;

    static SCBoolean      StaticSave (SCMem &);
    static SCBoolean      StaticRestore (SCMem &);
    static void           StaticSize (SCSize *pSize);

    static void           StaticStop (void);
    static void           StaticSetModified (SCBoolean setQueues = false);
    static void           StaticUnsetModified (SCBoolean setQueues = false);
    static SCMachine *    GetMachineFromName (const char * name);
    
  protected:
    SCDuration            Speed (const SCRequestType *requestType) const;

    void                  Reschedule (void);      // calculate the next time
                                                  // the machine will be
                                                  // activated
    void                  FinishRequests (void);  // remove finished requests
                                                  // from machine and wake up
                                                  // requesting process
    SCTime                lastTime;               // Last time machine has
                                                  // checked its queues
    SCBoolean             working;                // save this in system state!

  private:
    SCNatural             servers;                // Number of servers,
                                                  // don't save, because it
                                                  // never changes!
    SCDiscipline          discipline;             // don't save!
    SCNatural             services;               // Number of services supported.
                                                  // don't save!
    SCDuration *          speeds;                 // Array containing the speeds
                                                  // for each kind of service.
                                                  // don't save!
    SCRequestSaveList *   serviceQueue;           // cointains all requests
                                                  // currently in service
    const char *                name;

    static SCMachineList *machineList;            // list of all machines
};


#if !_SC_NOINLINES
  #include "SCMachine.inl.h"
#endif


#endif  //  __SCMACHINE__
