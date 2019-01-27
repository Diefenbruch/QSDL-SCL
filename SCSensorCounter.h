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
|  SCSensorCounter  | SCSensorCounter.h | 01. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORCOUNTER__

#define __SCSENSORCOUNTER__

#include "SCSensor.h"


class SCSensorCounter : public SCSensor
{
  public:
                            SCSensorCounter (const char *pName,
                                             const SCDuration interval = 1.0,
                                             const SCObject *father = NULL,
                                             const SCBoolean isBaseSensor = false,
                                             const SCSensorType = scSensorCounter);
                   virtual ~SCSensorCounter (void);

    // Aendern des Sensors
    void                    Update (SCInteger sample = 1);    // Increment counter
                                                              // neg. values are also
                                                              // possible
    virtual void            Clear (void);                     // reset sensor

    // Die Write-Methode schreibt bei allen relevanten
    // Sensor-Ereignissen die entsprechenden Daten
    // auf den Kommunikations-Socket des Schedulers.
    // Write ist immer gleich und muss definiert werden.

    virtual SCBoolean       Write (const SCSensorEvent event) const;

    // Abfragen des Sensors
    SCNatural               GetSumOfSamples (void) const           // Sum of all samples
                              { return counter; }
    SCNatural               GetSumInt (void) const        // Sum of all samples in
                              { return counterLastInterval; } // last interval
    SCReal                  GetSumPerTime (void) const;     // Samples per time
    SCReal                  GetSumPerTimeInt (void) const;  // Samples per time in
                                                              // last interval

    // Ausgabe des Sensors
    friend SCStream &        operator<< (SCStream &pStream,
                                        const SCSensorCounter &pData);
    SCStream &               Display (SCStream &pStream) const;

    // Backtracking
    SCBoolean               Save(SCMem&) const;
    SCBoolean               Restore(SCMem&);
    void                    Size(SCSize *curSize) const;

  private:
    SCNatural               counter;
    SCNatural               counterLastInterval;
    SCNatural               counterCurrentInterval;
    SCTime                  creationTime;
    SCTime                  intervalStop;
    const SCDuration        intervalLength;
};

#endif // __SCSENSORCOUNTER__
