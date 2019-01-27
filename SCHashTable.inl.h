/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch                                               |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCHashTable  | SCHashTable.inl.h | 19. Nov 1997  |   SCL             |      |
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


#ifndef __SCHASHTABLE_INL__

#define __SCHASHTABLE_INL__

#include <math.h>

#include "SCMem.h"

#if _SC_USE_STANDARD_BYTEORDER
  #include <sys/types.h>
  #include <netinet/in.h>
#endif


_SCINLINE_ SCNatural SCHashTable::GetNumOfAnalyzedStates(void) const
{
  return analyzedStates;
}


_SCINLINE_ SCNatural SCHashTable::GetStateSpaceSize(void) const
{
  return hashTableSize;
}


_SCINLINE_ SCNatural SCHashTable::GetTablePower(void) const
{
  return hashTablePower;
}


_SCINLINE_ SCNatural SCHashTable::GetFunctionIndex(void) const
{
  return hashFunctionIndex;
}


_SCINLINE_ SCInteger SCHashTable::GetCurrentKey(void) const
{
  return m;
}


_SCINLINE_ void SCHashTable::CalcBasicHashKey (const SCMem *state)
{
  m = -1;
  h = state->save_area_size / sizeof(long);   // Assumption: save_area is allocated
                                              // to sizeof(long) boundaries.
  q = (long *)state->save_area;               // 4 Strukturbytes als
                                              // long interpretieren
  do
  {
    m += m;
    if (m < 0)
    {
      m ^= z;
    }
#if _SC_USE_STANDARD_BYTEORDER
    m ^= htonl(*q++);
#else
    m ^= *q++;
#endif
  } while(--h > 0);
}


_SCINLINE_ SCNatural SCHashTable::Holzmann1(const SCMem *state) 
{
  CalcBasicHashKey(state);
  
  return (m ^ (m >> (32 - hashTablePower))) & hashMask;
}


_SCINLINE_ SCNatural SCHashTable::Holzmann2(const SCMem *state) 
{
  unsigned short     pos;
  register SCNatural hash_value;

  CalcBasicHashKey(state);

  pos = hashTablePower;
  hash_value = m;
  while (pos < 32)
  {
    hash_value ^= m >> pos;
    pos += hashTablePower;
  }
  return hash_value &= hashMask;

}


_SCINLINE_ SCNatural SCHashTable::Multiply(const SCMem *state) 
{
  long double currentPhi = phi;
  long double hashFactor;
  
  CalcBasicHashKey(state);

  currentPhi *= m;

  hashFactor = currentPhi - floor(currentPhi);
  
  return (SCNatural) floor(hashTableSize * hashFactor);

}


_SCINLINE_ SCNatural SCHashTable::Modulo(const SCMem *state)
{
  CalcBasicHashKey(state);
  
  return (m % primes[hashTablePower - 1]);

}


#endif  // __SCHASHTABLE_INL__
