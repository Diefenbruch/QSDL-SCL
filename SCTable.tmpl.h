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
|   SCTable     |   SCTable.tmpl.h  | 24. Feb 1996  |   SCL             |      |
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

#ifndef	__SCTABLE_TMPL__

#define	__SCTABLE_TMPL__

#if _SC_PROFILING
extern int table_ineff;
#endif

#include "SCStream.h"
#include "SCTable.h"
#include "SCObject.h"
#include "SCListCons.h"
#include "SCListIter.h"


template<class T> SCTable<T>::SCTable (SCNatural size,
                                       SCBoolean delElems,
                                       SCBoolean rslvColl,
                                       SCBoolean resize,
                                       const SCObject* father) :
  SCObject(SC_TABLE, father),                     // Base class initialization.
  tableSize(size),
  numOfElems(0),                                  // Table is empty
  deleteElems(delElems),
  resolveCollisions(rslvColl),
  resizeIfFull(resize)
{
  Initialize();
}


template<class T> SCTable<T>::~SCTable (void)
{
  SCNatural i;

  if (deleteElems)
  {
    for (i = 0; i < tableSize; i++)
    {
      if (table[i] != NULL)
      {
        delete table[i];
      }
    }
  }
  delete[] table;
  delete[] tableKeys;
  if (keyCollisions) delete[] keyCollisions;
}


template<class T> SCBoolean SCTable<T>::Initialize (void)
{
  SCNatural i;

  table = new T*[tableSize];
  assert(table);

  tableKeys = new SCInteger[tableSize];           // original keys of
  assert(tableKeys);                              // elems stored at a
                                                  // position
  if (resolveCollisions)
  {
    keyCollisions = new SCNatural[tableSize];
    assert(keyCollisions);
  }
  else
  {
    keyCollisions = NULL;
  }
  
  for (i = 0; i < tableSize; i++)
  {
    table[i] = NULL;
    tableKeys[i] = kSCEmptyTableEntry;
    if (resolveCollisions)
      keyCollisions[i] = 0;
  }
  
  numOfElems = 0;

  return true;
}


template<class T> SCBoolean SCTable<T>::Resize (void)
{
  SCNatural   i;
  T **        oldTable;
  SCInteger * oldTableKeys;
  SCNatural * oldKeyCollisions;
  SCNatural   oldTableSize;

  if (tableSize >= ULONG_MAX - _SC_TABLE_RESIZE_AMOUNT)
  {
    return false;
  }
  
  oldTable = table;
  oldTableKeys = tableKeys;
  oldKeyCollisions = keyCollisions;
  oldTableSize = tableSize;
  
  tableSize += _SC_TABLE_RESIZE_AMOUNT;
  
  if (!Initialize())
  {
    return false;
  }
  
  for (i = 0; i < oldTableSize; i++)
  {
    if (oldTable[i])
      Insert(oldTableKeys[i], oldTable[i]);
  }
  
  delete[] oldTable;
  delete[] oldTableKeys;
  if (oldKeyCollisions) delete[] oldKeyCollisions;

  return true;
}


template<class T> SCBoolean SCTable<T>::Insert (SCInteger key, T *element)
{
  SCNatural insertKey = key % tableSize;

#if _SC_LOCKDATA
  Lock();
#endif

  if (IsFull())
  {
    if (!resizeIfFull)
    {
#if _SC_LOCKDATA
      Unlock();
#endif
      return false;
    }
    else
    {
      Resize();
      insertKey = key % tableSize;
    }
  }

  if (tableKeys[insertKey] != kSCEmptyTableEntry)
  {
    if (resolveCollisions)      // collions detection wanted?
    {
      keyCollisions[insertKey]++; // one more collision for this key

      do
      {
        insertKey = (insertKey + 1) % tableSize;
      } while (tableKeys[insertKey] != kSCEmptyTableEntry);
    }
    else                        // no collision detection!
    {
      std::cerr << "SCBoolean SCTable<T>::Insert(): table slot already in use!";
      std::cerr << std::endl;

#if _SC_LOCKDATA
      Unlock();
#endif
      return false;
    } // else
  } // if
   
  tableKeys[insertKey] = key;
  table[insertKey] = element;

  numOfElems++;

#if _SC_LOCKDATA
  Unlock();
#endif

  return true;
}


/////////////////////////////////////////////////////////
// Remove(): Searching for a element with a certain key
//           and rfemoving it from the hash table.
//           The second parameter removeElem is optional.
//           If it is given, not only the key must fit
//           but also the elements itself are compared.
//           The second parameter is useful if the keys are
//           not unique. This is used for SCMemTable
/////////////////////////////////////////////////////////

template<class T> T *SCTable<T>::Remove (SCInteger key,
                                         const T * removeElem)
{
  SCNatural removeKey = key % tableSize;
  SCNatural counter = 0, origRemoveKey;
  T *       returnElem;

#if _SC_LOCKDATA
  Lock();
#endif

  origRemoveKey = removeKey;

  if (resolveCollisions &&
      (tableKeys[removeKey] != key ||  // elem not at hash position?
       (removeElem && *table[removeKey] != *removeElem)))
  {
    if (keyCollisions[removeKey] == 0)  // is it not possible to find
    {                                   // it by linear search?
#if _SC_LOCKDATA
      Unlock();
#endif
      return NULL;                      // give up!
    }
    do                                  // linear search...
    {
      removeKey = (removeKey + 1) % tableSize;
      if (counter > tableSize)
      {
#if _SC_PROFILING
        table_ineff += counter;
        std::cerr << "Inefficient use of SCTable<T>::Remove" << std::endl;
#endif

#if _SC_LOCKDATA
        Unlock();
#endif
        return NULL; // zyklisch alles durchsucht?
      } // if
      counter++;
    } while (tableKeys[removeKey] != key ||
             (removeElem && *table[removeKey] != *removeElem));
  } // if
  else // don't resolve collions !
  {
    if (tableKeys[removeKey] == kSCEmptyTableEntry)
    {
      std::cerr << "SCTable<T>::Remove(): Slot is empty!" << std::endl;

#if _SC_LOCKDATA
      Unlock();
#endif

      return NULL;
    }
  }
  
  returnElem = table[removeKey];          // retrieve elem

  if (removeKey != origRemoveKey)
  {
    assert(keyCollisions[origRemoveKey] > 0);
    keyCollisions[origRemoveKey]--;      // decrement key usage
  }
  
  tableKeys[removeKey] = kSCEmptyTableEntry;  // empty slot
  table[removeKey] = NULL;

  numOfElems--;

#if _SC_PROFILING
  table_ineff += counter;
#endif

#if _SC_LOCKDATA
  Unlock();
#endif

  return returnElem;
}


/////////////////////////////////////////////////////////
// Find(): Searching for a element with a certain key.
//         The second parameter findElem is optional.
//         If it is given, not only the key must fit
//         but also the elements itself are compared.
//         The second parameter is useful if the keys are
//         not unique. This is used for SCMemTable
/////////////////////////////////////////////////////////

template<class T> T *SCTable<T>::Find (SCInteger key,
                                       const T * findElem) const
{
  SCNatural findKey = key % tableSize;
  SCNatural counter = 0;
  T *       returnElem;

#if _SC_LOCKDATA
  Lock();
#endif

  if (resolveCollisions &&           // do we care about collisions?
      (tableKeys[findKey] != key ||  // elem not at hash position?
       (findElem && *table[findKey] != *findElem)))
                                     // or is it at hash position
                                     // but the element is different
                                     // from the searched one (if
                                     // given) ?
  {
    if (keyCollisions[findKey] == 0) // is it not possible to find
    {                                // it by linear search?
#if _SC_LOCKDATA
      Unlock();
#endif
      return NULL;                   // give up!
    }
    do                               // linear search...
    {
      findKey = (findKey + 1) % tableSize;
                                     // look at next position
      if (counter > tableSize)       // make sure not to search
      {                              // endlessly
#if _SC_PROFILING
        table_ineff += counter;
        cerr << "Inefficient use of SCTable<T>::Find" << endl;
#endif

#if _SC_LOCKDATA
        Unlock();
#endif

        return NULL;                 // whole table searched!
      }
      counter++;
    } while (tableKeys[findKey] != key ||
             (findElem && *table[findKey] != *findElem));
  } // if

#if _SC_PROFILING
  table_ineff += counter;
#endif

  returnElem = table[findKey];

#if _SC_LOCKDATA
  Unlock();
#endif

  return returnElem;
}


template<class T> SCStream &operator<< (SCStream &pStream,
                                       const SCTable<T> &pData)
{
  SCNatural i;

  pStream << "[" << std::endl;
  for (i = 0;
       i < pData.NumOfElems();
       i++)
  {
    pStream << i << ": " << *(pData[i]) << std::endl;
  }
  pStream << "]" << std::endl;

  return pStream;
}


template<class T> SCStream& SCTable<T>::Display(SCStream& pStream) const
{
  return (pStream << *this);
}

#endif  // __SCTABLE_TMPL__
