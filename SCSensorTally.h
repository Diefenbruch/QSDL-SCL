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
|  SCSensorTally    |  SCSensorTally.h  | 01. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORTALLY__

#define __SCSENSORTALLY__

#include "SCSensor.h"

#define kSCTimeIntervalStart -1.0
#define kSCTimeIntervalEnd -2.0

class SCSensorTally : public SCSensor
{
  public:
                            SCSensorTally (const char *pName = NULL,
                                           const SCDuration interval = 1.0,
                                           const SCObject *father = NULL,
                                           const SCBoolean isBaseSensor = false,
                                           const SCSensorType pType = scSensorTally);
                   virtual ~SCSensorTally (void);

    // Aendern des Sensors
    void                    Update (SCReal sample, SCReal weight = 1.0);
    virtual void            Clear (void);

    // Die Write-Methode schreibt bei allen relevanten
    // Sensor-Ereignissen die entsprechenden Daten
    // auf den Kommunikations-Socket des Schedulers.
    // Write ist immer gleich und muss definiert werden.

    virtual SCBoolean       Write (const SCSensorEvent event) const;

    // Abfragen des Sensors
    SCReal                  GetLastSample (void) const          // Letzte Stichprobe 
                                  { return lastSample; }
    SCTime                  GetLastUpdateTime (void) const      // Zeitpunkt des letzten
                                  { return lastUpdate; }        // Updates
    SCReal                  GetNumOfSamples (void) const        // Anzahl der Proben (gewichtet)
                                  { return numSamples; }
    SCReal                  GetSumOfSamples (void) const        // Summe der Proben (gewichtet)
                                  { return sumSamples; }
    SCReal                  GetMinimum (void) const             // Minimum
                                  { return minimum; }
    SCReal                  GetMaximum (void) const             // Maximum
                                  { return maximum; }
    SCReal                  GetAverage (void) const;            // Durchschnitt (total)
    SCReal                  GetNumInt (void) const              // Anzahl Proben im aktuellen
                                  { return numInterval; }       // Zeitintervall (gewichtet)
    SCReal                  GetSumInt (void) const              // Summe der Proben im aktuellen
                                  { return sumInterval; }       // Zeitintervall
    SCReal                  GetAvgInt (void) const              // Durchschnitt der Proben
                                  { return avgInterval; }       // im aktuellen Intervall
    SCReal                  GetSumSquare (void) const           // Summe der Proben im Quadrat
                                  { return sumQuadSamples; }
    SCReal                  GetVariance (void) const;           // Varianz
    SCReal                  GetStdDev (void) const;             // Standardabweichung
    SCReal                  GetVarCoeff (void) const;

    // Ausgabe des Sensors
    friend SCStream &        operator<< (SCStream &pStream,
                                        const SCSensorTally &pData);
    virtual SCStream &       Display (SCStream &pStream) const;

    // Backtracking
    SCBoolean               Save(SCMem&) const;
    SCBoolean               Restore(SCMem&);
    void                    Size(SCSize *pSize) const;

  protected:
    SCReal                  lastSample;
    SCTime                  lastUpdate;
    SCReal                  numSamples;   // This is a real value because it
                                          // is updated with a real value weight.
                                          // Usually the weight is 1.0 (default)
                                          // but sometimes the weight is used
                                          // as a time intervall since the last
                                          // update (e.g. Utilization). In this case
                                          // numSamples holds the lifetime of the
                                          // sensor
    SCReal                  sumSamples;
    SCReal                  sumQuadSamples;
    SCReal                  minimum;
    SCReal                  maximum;
    SCReal                  numInterval;
    SCReal                  sumInterval;
    SCReal                  avgInterval;
    const SCDuration        intervalLength;
    SCTime                  intervalStop;
};


#endif // __SCSENSORTALLY__
