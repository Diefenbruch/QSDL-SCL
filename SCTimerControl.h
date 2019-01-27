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
| SCTimerControl| SCTimerControl.h  | 30. Aug 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     30.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCTIMERCONTROL__

#define __SCTIMERCONTROL__

#include "SCBasicTypes.h"
#include "SCRunnable.h"
#include "SCTimeEventList.h"

class SCTimerControl : public SCRunnable
{
  public:
                            SCTimerControl (void);
    virtual                ~SCTimerControl (void);

    static SCTimerControl * GetTimerControl (void) { return timerControl; }

    void                    Reschedule (void);
    SCTimeEventSaveList *   GetTimerEventList (void) const { return timerEventList; }

    SCTimeEventCons *       AddTimerEvent (const class SCTimer *pTimer);

    void                    RemoveTimerEvent (SCTimeEventCons *pElem);
                                           
    friend SCStream &        operator<< (SCStream& pStream,
                                        const SCTimerControl& pData);
    virtual SCStream &       Display (SCStream& pStream) const;

    SCBoolean               Save (SCMem &) const;
    SCBoolean               Restore (SCMem &);

    void                    Size (SCSize *pSize) const;

  private:
    void                    Body (void);

    SCTimeEventSaveList *   timerEventList;

    static SCTimerControl * timerControl;
};


#endif  //  __SCTIMERCONTROL__
