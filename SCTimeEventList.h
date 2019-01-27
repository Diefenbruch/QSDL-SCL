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
| SCTimeEventList   | SCTimeEventList.h | 24. Jan 1997  |   SCL             |   |
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


#ifndef __SCTIMEEVENTLIST__

#define __SCTIMEEVENTLIST__

#include "SCList.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCTimeEventTable.h"

class SCTimeEvent;

typedef SCListCons<SCTimeEvent> SCTimeEventCons;
typedef SCListIter<SCTimeEvent> SCTimeEventIter;

class SCTimeEventSaveList : public SCList<SCTimeEvent>
{
  public:
                        SCTimeEventSaveList (SCBoolean isFair = false,
                                             SCNatural pSize = kSCDefaultTableSize,
                                             SCObject *pParent = NULL);
                       ~SCTimeEventSaveList (void);

    void                RemoveAllElements(void);
    SCTimeEventCons *   Insert (SCTimeEvent *toInsert);
    class SCTimeEvent * Remove (SCTimeEvent* element);
    class SCTimeEvent * Remove (SCTimeEventCons* element = NULL);

    SCBoolean           Save (SCMem &saveArea) const;
    SCBoolean           Restore (SCMem &saveArea);
    void                Size(SCSize *pSize) const;

    class SCTimeEvent * LookupRunnable(class SCRunnable *runnable) const;
    
  private:
    SCBoolean           fair;      // important for Insert() method
                                   // true: maximum progress
                                   // false: round robin
    SCTimeEventTable *  lookupTable;
};

#endif  // __SCTIMEEVENTLIST__
