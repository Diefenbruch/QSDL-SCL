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
|   SCProcess   |   SCProcess.h     | 5. Jul 1994   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     05.07.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#ifndef __SCPROCESS__

#define __SCPROCESS__

#include "SCBasicTypes.h"
#include "SCSignal.h"     // don't remove this; it's needed for template instantiation
#include "SCRequest.h"    // don't remove this; it's needed for template instantiation
#include "SCSignalList.h"
#include "SCTimer.h"      // don't remove this; it's needed for template instantiation
#include "SCTimerList.h"
#include "SCAutomaton.h"
#include "SCProcessList.h"

#include "SCTraceSensor.h"

class SCProcess : public SCAutomaton
{
  public:
    virtual                  ~SCProcess (void);

    class SCProcessType *     GetType (void) const;

    SCSignalSaveList *        GetInputQueue (void) const { return inputQueue; }
    SCTimerSaveList *         GetTimerQueue (void) const { return timerQueue; }

    virtual void              Stop (SCBoolean traceIt = true);

    friend SCStream &         operator<< (SCStream& pStream,
                                          const SCProcess& pData);
    virtual SCStream &        Display (SCStream& pStream) const;

    SCBoolean                 Save (SCMem &) const;
    SCBoolean                 Restore (SCMem &);
   
    void                      Size (SCSize *pSize) const;

    SCProcessCons *           GetContainer (void) const { return container; }
    void                      SetContainer (SCProcessCons *cont) { container = cont; }

    SCBoolean                 GetCallScheduler (void) const;
    void                      SetCallScheduler (SCBoolean pCallScheduler);

  protected:
                              SCProcess (const SCProcessID creator,
                                         const class SCProcessType *pProcType,
                                         class SCStateType *startState,
                                         class SCDataType *processParameters = NULL,
                                         const SCBoolean varSize = false,
                                         const SCObject* pParent = NULL);
                              SCProcess (const SCBoolean varSize = false,
                                         const SCObject *pParent = NULL);

    SCProcessID               Self (void) const;
    SCProcessID               Sender (void) const;
    SCProcessID               Parent (void) const;
    SCProcessID               Offspring (void) const;

    void                      SetSender (const SCProcessID pSender);
    void                      SetOffspring (const SCProcessID pOffspring);

    void                      Receive (class SCSignal *signal);
    void                      ReceiveFromTimer (SCNatural timerID);
    SCTimerCons *             LocateTimer (const SCTimerType *const timerType,
                                           const SCDataType *const data) const;

  private:
    SCProcessID               parentID;               // ID of parent process
    SCProcessID               senderID;               // ID of last sender 
    SCProcessID               offspringID;            // ID of last created child-process
    SCSignalSaveList *        inputQueue;
    SCTimerSaveList *         timerQueue;
    class SCProcessType *     procType;
    SCBoolean                 callScheduler;          // should State method activate
                                                      // Scheduler? don't save!

    SCProcessCons *           container;              // actual SCListCons elem
                                                      // in which process is
                                                      // embedded (do not save!)
  friend class SCProcessType;
  friend class SCAutomaton;
  friend class SCTimerControl;
  friend class SCPath;
  friend class SCProcedure;
};


#if !_SC_NOINLINES
  #include "SCProcess.inl.h"
#endif


#endif  // __SCPROCESS__
