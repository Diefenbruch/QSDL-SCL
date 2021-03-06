/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+-------------------+---------------+-------------------+--+
|       Module      |   File            |   Created     |   Project         |  |
+-------------------+-------------------+---------------+-------------------+--+
|  SCMemTable       | SCMemTable.h      | 26. Nov 1997  |   SCL             |  |
+-------------------+-------------------+---------------+-------------------+--+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#ifndef __SCMEMTABLE__

#define __SCMEMTABLE__

#include "SCBasicTypes.h"
#include "SCTable.h"

class SCMem;

typedef SCTable<SCMem> SCMemTable;

#endif  // __SCMEMTABLE__
