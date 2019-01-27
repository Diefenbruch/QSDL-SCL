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
|    SCSensor       |   SCSensor.h      | 01. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSOR__

#define __SCSENSOR__


#include "SCObject.h"
#include "SCSensorTable.h"
#include "SCSensorList.h"

typedef enum
{
  scSensorNone = 0,
  scSensorCounter = 1,
  scSensorFrequency = 2,
  scSensorTally = 3,
  scSensorQueueLength = 4
} SCSensorType;

typedef enum
{
  scSensorCreate = 1,
  scSensorDestroy = 2,
  scSensorUpdate = 3,
  scSensorClear = 4
} SCSensorEvent;

// Konstante fuer automatisches Sensorenupdate nach Zeit
#define kSCAutoUpdate  -2.0
#define kSCNoSample ULONG_MAX
#define kSCNoValue ULONG_MAX

class SCSensor : public SCObject
{
  public:
                      SCSensor (const SCSensorType pType,
                                const char *pName = NULL,
                                const SCObject *father = NULL,
                                const SCBoolean isBaseSensor = false);
    virtual          ~SCSensor (void);

    // Aktivierung; es hat keinen Einfluss auf einen Sensor,
    // ob er aktiv ist oder nicht.
    // Man kann das Flag und die Methoden als Markierung fuer
    // SensorUpdate ansehen.

    void              Activate (void) { active = true; }
    void              Deactivate (void) { active = false; }
    SCBoolean         IsActive (void) const { return active; }
    const char *      GetName (void) const { return name; }
    SCNatural         GetID (void) const { return sensorID; }
    SCBoolean         IsBaseSensor (void) const { return baseSensor; }
    
    // Clear ist immer gleich und muss definiert werden.
    // Es werden alle Werte initialisiert.

    virtual void      Clear (void) = 0;

    // Die Write-Methode schreibt bei allen relevanten
    // Sensor-Ereignissen die entsprechenden Daten
    // auf den Kommunikations-Socket des Schedulers.
    // Write ist immer gleich und muss definiert werden.

    virtual SCBoolean Write (const SCSensorEvent event) const = 0;

    friend SCStream &  operator<< (SCStream& pStream,
                                  const SCSensor& pData);
    virtual SCStream & Display (SCStream &pStream) const;

    SCBoolean         operator== (const SCSensor & second) const;
    SCBoolean         operator!= (const SCSensor & second) const;

    static SCStream & DisplayAll (SCStream &pStream);

    // Backtracking

    SCBoolean         Save(SCMem &saveArea) const;
    SCBoolean         Restore(SCMem &saveArea);
    void              Size(SCSize *pSize) const;
    static SCBoolean  StaticSave (SCMem &);
    static SCBoolean  StaticRestore (SCMem &);
    static void       StaticSize (SCSize *pSize);

    static SCBoolean  Initialize (SCNatural pTableSize);
    static SCBoolean  Finish (void);

  protected:
    SCBoolean         active;
    const char *      name;
    SCNatural         sensorID;          // ID of sensor
    SCSensorType      sensorType;
    SCBoolean         baseSensor;        // sensor is decalred
                                         // as BaseTally, BaseFrequency
                                         // or BaseCounter in specification
    SCSensorCons *    container;         // current SCListCons elem
                                         // in which sensor is
                                         // embedded (do not save!)
    static SCNatural  nextSensorID;
    static SCSensorTable * sensorTable;  // table of all existing sensors
    static SCSensorList * sensorList;    // list of all existing sensors
};

#endif // __SCSENSOR__
