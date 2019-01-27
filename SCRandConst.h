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
| SCRandConst   |   SCRandConst.h   | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCRANDCONST__

#define __SCRANDCONST__


#include "SCRandDouble.h"


class SCRandConst : public SCRandDouble
{
  public:
                      SCRandConst (SCReal value = 0.0);

    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCReal newValue);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCReal            value;
};

typedef SCRandConst SCDeterministic;

#if !_SC_NOINLINES
  #include "SCRandConst.inl.h"
#endif


#endif  // __SCRANDCONST__
