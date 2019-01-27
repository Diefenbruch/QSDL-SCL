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
|  SCFullState  |  SCFullState.cc   | 30. Apr 1996  |   SCL             |      |
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

#include "SCFullState.h"
#include "SCMem.h"
#include "SCDebug.h"
#include "SCStackElem.h"
#include "SCStream.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCFullState::SCFullState (SCNatural hashTablePow,
                          SCNatural hashFuncIndex,
                          const SCObject * father):
  SCHashTable(scFullStateSpace, hashTablePow, hashFuncIndex, father)
{
  hashTableSize = (1L << hashTablePow);           // Size of Hash-Table is
                                                  // 2^hash_table_pow

  hashTable = new SCMemTable(hashTableSize,       // allocate table
                             true,                // delete elems !
                             true,                // resolve collisions !
                             false,               // don't resize table
                             this);               // this is parent

  assert(hashTable);
  
  hashMask = (1L << hashTablePower) - 1;          // Set mask for hashfunctions.
  
  SCStackElem::SetDeleteState(false);             // don't delete system states
                                                  // if stack elem is popped
}


SCFullState::~SCFullState (void)
{
  delete hashTable;                                // Deallocate memory for hashtable.
}


SCBoolean SCFullState::IsStateAnalyzed(SCMem *state)
{
  register SCMem *   result;                     // Temporary for result.

  register SCNatural hash_value = (this->*hashFunction)(state);  // Calculate hash value.

  if (hashTable->IsFull())
  {
    return true;
  }
    
  result = hashTable->Find(hash_value, state);
  
  if (result == NULL)
  {                                                  
    hashTable->Insert(hash_value, state);

    analyzedStates++;                             // Increment counter.

#if _SC_SHOW_PROGRESS
    if (!(analyzedStates % _SC_PROGRESS_STEP)) { SCScheduler::outputStream << "#"; SCScheduler::outputStream.GetStream().flush(); }
#endif
    return false;
  }

  return true;

}


SCStream& operator<< (SCStream& pStream,
                     const SCFullState& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCFullState::Display (SCStream& pStream) const
{
  pStream << "Double states:              avoided by full-state hashing";
  pStream << std::endl;

  SCHashTable::Display(pStream);

  return pStream;
}
