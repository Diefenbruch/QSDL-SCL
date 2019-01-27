/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 University of Essen, Germany                                   |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCObject    |   SCObject.h      | 24. Feb 1996  |   SCL             |      |
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


#ifndef __SCOBJECT__

#define __SCOBJECT__


#include "SCBasicTypes.h"

class SCObject
{
  public:
    virtual                 ~SCObject (void);

    SCObject *               GetParent (void) const;
                                              // Pointer to father.
    void                     SetParent (SCObject *pParent);

    SCObjectType             GetType (void) const { return type; }    

    virtual void             Size (SCSize *pSize) const;
                                              // Return the size of data
                                              // that is saved on the stack and
                                              // that is relevant for VALIDATION.
                                              // N.B. that the actual size of the
                                              // object may be much larger than
                                              // the value returned!!!

    virtual SCBoolean        Save (class SCMem &saveArea) const;
    virtual SCBoolean        Restore (class SCMem &saveArea);

    virtual class SCStream & Display (class SCStream &pStream) const = 0;
    friend SCStream &        operator<< (class SCStream& pStream,
                                        const SCObject& pData);
    
  protected:
                             SCObject(const SCObjectType = SC_NONE,
                                      const SCObject* = NULL);
                                              // Only derived classes
                                              // may use the constructor!

  private:
    SCObject *              parent;           // Pointer to parent object.
    SCObjectType            type;             // Type of this object.
};

#endif // __SCOBJECT__
