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
| SCStackElemList   | SCStackElemList.h | 22. Apr 1997  |   SCL             |   |
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


#ifndef __SCSTACKELEMLIST__

#define __SCSTACKELEMLIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCList.h"

class SCStackElem;

typedef SCListCons<SCStackElem> SCStackElemCons;
typedef SCList<SCStackElem>     SCStackElemList;
typedef SCListIter<SCStackElem> SCStackElemIter;

#endif  // __SCSTACKELEMLIST__
