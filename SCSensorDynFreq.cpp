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
|  SCSensorDynFreq  | SCSensorDynFreq.cc| 01. Sep 1996  |   SCL            |   |
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

#include "SCSensorDynFreq.h"
#include "SCScheduler.h"
#include "SCDebug.h"
#include "SCMem.h"
#include "SCIndet.h"

#if _SC_VALIDATION_OPTIMIZE
  #include "SCRunnable.h"     // for size adjustment
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSensorDynFreq::SCSensorDynFreq (const char *pName,
                                  const SCObject *father,
                                  const SCBoolean pIsBaseSensor,
                                  const SCSensorType pType) :
  SCSensorFrequency (pName, father, pIsBaseSensor, pType)
{
  // don't call Clear here to avoid size adjustment

  frequencies = new SCNatural[1];
  assert(frequencies);

  frequencies[0] = 0;

  valueDisplayed = new SCBoolean[1];
  assert(valueDisplayed);
  
  valueDisplayed[0] = false;
}


SCSensorDynFreq::~SCSensorDynFreq (void)
{
  if (frequencies)
    delete[] frequencies;
  if (valueDisplayed)
    delete[] valueDisplayed;
}


void SCSensorDynFreq::Clear (void)
{
#if _SC_VALIDATION_OPTIMIZE
  SCRunnable *runnable;

  if (GetParent()) // this is true for machine and process sensors
  {                // but not for system sensors
    runnable = (SCRunnable *)GetParent();
    assert(runnable);

    runnable->currentHistorySize -= (sizeof(SCNatural) * maxValue);
  }
#endif    

  SCSensorFrequency::Clear();

  maxValue = 0;

  if (frequencies)
    delete[] frequencies;

  frequencies = new SCNatural[1];
  assert(frequencies);

  frequencies[0] = 0;
}


void SCSensorDynFreq::Update (SCInteger sample, const SCNatural value)
{
  if (value > maxValue)
  {
    SCNatural *  oldFreq = frequencies;
    SCBoolean *  oldValueDisplayed = valueDisplayed;
    SCNatural    z_i;
#if _SC_VALIDATION_OPTIMIZE
    SCRunnable * runnable;
#endif

    frequencies = new SCNatural[value + 1];
    assert(frequencies);

    // copy old frequencies:
    for (z_i = 0; z_i <= maxValue; z_i++)
      frequencies[z_i] = oldFreq[z_i];

    // clear new frequencies:
    for (; z_i <= value; z_i++)
      frequencies[z_i] = 0;

    valueDisplayed = new SCBoolean[value + 1];
    assert(valueDisplayed);

    // copy old valueDisplayed:
    for (z_i = 0; z_i <= maxValue; z_i++)
      valueDisplayed[z_i] = oldValueDisplayed[z_i];

    // clear new frequencies:
    for (; z_i <= value; z_i++)
      valueDisplayed[z_i] = false;


#if _SC_VALIDATION_OPTIMIZE
    if (GetParent()) // this is true for machine and process sensors
    {                // but not for system sensors
      runnable = (SCRunnable *)GetParent();
      assert(runnable);

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << "SCSensorDynFreq::Update(): updating history size of runnable " << *runnable;
      scValidationDebugLog << " from " << runnable->currentHistorySize;
#endif

      runnable->currentHistorySize += (sizeof(SCNatural) * (value - maxValue));

#if _SC_VALIDATION_DEBUG >= 3
      scValidationDebugLog << " to " << runnable->currentHistorySize << endl;
#endif

    }
#endif    

    maxValue = value;
    delete[] oldFreq;
    delete[] oldValueDisplayed;
  } // if

  lastValue = value;
  lastSample = sample;
  lastUpdateDepth = SCScheduler::GetIndet()->CurrentDepth();
  frequencies[value] += sample;
  sumSamples += sample;

  if (sensorType == scSensorFrequency)
    Write(scSensorUpdate);
}


SCBoolean SCSensorDynFreq::Restore(SCMem& saveArea)
{
  SCSensor::Restore(saveArea);
#if _SC_VALIDATION_OPTIMIZE
    SCRunnable *runnable = NULL;
#endif

  if (frequencies)
  {
    delete[] frequencies;

#if _SC_VALIDATION_OPTIMIZE
    // the runnable size must be adjusted:
    if (GetParent()) // this is true for machine and process sensors
    {                // but not for system sensors
      runnable = (SCRunnable *)GetParent();
      assert(runnable);

      runnable->currentHistorySize -= (sizeof(SCNatural) * maxValue);
    }
#endif
  }

  saveArea.HistoryRestore(&maxValue, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastValue, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastSample, sizeof(SCNatural));

  frequencies = new SCNatural[maxValue + 1];
  assert(frequencies);

#if _SC_VALIDATION_OPTIMIZE
  // the runnable size must be adjusted:
  if (runnable)    // this is true for machine and process sensors
  {                // but not for system sensors
    runnable->currentHistorySize += (sizeof(SCNatural) * maxValue);
  }
#endif

  saveArea.HistoryRestore(frequencies, (maxValue + 1) * sizeof(SCNatural));
  saveArea.HistoryRestore(&sumSamples, sizeof(SCNatural));
  saveArea.HistoryRestore(&lastUpdateDepth, sizeof(SCNatural));

  if (sensorType == scSensorFrequency)
    Write(scSensorUpdate);

  return true;
}
