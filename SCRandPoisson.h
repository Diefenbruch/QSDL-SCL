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
| SCRandPoisson | SCRandPoisson.h   | 23. Mai 1994  |   SCL             |       |
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

#ifndef __SCRANDPOISSON__

#define __SCRANDPOISSON__


#include "SCRandLong.h"


class SCRandPoisson : public SCRandLong
{
  public:
                           SCRandPoisson (SCInteger mean = 0.0);

    virtual SCInteger      operator() (void);
    virtual void           Draw (SCReal &var);
    virtual void           Draw (SCInteger &var);
    void                   Init (SCInteger newMean);

    virtual SCStream &      Display (SCStream &pStream) const;

  private:
    SCInteger              mean;
};

typedef SCRandPoisson SCPoisson;

#if !_SC_NOINLINES
  #include "SCRandPoisson.inl.h"
#endif

#endif  // __SCRANDPOISSON__
