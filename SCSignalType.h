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
| SCSignalType  |  SCSignalType.h   | 2. Apr 1995   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     02.04.95    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCSIGNALTYPE__

#define __SCSIGNALTYPE__


#include "SCType.h"
#include "SCSignalTypeTable.h"

class SCSignalType : public SCType
{
  public:
                               SCSignalType (SCNatural typeKey,
                                             const char *typeName,
                                             const SCObject* father = NULL);
                              ~SCSignalType (void);
                  
    static SCBoolean           Initialize (SCNatural tableSize);
    static SCBoolean           Finish (void);
    static SCSignalTypeTable * GetSignalTypeTable(void) { return signalTypeTable; } // table of all existing process!
    static void                SetSignalTypeTable(SCSignalTypeTable *newTable) { signalTypeTable = newTable; }

    virtual class SCDataType * NewData(void) const { return NULL; }
                                           // virtual method for creation
                                           // of signal parameters
    friend SCStream&            operator<< (SCStream &pStream,
                                           const SCSignalType &pData);

    SCBoolean                  operator== (const SCSignalType & second) const;
    SCBoolean                  operator!= (const SCSignalType & second) const;

    static SCSignalType *      GetSignalTypeFromName(const char * name);
                                                // for PEV!
  private:
    static SCSignalTypeTable  *signalTypeTable; // hash table of all existing
                                                // signal types

};


#endif  // __SCSIGNALTYPE__
