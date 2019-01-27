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
|SCRandEmpirical| SCRandEmpirical.cc| 23. Mai 1994  |   SCL             |      |
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


#include <stdio.h>

#include "SCStream.h"
#include "SCRandEmpirical.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCRandEmpirical::SCRandEmpirical (SCInteger n, SCFileSpec fn) :
  values(NULL)
{
  if (n && fn)
    Init (n, fn);
}


SCRandEmpirical::~SCRandEmpirical (void)

{
  if (values)
    delete[] values;
}


SCReal SCRandEmpirical::operator() (void)
{
  return (values ? values[(SCInteger)(Uniform() * numValsReal) % numVals] : 0.0);
}


void SCRandEmpirical::Draw (SCReal &var)
{
  var = (*this)();
}


void SCRandEmpirical::Draw (SCInteger &var)
{
  var = (SCInteger)(*this)();
}


void SCRandEmpirical::Init (SCInteger n, SCFileSpec fn)
{
  SCInteger   z_i;
  FILE *      file;

  if (values)
    delete[] values;
  numVals = n;
  numValsReal = (SCReal)n;
  values = new SCReal [n];
  file = fopen (fn, "r");
  for (z_i = 0; z_i < n; z_i++)
    fscanf (file, "%lf", &values[z_i]);
  fclose (file);
}


SCStream& SCRandEmpirical::Display (SCStream& pStream) const
{
  pStream << "'Empirical', " << numVals;

  return pStream;
}
