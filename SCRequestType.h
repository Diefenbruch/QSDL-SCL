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
| SCRequestType |  SCRequestType.h  | 2. Apr 1995   |   SCL             |       |
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

#ifndef __SCREQUESTTYPE__

#define __SCREQUESTTYPE__


#include "SCType.h"
#include "SCRequestTypeTable.h"

class SCRequestType : public SCType
{
  public:
                                SCRequestType (SCNatural typeKey,
                                               const char *typeName = NULL,
                                               const SCObject* father = NULL);

                               ~SCRequestType (void);

    static SCBoolean            Initialize (SCNatural tableSize);
    static SCBoolean            Finish (void);
    static SCRequestTypeTable * GetRequestTypeTable(void) { return requestTypeTable; } // table of all existing process!
    static void                 SetRequestTypeTable(SCRequestTypeTable *newTable) { requestTypeTable = newTable; }

    virtual class SCDataType *  NewData(void) const { return NULL; }
                                           // virtual method for creation
                                           // of request parameters
    friend SCStream&            operator<< (SCStream &pStream,
                                            const SCRequestType &pData);
    SCBoolean                   operator== (const SCRequestType & second) const;
    SCBoolean                   operator!= (const SCRequestType & second) const;

    static SCRequestType *      GetRequestTypeFromName(const char * name);
                                                // for PEV!
  private:
    static SCRequestTypeTable  *requestTypeTable; // hash table of all existing
                                                  // request types
};


#endif  // __SCREQUESTTYPE__
