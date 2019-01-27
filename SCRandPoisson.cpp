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
| SCRandPoisson | SCRandPoisson.cc  | 23. Mai 1994  |   SCL             |      |
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
#include "SCRandPoisson.h"

#if _SC_NOINLINES
  #include "SCRandPoisson.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandPoisson::SCRandPoisson (SCInteger m)
{
  Init (m);
}


SCInteger SCRandPoisson::operator() (void)
{
  SCReal    bound,
            product;
  SCInteger count = 0;

  bound = exp(-1.0 * mean);
  for (product = 1.0; product >= bound; product *= Uniform())
    count++;
  return (count - 1);
}


void SCRandPoisson::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandPoisson::Draw (SCInteger &var)
{
  var = (*this)();
}


SCStream& SCRandPoisson::Display (SCStream& pStream) const
{
  pStream << "'Poisson', " << mean;

  return pStream;
}
