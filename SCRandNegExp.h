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
| SCRandNegExp  | SCRandNegExp.h    | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCRANDNEGEXP__

#define __SCRANDNEGEXP__


#include "SCRandDouble.h"

#include <float.h>

class SCRandNegExp : public SCRandDouble
{
  public:
                      SCRandNegExp (SCReal rate = DBL_MAX);

    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCReal newRate);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCReal            rate;
};

typedef SCRandNegExp SCNegexp;

#if !_SC_NOINLINES
  #include "SCRandNegExp.inl.h"
#endif


#endif  // __SCRANDNEGEXP__
