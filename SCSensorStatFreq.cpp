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
|  SCSensorStatFreq |SCSensorStatFreq.cc| 01. Sep 1996  |   SCL            |   |
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

#include "SCSensorStatFreq.h"

#include "SCScheduler.h"
#include "SCMem.h"
#include "SCIndet.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorStatFreq::SCSensorStatFreq (const char *pName,
                                    const SCNatural pMinValue,
                                    const SCNatural pMaxValue,
                                    const SCObject *father,
                                    const SCBoolean pIsBaseSensor,
                                    const SCSensorType pType) :
  SCSensorFrequency(pName, father, pIsBaseSensor, pType)
{
  minValue = pMinValue;
  maxValue = pMaxValue;

  frequencies = new SCNatural[(maxValue - minValue) + 1];
  assert(frequencies);

  valueDisplayed = new SCBoolean[(maxValue - minValue) + 1];
  assert(valueDisplayed);
  
  for (SCNatural z_i = minValue; z_i <= maxValue; z_i++)
    valueDisplayed[z_i - minValue] = false;

  SCSensorStatFreq::Clear();
}


SCSensorStatFreq::~SCSensorStatFreq (void)
{
  if (frequencies)
    delete[] frequencies;
  if (valueDisplayed)
    delete[] valueDisplayed;
}


void SCSensorStatFreq::Clear (void)
{
  SCSensorFrequency::Clear();

  for (SCNatural z_i = minValue; z_i <= maxValue; z_i++)
    frequencies[z_i - minValue] = 0;
}


void SCSensorStatFreq::Update (SCInteger sample, const SCNatural value)
{
  assert(value <= maxValue);
  
  lastValue = value;
  lastSample = sample;
  lastUpdateDepth = SCScheduler::GetIndet()->CurrentDepth();

  if (value <= maxValue)
  {
    frequencies[value - minValue] += sample;
    sumSamples += sample;
  }
}


SCBoolean SCSensorStatFreq::Restore(SCMem& saveArea)
{
  SCSensor::Restore(saveArea);

//  saveArea.HistoryRestore(&minValue, sizeof(SCNatural));
  saveArea.HistoryRestore(&maxValue, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastValue, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastSample, sizeof(SCNatural));
  saveArea.HistoryRestore(frequencies, (maxValue + 1) * sizeof(SCNatural));
  saveArea.HistoryRestore(&sumSamples, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastUpdateDepth, sizeof(SCNatural));

  if (sensorType == scSensorFrequency)
    Write(scSensorUpdate);

  return true;
}
