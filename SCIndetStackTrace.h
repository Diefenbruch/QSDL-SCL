/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1996 University of Essen, Germany                                       |
|                                                                               |
+---------------+----------------------+---------------+----------------+-------+
|   Module      |   File               |   Created     |   Project      |       |
+---------------+----------------------+---------------+----------------+-------+
|SCIndetStackTrace|SCIndetStackTrace.h | 05. Sepr 1996 |   SCL          |       |
+---------------+----------------------+---------------+----------------+-------+
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


#ifndef __SCINDETSTACKTRACE__

#define __SCINDETSTACKTRACE__

#include "SCBasicTypes.h"
#include "SCIndet.h"
#include "SCEnabledTransitionList.h"
//#include "SCTimeEventList.h"

class SCIndetStackTrace : public SCIndet
{
  public:
                          SCIndetStackTrace (class SCIndetVal *indetVal,
                                             class SCTrace *errorTrace);

    virtual              ~SCIndetStackTrace (void);

    SCNatural             NonDetChoice (const SCNatural range);
    SCNatural             ChooseOneTrue (const SCNatural choiceSize,
                                         const SCNatural numOfTrues,
                                         const SCNatural singleTrue,
                                         const SCBoolean *choiceSet);
    class SCRunnable  *   ChooseRunnable (SCTimeEventSaveList *activeQueue);
    class SCEnabledTransition *ChooseTransition (void);

    void                  StartMessage (void);
    void                  EndMessage (void);

    // The following three methods classify the reached system
    // state:
    
		SCBoolean             Create (SCProcessID) { return true; }

    SCErrorCode           IsErrorState (SCTimeEventSaveList *activeQueue);
    SCBoolean             IsLimitingState (SCTimeEventSaveList *activeQueue);

    void                  RestoreStartState (void); // SCTimeEventSaveList *activeQueue);

    SCBoolean             LogError (SCErrorCode errorCode)      // deadlock
                            { (void)errorCode; return true; }
    SCBoolean             LogError (SCErrorCode errorCode,          // signal droped or
                                    const class SCProcess *process, // signal rejected
                                    const class SCSignal *signal)
                            { (void)errorCode; (void)process; (void)signal; return true; }
    SCBoolean             LogError (SCErrorCode errorCode,         // or no receiver
                                    const class SCProcess *process,
                                    const class SCSignalType *signal)
                            { (void)errorCode; (void)process; (void)signal; return true; }
    SCBoolean             LogError (SCErrorCode errorCode,         // process creation
                                    const class SCProcess *pParent, // failed
                                    const class SCProcessType *offspringType)
                            { (void)errorCode; (void)pParent; (void)offspringType; return true; }

  private:
    class SCIndetVal *        theIndetVal;
    const class SCStackElem * curStackPos;        // Current position in stack.
    SCNatural                 curDepth;
    class SCTrace *           errorTracing;
    SCBoolean                 origVerbose;
    SCBoolean                 finish1;            // true if the end of 1. stack is reached
    SCBoolean                 finish2;            // true if the end of 2. stack is reached
    SCBoolean                 finish3;            // true if the end of 1. stack is reached again
    SCStackElem *             lastElem;
};


#if !_SC_NOINLINES
  #include "SCIndetStackTrace.inl.h"
#endif


#endif  // __SCINDETSTACKTRACE__
