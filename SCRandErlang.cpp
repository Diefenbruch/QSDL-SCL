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
| SCRandErlang  | SCRandErlang.cc   | 23. Mai 1994  |   SCL             |      |
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

#include <math.h>

#include "SCStream.h"
#include "SCRandErlang.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandErlang::SCRandErlang (SCReal m, SCReal sd)
{
  Init (m, sd);
}


void SCRandErlang::Init (SCReal newMean, SCReal newStdDev)
{
  SCReal  z;

  mean = newMean;
  stdDev = newStdDev;

  if (stdDev != 0.0)
  {
    z = mean / stdDev;
    k = (SCInteger) (z * z);
  }
}


SCReal SCRandErlang::operator() (void)
{
  SCReal    z = 1.0;
  SCInteger z_i;

  for (z_i = k; z_i > 0; z_i--)
    z *= Uniform();
  return (-(mean / k) * log(z));
}


void SCRandErlang::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandErlang::Draw (SCInteger &var)
{
  var = (SCInteger)(*this)();
}


SCStream& SCRandErlang::Display (SCStream& pStream) const
{
  pStream << "'Erlang', " << mean << ", " << stdDev;

  return pStream;
}
