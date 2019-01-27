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
| SCSignalList |  SCSignalList.h  | 29. Feb 1996  |   SCL             |       |
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


#ifndef __SCSIGNALLIST__

#define __SCSIGNALLIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCListSave.h"

class SCSignal;

typedef SCListCons<SCSignal> SCSignalCons;
typedef SCListSave<SCSignal> SCSignalSaveList;
typedef SCListIter<SCSignal> SCSignalIter;

#endif  // __SCSIGNALLIST__
