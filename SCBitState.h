/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|  SCBitState   |  SCBitState.h     | 25. Apr 1996  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     --.--.--    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCBITSTATE__

#define __SCBITSTATE__

#include "SCBasicTypes.h"
#include "SCHashTable.h"

class SCBitState: public SCHashTable
{
  public:
                      SCBitState (SCNatural hashTablePow = kSCHashTablePower,
                                  SCNatural hashFunctionIndex = kSCHashFunctionIndex,
                                  const SCObject * father = NULL);
                     ~SCBitState (void);

    SCBoolean         IsStateAnalyzed(class SCMem *state);
    
    friend SCStream &  operator<< (SCStream& pStream,
                                  const SCBitState& pData);
    virtual SCStream & Display (SCStream &pStream) const;

  private:
  
    SCCharacter *    hashTable;

    friend class SCIndetVal;
};


#endif  // __SCBITSTATE__
