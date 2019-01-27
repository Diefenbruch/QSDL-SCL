/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+-------------------+--------------------+---------------+-----------------+---+
|       Module      |   File             |   Created     |   Project       |   |
+-------------------+--------------------+---------------+-----------------+---+
| SCSensorFrequency |SCSensorFrequency.cc| 14.01.1997    |   SCL           |   |
+-------------------+--------------------+---------------+-----------------+---+
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
#include "SCSensorFrequency.h"
#include "SCScheduler.h"
#include "SCSocket.h"
#include "SCDebug.h"
#include "SCMem.h"
#include "SCIndet.h"
#include "SCObject.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorFrequency::SCSensorFrequency (const char *pName,
                                      const SCObject *father,
                                      const SCBoolean isBaseSensor,
                                      const SCSensorType pType) :
  SCSensor (pType, pName, father, isBaseSensor),
  minValue(0),
  maxValue(0),
  lastValue(kSCNoValue),  // don't use 0 here because 0 is a valid
                          // sample value!
  lastSample(kSCNoSample),
  sumSamples(0),
  lastUpdateDepth(0)
{
  if (sensorType == scSensorFrequency)
    Write(scSensorCreate);
}


SCSensorFrequency::~SCSensorFrequency (void)
{
  if (sensorType == scSensorFrequency)
  {
    Write(scSensorDestroy);
  }
}


void SCSensorFrequency::Clear (void)
{
  // don't reset maxSample here!

  lastValue = ULONG_MAX;
  sumSamples = 0;

  if (sensorType == scSensorFrequency)
    Write(scSensorClear);
}


void SCSensorFrequency::ClearLastValue (void)
{
  // this function is used to invalidate the lastValue
  // data member, e.g. the last value SignalInFreq-Sensors
  // must be invalidated if an spontaneous transiton
  // or a continuous signal fires!

  lastValue = ULONG_MAX;
}


SCBoolean SCSensorFrequency::Write (const SCSensorEvent event) const
{
  SCSocket *socket;
  SCBoolean result = true;
  SCNatural z_i;
  
  socket = SCScheduler::GetSocket();
  
  if (!socket || !socket->IsOpen())
    return true;

#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "SCSensorFrequency::Write(): started ";
#endif

  switch (event)
  {
    case scSensorCreate:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing create infos)" << endl;
#endif

      *socket << sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      *socket << (SCNatural)sensorType;
      *socket << name;
      break;

    case scSensorDestroy:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing destroy infos)" << endl;
#endif
      *socket << sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      break;

    case scSensorUpdate:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing update infos)" << endl;
#endif
      *socket << sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      *socket << sumSamples;
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
      *socket << lastSample;
      break;

    case scSensorClear:
#if _SC_SOCKET_DEBUG
      scSocketDebugLog << "(writing clear infos)" << endl;
#endif
      *socket << sensorID;
      *socket << (SCNatural)event;
      *socket << SCScheduler::GetCurrentTime();

      break;
      
    default:
      assert(false);
      return false;
  }
  
#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSensorFrequency::Write(): finished" << endl;
#endif

  return result;
}


SCNatural SCSensorFrequency::GetAbsFreq (const SCNatural value) const
{
  return value <= maxValue ? frequencies[value - minValue] : 0;
}


SCReal SCSensorFrequency::GetRelFreq (const SCNatural value) const
{
  return (sumSamples && (value <= maxValue)) ?
          ((SCReal)frequencies[value - minValue] / (SCReal)sumSamples) :
          0.0;
}


SCNatural SCSensorFrequency::GetCurrentValue (void) const
{
  if (lastUpdateDepth >= SCScheduler::GetIndet()->CurrentDepth() - 1)
    return lastValue;
  else
    return kSCNoValue;
}


SCStream& operator<< (SCStream& pStream, const SCSensorFrequency& pData)
{
  if (pData.sensorType == scSensorFrequency)
  {
    pStream << "Frequency sensor '" << pData.GetName() << "'";
    if (pData.GetParent())
    {
      pStream << " of " << *pData.GetParent();
    }
    pStream << ":" << std::endl;
  }
  pStream << "  Max. Value:   ";
  pData.DisplayValue(pStream, pData.maxValue);
  pStream << std::endl;
  pStream << "  Last Value:   ";
  if (pData.GetLastValue() != kSCNoValue)
  {
    pData.DisplayValue(pStream, pData.GetLastValue());
    pStream << std::endl;
  }
  else
  {
    pStream << "none" << std::endl;
  }
  pStream << "  Last Sample:  ";
  if (pData.GetLastSample() != kSCNoSample)
  {
    pStream << pData.GetLastSample() << std::endl;
  }
  else
  {
    pStream << "none" << std::endl;
  }
  pStream << "  Sum Samples:  " << pData.GetSumOfSamples() << std::endl;
  pStream << "  Samples (value, abs, rel)" << std::endl;
  for (SCNatural z_i = pData.minValue; z_i <= pData.maxValue; z_i++)
  {
    if (pData.GetAbsFreq(z_i) > 0)
    {
      pStream << "    (";
      pData.DisplayValue(pStream, z_i);
      pStream << ", " << pData.GetAbsFreq(z_i) << ", ";
      pStream << pData.GetRelFreq(z_i) << ")" << std::endl;
    }
  }

  return (pStream);
}


SCStream& SCSensorFrequency::Display (SCStream &pStream) const
{
  return (pStream << *this);
}


SCStream& SCSensorFrequency::DisplayValue (SCStream &pStream, SCNatural pValue) const
{
  pStream << pValue;

  return pStream;
}


SCBoolean SCSensorFrequency::Save(SCMem& saveArea) const
{
  SCSensor::Save(saveArea);

  // don't save minValue because it connot change!
  saveArea.HistoryStore(&maxValue, sizeof(SCNatural));
  saveArea.HistoryStore(&lastValue, sizeof(SCNatural));
  saveArea.HistoryStore(&lastSample, sizeof(SCNatural));
  saveArea.HistoryStore(frequencies, (maxValue + 1) * sizeof(SCNatural));
  saveArea.HistoryStore(&sumSamples, sizeof(SCNatural));
  saveArea.HistoryStore(&lastUpdateDepth, sizeof(SCNatural));

  return true;
}


void SCSensorFrequency::Size(SCSize *curSize) const
{
  SCSensor::Size(curSize);

  curSize->historySize += (sizeof(SCNatural) +                   // Member maxSample
                           sizeof(SCNatural) +                   // Member lastValue
                           sizeof(SCNatural) +                   // Member lastSample
                           (maxValue + 1) * sizeof(SCNatural) +  // Member frequencies
                           sizeof(SCNatural) +                   // Member sumSamples
	                         sizeof (SCNatural));                  // Member lastUpdateDepth
}

