/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCByte      |   SCByte.cc       | 17. Aug 1995  |   SCL             |      |
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

#include "SCByte.h"

#include "SCCharstring.h"

#include <stdio.h>

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
    #include "SCByte.inl.h"
#endif


SCByte bhex (const SCCharstring &cs)
{
  long  longVal;

  sscanf (cs.GetString(), "%lx", &longVal);
  return ((SCByte)longVal);
}
