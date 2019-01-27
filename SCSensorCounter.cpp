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
|  SCSensorCounter  |SCSensorCounter.cc | 01. Sep 1996  |   SCL            |   |
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
#include "SCSensorCounter.h"
#include "SCScheduler.h"
#include "SCSocket.h"
#include "SCDebug.h"
#include "SCMem.h"
#include "SCIndet.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorCounter::SCSensorCounter (const char *pName,
                                  const SCDuration interval,
                                  const SCObject *father,
                                  const SCBoolean isBaseSensor,
                                  const SCSensorType pType) :
  SCSensor (pType, pName, father, isBaseSensor),
  intervalLength (interval)
{
  if (sensorType == scSensorCounter)
    Write(scSensorCreate);

  SCSensorCounter::Clear();
}


SCSensorCounter::~SCSensorCounter (void)
{
  if (sensorType == scSensorCounter)
  {
    Write(scSensorDestroy);
  }
}


void SCSensorCounter::Clear (void)
{
  counter = 0;
  counterLastInterval = 0;
  counterCurrentInterval = 0;

  creationTime = SCScheduler::GetCurrentTime();
  intervalStop = SCScheduler::GetCurrentTime() + intervalLength;

  if (sensorType == scSensorCounter)
    Write(scSensorClear);
}


SCBoolean SCSensorCounter::Write (const SCSensorEvent event) const
{
  SCSocket *socket;
  SCBoolean result = true;
  
  socket = SCScheduler::GetSocket();
  
  if (!socket || !socket->IsOpen())
    return true;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSensorCounter::Write(): started ";
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

      *socket << counter;
      *socket << counterLastInterval;
      *socket << GetSumPerTime();
      *socket << GetSumPerTimeInt();

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
  scSocketDebugLog << "SCSensorCounter::Write(): finished" << endl;
#endif

  return result;
}


void SCSensorCounter::Update (SCInteger sample)
{
  counter += sample;

  while (SCScheduler::GetCurrentTime() >= intervalStop) // run over interval border?
  {
    counterLastInterval = counterCurrentInterval;  // store interval counter 
    counterCurrentInterval = 0;                    // reset intervall counter
    intervalStop += intervalLength;
  }
  counterCurrentInterval += sample;

  if (sensorType == scSensorCounter)
    Write(scSensorUpdate);
}


SCReal SCSensorCounter::GetSumPerTime (void) const
{
  SCTime now = SCScheduler::GetCurrentTime();

  return (now > creationTime ? (SCReal)counter / (now - creationTime) : 0.0);
}


SCReal SCSensorCounter::GetSumPerTimeInt (void) const
{
  
  return (intervalLength > 0 ? ((SCReal)counterLastInterval / intervalLength) : 0.0);
}


SCStream& operator<< (SCStream& pStream, const SCSensorCounter& pData)
{
  if (pData.sensorType == scSensorCounter)
  {
    pStream << "Counter sensor '" << pData.GetName() << "'";
    if (pData.GetParent())
    {
      pStream << " of " << *pData.GetParent();
    }
    pStream << ":" << std::endl;
  }
  pStream << "  Counter            : " << pData.GetSumOfSamples() << std::endl;
  pStream << "  Counter (Intv.)    : " << pData.GetSumInt() << std::endl;
  pStream << "  Count/Time         : " << pData.GetSumPerTime() << std::endl;
  pStream << "  Count/Time (Intv.) : " << pData.GetSumPerTimeInt() << std::endl;

  return (pStream);
}


SCStream& SCSensorCounter::Display (SCStream &pstream) const
{
  return (pstream << *this);
}


SCBoolean SCSensorCounter::Save(SCMem& saveArea) const
{
  SCSensor::Save(saveArea);

  saveArea.HistoryStore(&counter, sizeof(SCNatural));
  saveArea.HistoryStore(&counterLastInterval, sizeof(SCNatural));
  saveArea.HistoryStore(&counterCurrentInterval, sizeof(SCNatural));
  saveArea.HistoryStore(&creationTime, sizeof(SCTime));
  saveArea.HistoryStore(&intervalStop, sizeof(SCTime));
  saveArea.HistoryStore(&intervalLength, sizeof(SCDuration));

  return true;
}


SCBoolean SCSensorCounter::Restore(SCMem& saveArea)
{
  SCSensor::Restore(saveArea);

  saveArea.HistoryRestore(&counter, sizeof(SCNatural));
  saveArea.HistoryRestore(&counterLastInterval, sizeof(SCNatural));
  saveArea.HistoryRestore(&counterCurrentInterval, sizeof(SCNatural));
  saveArea.HistoryRestore(&creationTime, sizeof(SCTime));
  saveArea.HistoryRestore(&intervalStop, sizeof(SCTime));
  saveArea.HistoryRestore(&intervalLength, sizeof(SCDuration));

  if (sensorType == scSensorCounter)
    Write(scSensorUpdate);

  return true;
}


void SCSensorCounter::Size(SCSize *curSize) const
{
  SCSensor::Size(curSize);

  curSize->historySize += (sizeof(SCNatural) +   // Member counter
                           sizeof(SCNatural) +   // Member counterLastInterval
                           sizeof(SCNatural) +   // Member counterCurrentInterval
                           sizeof(SCTime) +      // Member creation
                           sizeof(SCTime) +      // Member intervalStop
                           sizeof(SCDuration));  // Member intervalLength
}
