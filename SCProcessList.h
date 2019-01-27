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
| SCProcessList | SCProcessList.h   | 10. Apr 1997  |   SCL             |       |
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

#ifndef __SCPROCESSLIST__

#define __SCPROCESSLIST__

#include "SCBasicTypes.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCList.h"

class SCProcess;

typedef SCListCons<SCProcess> SCProcessCons;
typedef SCListIter<SCProcess> SCProcessIter;

// This class is not derived from SCListSave!!!

class SCProcessList : public SCList<SCProcess>
{
  public:
                 SCProcessList (SCBoolean pDeleteElems = false,
                                const SCObject* father = NULL);
                ~SCProcessList (void);

    SCBoolean    Save(SCMem &) const;             // notwendig !
    SCBoolean    Restore(SCMem &);
    void         Size(SCSize *pSize) const;
};

#endif  //  __SCPROCESSLIST__
