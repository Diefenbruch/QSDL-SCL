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
|  SCTimerType  |  SCTimerType.h    | 2. Apr 1995   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     02.04.95    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCTIMERTYPE__

#define __SCTIMERTYPE__


#include "SCSignalType.h"
#include "SCTimerListTable.h"


class SCTimerType : public SCSignalType
{
  public:
                       SCTimerType (SCNatural typeKey,
                                    const char *typeName,
                                    const SCObject* father = NULL);
                      ~SCTimerType (void);

    SCTimerListTable * GetTimerListTable (void) const { return timerListTable; }

    friend SCStream&    operator<< (SCStream &pStream,
                                   const SCTimerType &pData);

  private:
    SCTimerListTable * timerListTable;
};


#endif  // __SCTIMERTYPE__
