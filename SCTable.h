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
|   SCTable     |   SCTable.h       | 24. Feb 1996  |   SCL             |      |
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

#ifndef __SCTABLE__

#define __SCTABLE__

#include "SCBasicTypes.h"

#if _SC_LOCKDATA
  #include "SCMutexTypes.h"
#endif

#include "SCObject.h"

#define _SC_TABLE_RESIZE_AMOUNT 1000

template<class T> class SCTable: public SCObject
#if _SC_LOCKDATA
, public SCMutex
#endif
{
  public:
                        SCTable (SCNatural size = kSCDefaultTableSize,
                                 SCBoolean deleteElems = true,
                                 SCBoolean reslvColl = true,
                                 SCBoolean resize = false,
                                 const SCObject* father = NULL);
      virtual          ~SCTable (void);

      SCBoolean         Insert (SCInteger key, T* insertElem);
      T *               Remove (SCInteger key, const T * removeElem = NULL);
      T *               Find (SCInteger key, const T * findElem = NULL) const;
      SCNatural         NumOfElems(void) const { return numOfElems; }
      SCBoolean         IsEmpty(void) const { return (numOfElems == 0); }
      SCBoolean         IsFull(void) const { return (numOfElems == tableSize); }
      SCNatural         GetTableSize(void) { return tableSize; }
            
      T *               operator[] (SCNatural index) const {return table[index]; }

      virtual SCStream & Display (SCStream &pStream) const;

  protected:
      SCBoolean         Initialize(void);
      SCBoolean         Resize(void);

      T **              table;       // array of table elems
      SCInteger *       tableKeys;   // array of table keys
      SCNatural *       keyCollisions; // array of counters (speedup for Find())
      SCNatural         tableSize;   // max num of elems
      SCNatural         numOfElems;  // current num of elems
      SCBoolean         deleteElems; // should destructor delete
                                     // contents of elems?
      SCBoolean         resolveCollisions; // is collision detection
                                           // needed? Default is true,
                                           // currently only set to false
                                           // for cycle detection hash
                                           // table
      SCBoolean         resizeIfFull; // enlarge the table if all
                                      // slots are in use
};

template<class T> class SCStream &  operator<< (class SCStream &pStream, const SCTable<T> &pData);

#ifndef __GNUC__
  #include "SCTable.tmpl.h"      // necessary for CC compiler
#endif

#endif  //  __SCTABLE__
