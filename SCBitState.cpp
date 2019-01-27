/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (C) 1994-98 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCBitState   |  SCBitState.cc    | 30. Apr 1996  |   SCL             |      |
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

#include <stdlib.h>

#include "SCBitState.h"
#include "SCMem.h"
#include "SCDebug.h"
#include "SCStream.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCBitState::SCBitState (SCNatural hashTablePow,
                        SCNatural hashFuncIndex,
                        const SCObject * father):
  SCHashTable(scBitStateSpace, hashTablePow, hashFuncIndex, father)
{
  assert(hashTablePow >= 3);

  do
  {
                                                  /* Groesse der Hash-Tabelle */
                                                  /* 2^hash_table_pow         */
    hashTableSize = (1L << hashTablePow);

    hashTable = (char *) calloc(hashTableSize / 8, 1);
                                                  /* allocate memory          */
                                                  /* divide by 8 because each */
                                                  /* byte can store 8 hash    */
                                                  /* addresses                */

  }
  while ((hashTable == NULL) && ((hashTablePow)-- >= 3));
                                                  // Not enough memory. Try to
                                                  // reduce the size of the hashtable.

  assert(hashTable);
  
  hashTablePower = hashTablePow;
  hashMask = (1L << hashTablePower) - 1;          // Set mask for hashfunctions.
}


SCBitState::~SCBitState (void)
{
  free(hashTable);                                // Deallocate memory for hashtable.
}


SCBoolean SCBitState::IsStateAnalyzed(SCMem *state)
{
  register SCBoolean  result;                     // Temporary for result.

  register SCNatural  hash_value = (this->*hashFunction)(state);  // Calculate hash value.
  
  result = hashTable[hash_value >> 3] & (1 << (hash_value & 7L));
                                                  // Extract bit at position hash_value.
                                                  
  hashTable[hash_value >> 3] |= (1 << (hash_value & 7L));
                                                  // Set bit at position hash_value.

  if (!result)
  {
    analyzedStates++;                             // Increment counter.

#if _SC_SHOW_PROGRESS
    if (!(analyzedStates % _SC_PROGRESS_STEP)) { SCScheduler::outputStream << "#"; SCScheduler::outputStream.GetStream().flush(); }
#endif
  }

  return(result);

}



SCStream& operator<< (SCStream& pStream,
                     const SCBitState& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCBitState::Display (SCStream& pStream) const
{
  pStream << "Double states:              avoided by bit-state hashing";
  pStream << std::endl;

  SCHashTable::Display(pStream);

  return pStream;
}
