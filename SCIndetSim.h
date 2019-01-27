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
|  SCIndetSim   |  SCIndetSim.h     | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCINDETSIM__

#define __SCINDETSIM__

#include "SCBasicTypes.h"
#include "SCIndet.h"
#include "SCEnabledTransition.h"  // needed for template instantiation!
#include "SCEnabledTransitionList.h"

class SCIndetSim : public SCIndet
{
  public:
                              SCIndetSim (SCTime    stopTime = 0.0,
                                          SCBoolean errors = false,
                                          SCNatural numErrors = 0,
                                          SCBoolean deadCode = false,
                                          SCNatural maxDepth = kSCMaxDepth,
                                          SCName2PropFuncPtr * qAtom = NULL);
    virtual                  ~SCIndetSim (void);

    SCNatural                 NonDetChoice (const SCNatural range);
    SCNatural                 ChooseOneTrue (const SCNatural choiceSize,
                                             const SCNatural numOfTrues,
                                             const SCNatural singleTrue,
                                             const SCBoolean *choiceSet);

    class SCRunnable  *       ChooseRunnable (SCTimeEventSaveList *activeQueue);
    SCEnabledTransition *     ChooseTransition (void);

    void                      StartMessage (void);
    void                      EndMessage (void);

		SCBoolean                 Create (SCProcessID) { return true; }

    // The following three methods classify the reached system
    // state:
    
    SCErrorCode               IsErrorState (SCTimeEventSaveList *activeQueue);
    SCBoolean                 IsLimitingState (SCTimeEventSaveList *activeQueue);

    SCBoolean                 LogError (SCErrorCode errorCode);         // deadlock
    SCBoolean                 LogError (SCErrorCode errorCode,          // signal drop or
                                        const class SCProcess *process, // signal rejected
                                        const class SCSignal *signal);
    SCBoolean                 LogError (SCErrorCode errorCode,          // no receiver
                                        const class SCProcess *process,
                                        const class SCSignalType *signal);
    SCBoolean                 LogError (SCErrorCode errorCode,          // process creation
                                        const class SCProcess *parent,  // failed
                                        const class SCProcessType *offspringType);

    friend SCStream &         operator<< (SCStream& pStream,
                                          const SCIndetSim& pData);
    virtual SCStream &        Display (SCStream &pStream) const;

  private:
    class SCRandUniform *     randomizer;
    SCNatural                 maxDepth;
    SCEnabledTransitionList * enabledTransitionList; // for data exchange between
                                                     // ChooseRunnable and ChooseTransition
};


#if !_SC_NOINLINES
  #include "SCIndetSim.inl.h"
#endif


#endif  // __SCINDETSIM__
