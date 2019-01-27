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
| SCRandUniform | SCRandUniform.h   | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCRANDUNIFORM__

#define __SCRANDUNIFORM__

#include "SCRandDouble.h"


class SCRandUniform : public SCRandDouble
{
  public:
                      SCRandUniform (SCReal low = 0.0,
                                     SCReal high = 0.0);
    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCReal low, SCReal high);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCReal            low;
    SCReal            range;
};

typedef SCRandUniform SCUniform;

#if !_SC_NOINLINES
  #include "SCRandUniform.inl.h"
#endif


#endif  //  __SCRANDUNIFORM__
