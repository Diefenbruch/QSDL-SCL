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
|  SCHashTable  |  SCHashTable.h    | 19. Nov 1997  |   SCL             |      |
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


#ifndef __SCHASHTABLE__

#define __SCHASHTABLE__

#include "SCBasicTypes.h"
#include "SCStateSpace.h"


class SCHashTable: public SCStateSpace
{
  public:
    typedef  SCNatural (SCHashTable::*SCHashFunction) (const class SCMem *state);

             SCHashTable (const SCStateSpaceType pType,
                          SCNatural hashTablePow = kSCHashTablePower,
                          SCNatural hashFunctionIndex = kSCHashFunctionIndex,
                          const SCObject * father = NULL);
    virtual ~SCHashTable (void);

    SCNatural         GetNumOfAnalyzedStates(void) const;
    SCNatural         GetStateSpaceSize(void) const;
    SCNatural         GetTablePower(void) const;
    SCNatural         GetFunctionIndex(void) const;
    SCInteger         GetCurrentKey(void) const;

    friend SCStream &  operator<< (SCStream& pStream,
                                   const SCHashTable & pData);
    virtual SCStream & Display (SCStream &pStream) const;

  protected:
  
    // Hashfuntions:
    SCNatural         Holzmann1(const class SCMem *); // don't declare as const
    SCNatural         Holzmann2(const class SCMem *); // to aviod warnings when
    SCNatural         Multiply(const class SCMem *);  // assigned to non-const
    SCNatural         Modulo(const class SCMem *);    // hashFunction variable

    SCNatural         hashTablePower;
    SCNatural         hashTableSize;
    SCNatural         hashFunctionIndex;
    SCHashFunction    hashFunction;
    SCNatural         analyzedStates;
    SCNatural         hashMask;              // Bit-mask for hashfunction #1.

  private:
    void              CalcBasicHashKey (const class SCMem *state);

    SCInteger         m;                     // first stage hash key
    int               h;                     // size off current state in bytes
    long              z;                     // magic value for hash functions
    long *            q;                     // current byte quadruple
    long double       phi;                   // used for multiply hash methods
    static SCNatural  primes[];              // Array of prime numbers for
                                             // hash function #5.

};

#if !_SC_NOINLINES
  #include "SCHashTable.inl.h"
#endif

#endif  // __SCSTATESPACE__
