/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                 |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCList      |   SCList.cc       | 24. Feb 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
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


#ifndef TEST_TEMPLATE
//#define   TEST_TEMPLATE  // Define this to test the implementation
#endif


#ifdef  TEST_TEMPLATE
    #include "SCList.tmpl.h"

template class SCListCons<SCInteger>;
template class SCListIter<SCInteger>;
template class SCList<SCInteger>;

#endif  // TEST_TEMPLATE

