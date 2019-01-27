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
|   SCTrace     |   SCTrace.cc      | 23. Sep 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     23.09.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCTrace.h"


#if _SC_NOINLINES
  #include "SCTrace.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCTrace::SCTrace (SCInteger pActionFlags,
  								const SCObject* pParent) :
  SCObject(SC_TRACE, pParent),
  actionFlags(pActionFlags),
  container(NULL)
{
}


SCTrace::~SCTrace()
{
}
