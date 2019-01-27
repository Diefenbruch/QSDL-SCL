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
|  SCSensorSignal   | SCSensorSignal.h  | 04. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORSIGNAL__

#define __SCSENSORSIGNAL__

#include "SCSensorDynFreq.h"
#include "SCSensorTally.h"
#include "SCSensorCounter.h"

#include "SCSignal.h"
#include "SCScheduler.h"
#include "SCSignalType.h"


/* Realisierungen fuer Signalorientierte Sensoren */

class SCSensorSignalFreq : public SCSensorDynFreq
{
  public:
    SCSensorSignalFreq (const char * pName,
                        const SCObject *father = NULL) :
      SCSensorDynFreq (pName, father) {}

    void Update (SCInteger sample, const SCSignal *signal)
    {
      SCSensorDynFreq::Update (sample, signal->GetSignalType()->GetID());
    }
    void Update (SCInteger sample, const SCSignalType *signalType)
    {
      SCSensorDynFreq::Update (sample, signalType->GetID());
    }
    SCNatural GetAbsFreq (const SCSignalType *signal) const  // Absolute Frequenz fuer value
    {
      return SCSensorDynFreq::GetAbsFreq (signal->GetID());
    }
    SCReal GetRelFreq (const SCSignalType *signal) const  // Relative Frequenz fuer value
    {
      return SCSensorDynFreq::GetRelFreq (signal->GetID());
    }
    SCStream & DisplayValue (SCStream &pStream, SCNatural pValue) const
    {
      SCSignalType *signalType;

      signalType = SCSignalType::GetSignalTypeTable()->Find(pValue);
      assert(signalType);

      pStream << *signalType;

      return pStream;
    }
};


class SCSensorSignalTime : public SCSensorTally
{
  public:
          SCSensorSignalTime (const char *pName,
                              const SCDuration interval = 1.0,
                              const SCObject *father = NULL) :
            SCSensorTally (pName, interval, father) {}

    void Update (const SCSignal *signal)
    {
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             signal->GetCreationTime());
    }
};


/* Globale Sensoren */

typedef class SCSensorSignalFreq SCSensorGlobalSignalFreq;
typedef class SCSensorCounter SCSensorGlobalSignalCount;

#endif // __SCSENSORSIGNAL__
