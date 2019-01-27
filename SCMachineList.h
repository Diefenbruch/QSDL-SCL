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
|SCMachineList | SCMachineList.h  | 16. Aug 1994  |   SCL             |       |
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

#ifndef __SCMACHINELIST__

#define __SCMACHINELIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCList.h"

class SCMachine;

typedef SCListCons<SCMachine> SCMachineCons;
typedef SCListIter<SCMachine> SCMachineIter;

// This class is not derived from SCListSave!!!

class SCMachineList : public SCList<SCMachine>
{
  public:
                 SCMachineList (SCBoolean pDeleteElems = false) :
                   SCList<SCMachine> (pDeleteElems) {};

    SCBoolean    Save(SCMem &) const;             // notwendig !
    SCBoolean    Restore(SCMem &);
    void         Size(SCSize *pSize) const;
};

#endif  //  __SCMACHINELIST__
