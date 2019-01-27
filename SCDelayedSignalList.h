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
| SCDelayedSignalList |  SCDelayedSignalList.h  | 29. Feb 1996  |   SCL             |       |
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


#ifndef __SCDELAYEDSIGNALLIST__

#define __SCDELAYEDSIGNALLIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCListSave.h"

class SCDelayedSignal;

typedef SCListCons<SCDelayedSignal> SCDelayedSignalCons;
typedef SCListSave<SCDelayedSignal> SCDelayedSignalSaveList;
typedef SCListIter<SCDelayedSignal> SCDelayedSignalIter;

#endif  // __SCDELAYEDSIGNALLIST__
