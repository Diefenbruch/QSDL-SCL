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
|SCProcedureList| SCProcedureList.h | 16. Aug 1994  |   SCL             |       |
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

#ifndef __SCPROCEDURELIST__

#define __SCPROCEDURELIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCList.h"

class SCProcedure;

typedef SCListCons<SCProcedure> SCProcedureCons;
typedef SCListIter<SCProcedure> SCProcedureIter;

// This class is not derived from SCListSave!!!

class SCProcedureList : public SCList<SCProcedure>
{
  public:
                 SCProcedureList (SCBoolean pDeleteElems = false) :
                   SCList<SCProcedure> (pDeleteElems) {};

    SCBoolean    Save(SCMem &) const;             // notwendig !
    SCBoolean    Restore(SCMem &);
    void         Size(SCSize *pSize) const;
};

#endif  //  __SCPROCEDURELIST__
