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
| SCSensorFrequency |SCSensorFrequency.h| 14.01 1997    |   SCL             |   |
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

#ifndef __SCSENSORFREQUENCY__

#define __SCSENSORFREQUENCY__

#include "SCSensor.h"


class SCSensorFrequency : public SCSensor
{
  public:
                            SCSensorFrequency (const char *pName,
                                               const SCObject *father = NULL,
                                               const SCBoolean isBaseSensor = false,
                                               const SCSensorType pType = scSensorFrequency);
                           ~SCSensorFrequency (void);

    // Aendern des Sensors
    virtual void            Clear (void);
    void                    ClearLastValue (void);

    // Die Write-Methode schreibt bei allen relevanten
    // Sensor-Ereignissen die entsprechenden Daten
    // auf den Kommunikations-Socket des Schedulers.
    // Write ist immer gleich und muss definiert werden.

    virtual SCBoolean       Write (const SCSensorEvent event) const;

    // Abfragen des Sensors
    SCNatural               GetSumOfSamples (void) const      // Anzahl der Proben (gewichtet)
                                  { return sumSamples; }
    SCNatural               GetAbsFreq (const SCNatural value) const;  // Absolute Frequenz fuer value
    SCReal                  GetRelFreq (const SCNatural value) const;  // Relative Frequenz fuer value

    SCNatural               GetLastValue (void) const
                                  { return lastValue; };
    SCNatural               GetCurrentValue (void) const;
    SCNatural               GetLastSample (void) const
                                  { return lastSample; };

    // Ausgabe des Sensors
    friend SCStream &        operator<< (SCStream &pStream,
                                        const SCSensorFrequency &pData);
    virtual SCStream &       Display (SCStream &pStream) const;
    virtual SCStream &       DisplayValue (SCStream &pStream, SCNatural pValue) const;

    // Backtracking (Restore is defined in derived classes)
    SCBoolean               Save(SCMem&) const;
    void                    Size(SCSize *pSize) const;

  protected:
    SCNatural               minValue;        // don't save it because it
                                             // cannot change!
    SCNatural               maxValue;
    SCNatural               lastValue;
    SCNatural               lastSample;
    SCNatural *             frequencies;
    SCBoolean *             valueDisplayed;  // the visualizer gets
                                             // the value names only once
                                             // so we must set a flag to
                                             // indicate if a name of
                                             // a certain value was been
                                             // transmitted
                                             // over the socket; DON't SAVE!
    SCNatural               sumSamples;
    SCNatural               lastUpdateDepth; // depth of analysis at
                                             // which the sensor was
                                             // last updated; value
                                             // is used to check,
                                             // if values of lastValue
                                             // is 'fresh' !
};

#endif // __SCSENSORDYNFREQ__
