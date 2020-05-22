/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCHashTable  |  SCHashTable.cc   | 18. Nov 1997  |   SCL             |      |
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

#include "SCHashTable.h"
#include "SCStream.h"
#include "SCMem.h"
#include "SCDebug.h"

#if _SC_NOINLINES
  #include "SCHashTable.inl.h"
#endif

SCNatural SCHashTable::primes[] = {1UL,
                                   3UL,
                                   7UL,
                                   13UL,
                                   31UL,
                                   61UL,
                                   127UL,
                                   251UL,
                                   509UL,
                                   1021UL,
                                   2039UL,
                                   4093UL,
                                   8191UL,
                                   16381UL,
                                   32749UL,
                                   65521UL,
                                   131071UL,
                                   262139UL,
                                   524287UL,
                                   1048573UL,
                                   2097143UL,
                                   4194301UL,
                                   8388593UL,
                                   16777213UL,
                                   33554393UL,
                                   67108859UL,
                                   134217689UL,
                                   268435399UL,
                                   536870909UL,
                                   1073741789UL,
                                   2147483647UL,
                                   4294967291UL};


SCHashTable::SCHashTable (const SCStateSpaceType pType,
                          SCNatural hashTablePow,
                          SCNatural hashFuncIndex,
                          const SCObject *father):
  SCStateSpace(pType, father),
  hashTablePower(hashTablePow),
  hashTableSize(0),
  hashFunctionIndex(hashFuncIndex),
  analyzedStates(0),
  hashMask(0),
  m(-1),
  h(0)
{
#if _SC_USE_STANDARD_BYTEORDER
  z = htonl(0x88888eefL);
#else
  z = 0x88888eefL;
#endif

  switch (hashFunctionIndex)
  {
    case 1:
      hashFunction = &SCHashTable::Holzmann1;
      break;

    case 2:
      hashFunction = &SCHashTable::Holzmann2;
      break;

    case 3:
      hashFunction = &SCHashTable::Multiply;
      phi = 0.618033988749894903;
      break;

    case 4:
      hashFunction = &SCHashTable::Multiply;
      phi = 0.612542337149213684;
      break;

    case 5:
      hashFunction = &SCHashTable::Modulo;
      break;

    default:
      assert (false);                         // Index out of range - bail out.
  }
}


SCHashTable::~SCHashTable (void)
{
}


SCStream& operator<< (SCStream& pStream,
                     const SCHashTable & pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCHashTable::Display (SCStream& pStream) const
{
  pStream << "Hashtable size:             2^";
  pStream << hashTablePower;
  pStream << " (" << hashTableSize;
  pStream << " elements)" << std::endl;
  pStream << "Hashfunction index:         ";
  pStream << hashFunctionIndex << std::endl;

#if _SC_SHOW_PROGRESS
  pStream << "Hash mark printing:         # = " << _SC_PROGRESS_STEP;
  pStream << " states" << std::endl;
#endif

  return pStream;
}
