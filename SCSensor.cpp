/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+-------------------+---------------+------------------+---+
|       Module      |   File            |   Created     |   Project        |   |
+-------------------+-------------------+---------------+------------------+---+
|    SCSensor       |   SCSensor.cc     | 01. Sep 1996  |   SCL            |   |
+-------------------+-------------------+---------------+------------------+---+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCSensor.h"
#include "SCMem.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorTable *SCSensor::sensorTable = NULL;
SCSensorList *SCSensor::sensorList = NULL;
SCNatural SCSensor::nextSensorID = 1;


SCSensor::SCSensor (const SCSensorType pType,
                    const char *pName,
                    const SCObject *father,
                    const SCBoolean isBaseSensor) :
  SCObject(SC_SENSOR, father),
  active(true),
  name(pName),
  sensorType(pType),
  baseSensor(isBaseSensor)
{
  // QueueLength-Sensors have two SCSensor base classes;
  // one of it gets scSensorNone to avoid double entry
  // in sensorTable and sensorList!

  if (sensorType != scSensorNone)
  {
    sensorID = nextSensorID++;

    assert(sensorTable);  // allocated in SCSensor::Initialize()
    sensorTable->Insert(sensorID, this);

    assert(sensorList);  // allocated in SCSensor::Initialize()
    container = sensorList->InsertAfter(this);
    assert(container);
  }
  else
  {
    sensorID = nextSensorID - 1; // use same value for both
                                 // base classes if using
                                 // multiple inheritance!
  }
}


SCSensor::~SCSensor (void)
{
  // QueueLength-Sensors have two SCSensor base classes;
  // one of it gets scSensorNone to avoid double entry
  // in sensorTable and sensorList!

  if (sensorType != scSensorNone)
  {
    assert(sensorTable);
    sensorTable->Remove(sensorID);

    assert(sensorList);
    sensorList->Remove(container);
  }
}


SCStream& operator<< (SCStream& pStream, const SCSensor& pData)
{
  return pData.Display(pStream);
}


SCStream& SCSensor::Display(SCStream& pStream) const
{
  assert(GetName());
  
  pStream << "<" << *GetName() << ">(#";
  pStream << GetID() << ")";

  return pStream;
}


void SCSensor::Size(SCSize *curSize) const
{
	curSize->historySize += sizeof (SCBoolean);  // active member
	curSize->historySize += sizeof (SCNatural);  // ID
}


SCBoolean SCSensor::Save(SCMem& saveArea) const
{
  saveArea.HistoryStore(&active, sizeof(SCBoolean));
  saveArea.HistoryStore(&sensorID, sizeof(SCNatural));

  return true;
}


SCBoolean SCSensor::Restore(SCMem& saveArea)
{
  saveArea.HistoryRestore(&active, sizeof(SCBoolean));
  saveArea.HistoryRestore(&sensorID, sizeof(SCNatural));

  return true;
}


SCBoolean SCSensor::StaticSave(SCMem &saveArea)
{
  saveArea.HistoryStore(&nextSensorID, sizeof(SCNatural));

  return true;
}


SCBoolean SCSensor::StaticRestore(SCMem &saveArea)
{
  saveArea.HistoryRestore(&nextSensorID, sizeof(SCNatural));

  return true;
}


void SCSensor::StaticSize(SCSize *curSize)
{
  curSize->historySize += sizeof(SCNatural);       // Member nextSensorID
}


SCBoolean SCSensor::Initialize (SCNatural sensorTableSize)
{
  // allocate table for all sensor instances

  sensorTable = new SCSensorTable(sensorTableSize, false);
  assert(sensorTable);

  sensorList = new SCSensorList(false);
  assert(sensorList);

  return true;
}


SCBoolean SCSensor::Finish (void) // static!
{
  // delete table of sensor instances

  if (sensorTable)
    delete sensorTable;

  if (sensorList)
    delete sensorList;

  return true;
}


SCStream & SCSensor::DisplayAll (SCStream &pStream) // static!
{
  SCSensorIter iter(*sensorList);
  SCSensor * sensor;

  if (!sensorList->IsEmpty())
  {

    pStream << "************************************" << std::endl;
    pStream << "* Sensor dump at time " << SCScheduler::GetCurrentTime();
    pStream << ":" << std::endl;
    pStream << "************************************" << std::endl << std::endl;

    for (sensor = iter++;
         sensor;
         sensor = iter++)
    {
       pStream << *sensor << std::endl;
    }
  }

  return pStream;
}


SCBoolean SCSensor::operator==(const SCSensor &second) const
{
  return (sensorID == second.sensorID);
}


SCBoolean SCSensor::operator!=(const SCSensor &second) const
{
  return (sensorID != second.sensorID);
}
