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
|   SCDataType  |   SCDataType.h    | 23. Mai 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     23.05.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCDATATYPE__

#define __SCDATATYPE__

#include "SCBasicTypes.h"
#include "SCObject.h"

class SCDataType: public SCObject
{
  public:
                         SCDataType (const SCObject* father = NULL);
                         SCDataType (const SCDataType &orig,
                                     const SCObject* father = NULL);
    virtual             ~SCDataType (void);

    virtual SCDataType * Clone (void) const = 0;
    virtual SCBoolean    Equal (const SCDataType &second) const = 0;

    // We need additional HistorySize(), HistorySave()
    // and HistoryRestore() methods here, because we don't
    // now if a variable of this data type is declared
    // as history or not:

    virtual SCBoolean    HistorySave (SCMem &saveArea) const { (void)saveArea; return true; }
    virtual SCBoolean    HistoryRestore (SCMem &saveArea) { (void)saveArea; return true; }
    virtual void         HistorySize (SCSize *pSize) const { (void)pSize; }

    friend SCStream &     operator<< (SCStream& pStream,
                                     const SCDataType& pData);
};


#endif  // __SCDATATYPE__
