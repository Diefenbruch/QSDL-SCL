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
| SCRandErlang  | SCRandErlang.h    | 23. Mai 1994  |   SCL             |       |
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

#ifndef __SCRANDERLANG__

#define __SCRANDERLANG__


#include "SCRandDouble.h"


class SCRandErlang : public SCRandDouble
{
  public:
                      SCRandErlang (SCReal mean = 1.0,
                                    SCReal stdDev = 1.0);

    virtual SCReal    operator() (void);
    virtual void      Draw (SCReal &var);
    virtual void      Draw (SCInteger &var);
    void              Init (SCReal newMean, SCReal newStdDev);

    virtual SCStream & Display (SCStream &pStream) const;

  private:
    SCReal            mean;
    SCReal            stdDev;
    SCInteger         k;
};

typedef SCRandErlang SCErlang;

#endif  // __SCRANDERLANG__
