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
| SCType        |  SCType.h         | 2. Apr 1995   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     02.04.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#ifndef __SCTYPE__

#define __SCTYPE__

#include "SCBasicTypes.h"
#include "SCObject.h"

class SCType: public SCObject
{
  public:
                          SCType (SCNatural typeID,
                                  const char *typeName,
                                  const SCObject* father = NULL);
    virtual              ~SCType (void);

    SCNatural             GetID (void) const;
    const char *          GetName (void) const;

    static const char *   GetObjectName(SCObjectType objectType,
                                        SCNatural id);
    
    friend SCStream&      operator<< (SCStream &pStream,
                                      const SCType &pData);
    virtual SCStream&     Display (SCStream& pStream) const;

  private:
    SCNatural             typeID;
    const char *          typeName;
};

#if !_SC_NOINLINES
  #include "SCType.inl.h"
#endif

#endif  // __SCTYPE__
