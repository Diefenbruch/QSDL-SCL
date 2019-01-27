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
|  SCSensorTally    |  SCSensorTally.cc | 01. Sep 1996  |   SCL            |   |
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


#include <math.h>

#include "SCStream.h"
#include "SCSensorTally.h"
#include "SCScheduler.h"
#include "SCSocket.h"
#include "SCDebug.h"
#include "SCMem.h"
#include "SCIndet.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorTally::SCSensorTally (const char *pName,
                              const SCDuration interval,
                              const SCObject *father,
                              const SCBoolean isBaseSensor,
                              const SCSensorType pType) :
  SCSensor (pType, pName, father, isBaseSensor),
  intervalLength (interval)
{
  if (sensorType == scSensorTally)
    Write(scSensorCreate);
  
  SCSensorTally::Clear();
}


SCSensorTally::~SCSensorTally (void)
{
  if (sensorType == scSensorTally)
  {
    Write(scSensorDestroy);
  }
}


void SCSensorTally::Clear (void)
{
  lastSample = 0.0;
  lastUpdate = SCScheduler::GetCurrentTime();
  numSamples = 0.0;
  sumSamples = 0.0;
  sumQuadSamples = 0.0;
  minimum = 0.0;
  maximum = 0.0;
  avgInterval = 0.0;
  numInterval = 0.0;
  sumInterval = 0.0;
  intervalStop = SCScheduler::GetCurrentTime() + intervalLength;

  if (sensorType == scSensorTally)
    Write(scSensorClear);
}


SCBoolean SCSensorTally::Write (const SCSensorEvent event) const
{
  SCSocket *socket;
  SCBoolean result = true;
  
  socket = SCScheduler::GetSocket();
  
  if (!socket || !socket->IsOpen())
    return true;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSensorTally::Write(): started ";
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
      assert(GetName());
      *socket << GetName();
      *socket << intervalLength;
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

      *socket << numSamples;
      *socket << numInterval;
      *socket << minimum;
      *socket << maximum;
      *socket << sumSamples;
      *socket << sumInterval;
      *socket << sumQuadSamples;
      *socket << GetAverage();
      *socket << avgInterval;
      *socket << GetVariance();
      *socket << GetStdDev();
      *socket << GetVarCoeff();
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
  scSocketDebugLog << "SCSensorTally::Write(): finished" << endl;
#endif

  return result;
}


void SCSensorTally::Update (SCReal sample, SCReal weight)
{
  // we have two special weight values
  // for measuring time intervals between
  // two Update calls:
  // 1. kSCTimeIntervalStart:
  //    only update lastUpdate member
  //    to store interval start
  // 2. kSCTimeIntervalStop:
  //    set sample to interval length
  // in both case the given parameter sample
  // is obsolete

  if (weight == kSCTimeIntervalEnd)
  {
    sample = SCScheduler::GetCurrentTime() - lastUpdate;
    weight = 1.0; // set to default
  }

  lastUpdate = SCScheduler::GetCurrentTime();

  if (weight == kSCTimeIntervalStart)
    return;

  lastSample = sample;
  
  if (numSamples == 0.0)
  {
    minimum = sample;
    maximum = sample;
  }
  else
  {
    if (sample < minimum)
      minimum = sample;
    else if (sample > maximum)
      maximum = sample;
  }
  numSamples += weight;  // in most cases weight is 1.0,
                         // if not weight is a time interval
                         // giving the time the value
                         // sample was valid (e.g. Utilization
                         // sensor)
  sample *= weight;      // now join both values to one
  sumSamples += sample;
  sumQuadSamples += sample * sample;

  while (SCScheduler::GetCurrentTime() >= intervalStop)
  {
    avgInterval = numInterval > 0.0 ? (sumInterval / numInterval) : 0.0;
    numInterval = 0.0;
    sumInterval = 0.0;
    intervalStop += intervalLength;
  }
  sumInterval += sample;
  numInterval += weight;

  Write(scSensorUpdate);
}


SCReal SCSensorTally::GetAverage (void) const
{
  return numSamples ? (sumSamples / numSamples) : 0.0;
}


SCReal SCSensorTally::GetVariance (void) const
{
  // negative values for variance _are_ possible if
  // the tally sensor is updated with weights != 1,
  // i.e. the tally sensor is used in intervall mode

  return numSamples > 1 ? ((numSamples * sumQuadSamples - sumSamples * sumSamples) /
                           (numSamples * (numSamples - 1))) :
                          0.0;
}


SCReal SCSensorTally::GetStdDev (void) const
{
  SCReal variance = GetVariance();

  return (numSamples && (variance > 0.0)) ? sqrt (variance) : 0.0;
}


SCReal SCSensorTally::GetVarCoeff (void) const
{
  SCReal average = GetAverage();

  return (average > 0.0) ? (GetStdDev() / average) : 0.0;
}


SCStream& operator<< (SCStream& pStream, const SCSensorTally& pData)
{
  if (pData.sensorType == scSensorTally)
  {
    pStream << "Tally sensor '" << pData.GetName() << "'";
    if (pData.GetParent())
    {
      pStream << " of " << *pData.GetParent();
    }
    pStream << ":" << std::endl;
  }
  pStream << "  Last Sample  : " << pData.GetLastSample() << std::endl;
  pStream << "  Last Time    : " << pData.GetLastUpdateTime() << std::endl;
  pStream << "  Num Samples  : " << pData.GetNumOfSamples() << std::endl;
  pStream << "  Sum Samples  : " << pData.GetSumOfSamples() << std::endl;
  pStream << "  Minimum      : " << pData.GetMinimum() << std::endl;
  pStream << "  Maximum      : " << pData.GetMaximum() << std::endl;
  pStream << "  Average      : " << pData.GetAverage() << std::endl;
  pStream << "  Num (Intv.)  : " << pData.GetNumInt() << std::endl;
  pStream << "  Sum (Intv.)  : " << pData.GetSumInt() << std::endl;
  pStream << "  Avg (Intv.)  : " << pData.GetAvgInt() << std::endl;
  pStream << "  Sum Squares  : " << pData.GetSumSquare() << std::endl;
  pStream << "  Variance     : " << pData.GetVariance() << std::endl;
  pStream << "  Std.Dev.     : " << pData.GetStdDev() << std::endl;
  pStream << "  Var.Coeff.   : " << pData.GetVarCoeff() << std::endl;

  return (pStream);
}


SCStream& SCSensorTally::Display (SCStream &pstream) const
{
  return (pstream << *this);
}


SCBoolean SCSensorTally::Save(SCMem& saveArea) const
{
  SCSensor::Save(saveArea);

  saveArea.HistoryStore(&lastSample, sizeof(SCReal));
  saveArea.HistoryStore(&lastUpdate, sizeof(SCTime));
  saveArea.HistoryStore(&numSamples, sizeof(SCReal));
  saveArea.HistoryStore(&sumSamples, sizeof(SCReal));
  saveArea.HistoryStore(&sumQuadSamples, sizeof(SCReal));
  saveArea.HistoryStore(&minimum, sizeof(SCReal));
  saveArea.HistoryStore(&maximum, sizeof(SCReal));
  saveArea.HistoryStore(&numInterval, sizeof(SCReal));
  saveArea.HistoryStore(&sumInterval, sizeof(SCReal));
  saveArea.HistoryStore(&avgInterval, sizeof(SCReal));
  saveArea.HistoryStore(&intervalLength, sizeof(SCDuration));
  saveArea.HistoryStore(&intervalStop, sizeof(SCTime));

  return true;
}


SCBoolean SCSensorTally::Restore(SCMem& saveArea)
{
  SCSensor::Restore(saveArea);

  saveArea.HistoryRestore(&lastSample, sizeof(SCReal));
  saveArea.HistoryRestore(&lastUpdate, sizeof(SCTime));
  saveArea.HistoryRestore(&numSamples, sizeof(SCReal));
  saveArea.HistoryRestore(&sumSamples, sizeof(SCReal));
  saveArea.HistoryRestore(&sumQuadSamples, sizeof(SCReal));
  saveArea.HistoryRestore(&minimum, sizeof(SCReal));
  saveArea.HistoryRestore(&maximum, sizeof(SCReal));
  saveArea.HistoryRestore(&numInterval, sizeof(SCReal));
  saveArea.HistoryRestore(&sumInterval, sizeof(SCReal));
  saveArea.HistoryRestore(&avgInterval, sizeof(SCReal));
  saveArea.HistoryRestore(&intervalLength, sizeof(SCDuration));
  saveArea.HistoryRestore(&intervalStop, sizeof(SCTime));

  if (sensorType == scSensorTally)
    Write(scSensorUpdate);

  return true;
}


void SCSensorTally::Size(SCSize *curSize) const
{
  SCSensor::Size(curSize);

  curSize->historySize += (sizeof(SCReal) +       // Member lastSample
                           sizeof(SCTime) +       // Member lastUpdate
                           sizeof(SCReal) +       // Member numSamples
                           sizeof(SCReal) +       // Member sumSamples
                           sizeof(SCReal) +       // Member sumQuadSamples
                           sizeof(SCReal) +       // Member minimum
                           sizeof(SCReal) +       // Member maximum
                           sizeof(SCReal) +       // Member numInterval
                           sizeof(SCReal) +       // Member sumInterval
                           sizeof(SCReal) +       // Member avgInterval
                           sizeof(SCDuration) +   // Member intervalLength
                           sizeof(SCTime));       // Member intervalStop
}
