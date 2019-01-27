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
| SCRequestList |  SCRequestList.h  | 29. Feb 1996  |   SCL             |       |
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


#ifndef __SCREQUESTLIST__

#define __SCREQUESTLIST__

#include "SCBasicTypes.h"
#include "SCListSave.h"
#include "SCListCons.h"
#include "SCListIter.h"

class SCRequest;

typedef SCListCons<SCRequest> SCRequestCons;
typedef SCList<SCRequest>     SCRequestList;
typedef SCListSave<SCRequest> SCRequestSaveList;
typedef SCListIter<SCRequest> SCRequestIter;

#endif  // __SCREQUESTLIST__
