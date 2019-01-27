/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+-------------------+-------------------+---------------+-------------------+---+
|       Module      |   File            |   Created     |   Project         |   |
+-------------------+-------------------+---------------+-------------------+---+
| SCTimeEvent       |  SCTimeEvent.h    | 04. Mai 1996  |   SCL             |   |
+-------------------+-------------------+---------------+-------------------+---+
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


#ifndef __SCTIMEEVENT__

#define __SCTIMEEVENT__


#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCTimeEventList.h"

class SCTimeEvent: public SCObject
{
  public:
                               SCTimeEvent (class SCRunnable *pRunnable,
                                            const SCTime &pWhen,
                                            const SCNatural pTimerID = 0,
                                            const SCObject *pParent = NULL);
                               SCTimeEvent (SCRunnableID pID,
                                            const SCTime &pWhen,
                                            const SCNatural pTimerID = 0,
                                            const SCObject *pParent = NULL);
                               SCTimeEvent (class SCTimeEvent &orig); // for SCActiveRunnable
                               SCTimeEvent(SCMem & saveArea,     // for Restore
                                           const SCObject *pParent = NULL);
                              ~SCTimeEvent (void);

    SCRunnableID               GetRunnableID(void) const { return runnableID; }
    class SCRunnable *         GetRunnable(void) const;
    const SCTime               GetTime(void) const;
    const SCNatural            GetTimerID(void) const;
    
    SCBoolean                  IsAutomaton (void) const { return isAutomaton; }    

    friend SCStream&            operator<< (SCStream& pStream,
                                           const SCTimeEvent& pData);
    virtual SCStream &          Display (SCStream &pStream) const;

    SCBoolean                  operator== (const SCTimeEvent & second) const;
    SCBoolean                  operator!= (const SCTimeEvent & second) const;

    SCBoolean                  Save (SCMem&) const;
    SCBoolean                  Restore (SCMem&);
    void                       Size (SCSize *pSize) const;

    SCTimeEventCons *          GetContainer (void) const { return container; }
    void                       SetContainer (SCTimeEventCons *cont) { container = cont; }

  private:
    SCRunnableID               runnableID;
    class SCRunnable *         runnable;
    SCBoolean                  isAutomaton;
    SCTime                     when;
    SCNatural                  timerID;     // for use in SCTimerControl only
                                            // not used for activeQueue
    
    SCTimeEventCons *          container;   // actual SCListCons elem
                                            // in which signal is
                                            // embedded (do not save!)

};

#endif  //  __SCTIMEEVENT__
