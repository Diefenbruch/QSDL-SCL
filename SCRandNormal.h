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
| SCRandNormal  | SCRandNormal.h    | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCRANDNORMAL__

#define __SCRANDNORMAL__


#include "SCRandDouble.h"


class SCRandNormal : public SCRandDouble
{
  public:
                      SCRandNormal (SCReal mean = 0.0,
                                    SCReal stdDev = 0.0);

    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCReal mean, SCReal stdDev);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCReal            mean;
    SCReal            stdDev;
    SCReal            z;
};

typedef SCRandNormal SCNormal;

#if !_SC_NOINLINES
  #include "SCRandNormal.inl.h"
#endif

#endif  // __SCRANDNORMAL__
