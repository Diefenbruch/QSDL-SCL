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
|  SCFullState   |  SCFullState.h     | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCFULLSTATE__

#define __SCFULLSTATE__

#include "SCBasicTypes.h"
#include "SCHashTable.h"
#include "SCMemTable.h"

class SCFullState: public SCHashTable
{
  public:
                      SCFullState (SCNatural hashTablePow = kSCHashTablePower,
                                   SCNatural hashFunctionIndex = kSCHashFunctionIndex,
                                   const SCObject * father = NULL);
                     ~SCFullState (void);

    SCBoolean         IsStateAnalyzed(class SCMem *state);
    
    friend SCStream &  operator<< (SCStream& pStream,
                                  const SCFullState& pData);
    virtual SCStream & Display (SCStream &pStream) const;

  private:
  
    SCMemTable *      hashTable;

    friend class SCIndetVal;
};


#endif  // __SCFULLSTATE__
