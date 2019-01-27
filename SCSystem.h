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
| SCSystem      |  SCSystem.h       | 9. Aug 1996   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     09.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCSYSTEM__

#define __SCSYSTEM__

#include "SCBasicTypes.h"

class SCSystem
{
  public:
                      SCSystem (void);
    virtual          ~SCSystem (void);

    static void       SetSystem(SCSystem *newSystem) { theSystem = newSystem; }
    static SCSystem * GetSystem(void) { return theSystem; }
    
    virtual SCBoolean SystemSave(class SCMem &) { return true; }
    virtual SCBoolean SystemRestore(class SCMem &) { return true; }
    virtual void      SystemSize(SCSize *pSize) { (void)pSize; }

  private:
    static SCSystem * theSystem;
};

#endif  //  __SCSYSTEM__
