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
| SCRandUniLong | SCRandUniLong.cc  | 23. Mai 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     23.05.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCRandUniLong.h"

#if _SC_NOINLINES
  #include "SCRandUniLong.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandUniLong::SCRandUniLong (SCInteger l, SCInteger h)
{
  Init (l, h);
}


SCInteger SCRandUniLong::operator() (void)
{
  return ((SCInteger)(low + (range * Uniform())));
}


void SCRandUniLong::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandUniLong::Draw (SCInteger &var)
{
  var = (*this)();
}


SCStream& SCRandUniLong::Display (SCStream& pStream) const
{
  pStream << "'UniLong', " << low << ", " << low + range;

  return pStream;
}
