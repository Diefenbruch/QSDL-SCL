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
|SCRandEmpirical| SCRandEmpirical.h | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCRANDEMPIRICAL__

#define __SCRANDEMPIRICAL__


#include "SCRandUniform.h"

typedef char *SCFileSpec;


class SCRandEmpirical : public SCRandDouble
{
  public:
                      SCRandEmpirical (SCInteger numVals = 0,
                                       SCFileSpec fileName = (char *)0L);
    virtual          ~SCRandEmpirical (void);

    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCInteger numVals, SCFileSpec fileName);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCInteger         numVals;
    SCReal            numValsReal;
    SCReal *          values;
};

typedef SCRandEmpirical SCEmpirical;

#endif  // __SCRANDEMPIRICAL__
