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
|  SCSensorQueue    |  SCSensorQueue.h  | 01. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORQUEUELENGTH__

#define __SCSENSORQUEUELENGTH__

#include "SCSensorDynFreq.h"
#include "SCSensorTally.h"

class SCSensorQueueLength : public SCSensorDynFreq, public SCSensorTally
{
  public:
                            SCSensorQueueLength (const char *pName,
                                                 const SCObject *father,
                                                 const SCDuration interval = 1.0);
                   virtual ~SCSensorQueueLength (void);

    // Aendern des Sensors
    void                    Update (const class SCObject *const queue,
                                    SCInteger changeValue);
    virtual void            Clear (void);

    // Die Write-Methode schreibt bei allen relevanten
    // Sensor-Ereignissen die entsprechenden Daten
    // auf den Kommunikations-Socket des Schedulers.
    // Write ist immer gleich und muss definiert werden.

    virtual SCBoolean       Write (const SCSensorEvent event) const;

    // Abfragen des Sensors
    // geschieht ueber Basisklassen

    // Ausgabe des Sensors
    friend SCStream &        operator<< (SCStream &pStream,
                                        const SCSensorQueueLength &pData);
    virtual SCStream &       Display (SCStream &pStream) const;

    // Backtracking
    SCBoolean               Save(SCMem&) const;
    SCBoolean               Restore(SCMem&);
    void                    Size(SCSize *pSize) const;
};


#endif // __SCSENSORQUEUELENGTH__
