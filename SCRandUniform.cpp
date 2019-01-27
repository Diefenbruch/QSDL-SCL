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
| SCRandUniform | SCRandUniform.cc  | 23. Mai 1994  |   SCL             |      |
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
#include "SCRandUniform.h"

#if _SC_NOINLINES
  #include "SCRandUniform.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandUniform::SCRandUniform (SCReal l, SCReal h)
{
  Init (l, h);
}


SCReal SCRandUniform::operator() (void)
{
  return (low + (range * Uniform()));
}


void SCRandUniform::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandUniform::Draw (SCInteger &var)
{
  var = (SCInteger)(*this)();
}


SCStream& SCRandUniform::Display (SCStream& pStream) const
{
  pStream << "'Uniform', " << low << ", " << low + range;

  return pStream;
}
