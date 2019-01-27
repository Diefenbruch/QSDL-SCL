/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+-------------------+-------------------+---------------+-------------------+---+
|       Module      |   File            |   Created     |   Project         |   |
+-------------------+-------------------+---------------+-------------------+---+
|  SCSensorDynFreq  | SCSensorDynFreq.h | 01. Sep 1996  |   SCL             |   |
+-------------------+-------------------+---------------+-------------------+---+
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

#ifndef __SCSENSORDYNFREQ__

#define __SCSENSORDYNFREQ__

#include "SCSensorFrequency.h"


class SCSensorDynFreq : public SCSensorFrequency
{
  public:
                            SCSensorDynFreq (const char *pName,
                                             const SCObject *father = NULL,
                                             const SCBoolean isBaseSensor = false,
                                             const SCSensorType pType = scSensorFrequency);
                                             
                           ~SCSensorDynFreq (void);
    // Aendern des Sensors
    void                    Update (SCInteger sample,        // sample (negative
                                                             // values are possible)
                                    const SCNatural value);  // for which value is the sample?
                                    
    virtual void            Clear (void);

    // Backtracking
    SCBoolean               Restore(SCMem&);
};

#endif // __SCSENSORDYNFREQ__
