/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+----------------------+---------------+-------------------+
|       Module      |   File               |   Created     |   Project         |
+-------------------+----------------------+---------------+-------------------+
|SCSensorQueueLength|SCSensorQueueLength.cc| 01. Sep 1996  |   SCL             |
+-------------------+----------------------+---------------+-------------------+
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
#include "SCSensorQueueLength.h"
#include "SCSignalList.h"
#include "SCScheduler.h"
#include "SCSocket.h"
#include "SCDebug.h"
#include "SCSensor.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorQueueLength::SCSensorQueueLength (const char *pName,
                                          const SCObject *father,
                                          const SCDuration interval) :
  SCSensorDynFreq(pName, father, false, scSensorQueueLength),
  SCSensorTally(pName, interval, father, false, scSensorNone)
{
  if (SCSensorDynFreq::sensorType == scSensorQueueLength)
    Write(scSensorCreate);

  SCSensorQueueLength::Clear();
}


SCSensorQueueLength::~SCSensorQueueLength (void)
{
  if (SCSensorDynFreq::sensorType == scSensorQueueLength)
  {
    Write(scSensorDestroy);
  }
}


void SCSensorQueueLength::Update (const class SCObject *const queue,
                                  SCInteger changeValue)
{
  SCSignalSaveList * myQueue;

  myQueue = (SCSignalSaveList *)queue;

  // increment frequency counter for cuurent
  // queue length:
  SCSensorDynFreq::Update (+1, myQueue->NumOfElems());

  // update tally sensor:
  assert((SCInteger)myQueue->NumOfElems() - changeValue >= 0);
  SCSensorTally::Update (myQueue->NumOfElems() - changeValue,
                         SCScheduler::GetCurrentTime() - GetLastUpdateTime());

  if (SCSensorDynFreq::sensorType == scSensorQueueLength)
    Write(scSensorUpdate);
}


void SCSensorQueueLength::Clear (void)
{
  SCSensorDynFreq::Clear();
  SCSensorTally::Clear();

  if (SCSensorDynFreq::sensorType == scSensorQueueLength)
    Write(scSensorClear);
}


SCBoolean SCSensorQueueLength::Write (const SCSensorEvent event) const
{
  SCSocket *socket;
  SCBoolean result = true;
  SCNatural z_i;
  
  socket = SCScheduler::GetSocket();
  
  if (!socket || !socket->IsOpen())
    return true;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSensorQueueLength::Write(): started ";
#endif

  switch (event)
  {
    case scSensorCreate:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing create infos)" << endl;
#endif
      *socket << SCSensorDynFreq::sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      *socket << (SCNatural)SCSensorDynFreq::sensorType;
      *socket << SCSensorDynFreq::name;
      *socket << intervalLength;

      break;

    case scSensorDestroy:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing destroy infos)" << endl;
#endif
      *socket << SCSensorDynFreq::sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      break;

    case scSensorUpdate:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing update infos)" << endl;
#endif
      *socket << SCSensorDynFreq::sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      // 1. Frequency-Components:
      *socket << SCSensorDynFreq::sumSamples;
      *socket << minValue;
      *socket << maxValue;
      for (z_i = minValue; z_i <= maxValue; z_i++)
      {
        if (!valueDisplayed[z_i - minValue])
        {
          DisplayValue(*socket, z_i);
          valueDisplayed[z_i - minValue] = true;
        }
        *socket << GetAbsFreq(z_i);
        *socket << GetRelFreq(z_i);
      }
      *socket << lastValue;
      *socket << SCSensorDynFreq::lastSample;

      // 2. Tally-Components:
      *socket << numSamples;
      *socket << numInterval;
      *socket << minimum;
      *socket << maximum;
      *socket << SCSensorTally::sumSamples;
      *socket << sumInterval;
      *socket << sumQuadSamples;
      *socket << GetAverage();
      *socket << avgInterval;
      *socket << GetVariance();
      *socket << GetStdDev();
      *socket << GetVarCoeff();
      *socket << SCSensorTally::lastSample;

      break;

    case scSensorClear:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing clear infos)" << endl;
#endif
      *socket << SCSensorDynFreq::sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      break;
      
    default:
      assert(false);
      return false;
  }
  
#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSensorQueueLength::Write(): finished" << endl;
#endif

  return result;
}


SCStream& operator<< (SCStream& pStream, const SCSensorQueueLength& pData)
{
  if (pData.SCSensorDynFreq::sensorType == scSensorQueueLength)
  {
    pStream << "Queue length sensor '" << pData.SCSensorDynFreq::GetName();
    pStream << "'";
    if (pData.SCSensorDynFreq::GetParent())
    {
      pStream << " of " << *pData.SCSensorDynFreq::GetParent();
    }
    pStream << ":" << std::endl;
  }

  pData.SCSensorDynFreq::Display(pStream);
  pData.SCSensorTally::Display(pStream);

  return (pStream);
}


SCStream& SCSensorQueueLength::Display (SCStream &pStream) const
{
  return (pStream << *this);
}


SCBoolean SCSensorQueueLength::Save(SCMem& saveArea) const
{
  SCSensorDynFreq::Save(saveArea);
  SCSensorTally::Save(saveArea);

  return true;
}


SCBoolean SCSensorQueueLength::Restore(SCMem& saveArea)
{
  SCSensorDynFreq::Restore(saveArea);
  SCSensorTally::Restore(saveArea);

  if (SCSensorDynFreq::sensorType == scSensorQueueLength)
    Write(scSensorUpdate);

  return true;
}


void SCSensorQueueLength::Size(SCSize *curSize) const
{
  SCSensorDynFreq::Size(curSize);
  SCSensorTally::Size(curSize);
}
