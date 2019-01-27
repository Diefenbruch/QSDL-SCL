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
| SCRandNormal  | SCRandNormal.cc   | 23. Mai 1994  |   SCL             |      |
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
#include "SCRandNormal.h"

#include <math.h>

#if _SC_NOINLINES
  #include "SCRandNormal.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandNormal::SCRandNormal (SCReal m, SCReal sd)
{
  Init (m, sd);

  z = 0.0;
}


SCReal SCRandNormal::operator() (void)
{
  // Use the polar method, due to Box, Muller and Marsaglia
  // Taken from Seminumerical Algorithms, Knuth, Addison-Wesley, p.117

  SCReal  x2;
  SCReal  s,
          v1,
          v2;

  if (z != 0.0)
  {
    x2 = z;
    z = 0.0;
  }
  else
  {
    do
    {
      v1 = 2.0 * Uniform() - 1.0;
      v2 = 2.0 * Uniform() - 1.0;
      s = v1*v1 + v2*v2;
    }
    while (s >= 1.0);

    s = sqrt ((-2.0 * log (s)) / s);
    x2 = v1 * s;
    z  = v2 * s;
  }

  return (mean + x2 * stdDev);
}


void SCRandNormal::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandNormal::Draw (SCInteger &var)
{
  var = (SCInteger)(*this)();
}


SCStream& SCRandNormal::Display (SCStream& pStream) const
{
  pStream << "'Normal', " << mean << ", " << stdDev;

  return pStream;
}
