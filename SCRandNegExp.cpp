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
| SCRandNegExp  | SCRandNegExp.cc   | 23. Mai 1994  |   SCL             |      |
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
#include "SCRandNegExp.h"

#include <math.h>

#if _SC_NOINLINES
  #include "SCRandNegExp.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandNegExp::SCRandNegExp (SCReal r)
{
  Init (r);
}


SCReal SCRandNegExp::operator() (void)
{
  return (rate * log (Uniform()));
}


void SCRandNegExp::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandNegExp::Draw (SCInteger &var)
{
  var = (SCInteger)(*this)();
}


SCStream& SCRandNegExp::Display (SCStream& pStream) const
{
  pStream << "'NegExp', " << rate;

  return pStream;
}
