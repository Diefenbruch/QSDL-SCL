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
|  SCSensorRequest  | SCSensorRequest.h | 04. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORREQUEST__

#define __SCSENSORREQUEST__

#include "SCSensorDynFreq.h"
#include "SCSensorTally.h"
#include "SCSensorCounter.h"

#include "SCRequest.h"
#include "SCScheduler.h"


/* Realisierungen fuer Requestorientierte Sensoren */

class SCSensorRequestFreq : public SCSensorDynFreq
{
  public:
    SCSensorRequestFreq (const char *pName,
                         const SCObject *father = NULL) :
      SCSensorDynFreq (pName, father) {}

    void  Update (SCInteger sample, const SCRequest *request)
    {
      SCSensorDynFreq::Update (sample, request->GetRequestType()->GetID());
    }
    void Update (SCInteger sample, const SCRequestType *serviceType)
    {
      SCSensorDynFreq::Update (sample, serviceType->GetID());
    }
    SCNatural GetAbsFreq (const SCRequestType *serviceType) const  // Absolute Frequenz fuer value
    {
      return SCSensorDynFreq::GetAbsFreq (serviceType->GetID());
    }
    SCReal GetRelFreq (const SCRequestType *serviceType) const  // Relative Frequenz fuer value
    {
      return SCSensorDynFreq::GetRelFreq (serviceType->GetID());
    }
    SCStream & DisplayValue (SCStream &pStream, SCNatural pValue) const
    {
      SCRequestType *serviceType;

      serviceType = SCRequestType::GetRequestTypeTable()->Find(pValue);
      assert(serviceType);

      pStream << *serviceType;

      return pStream;
    }
};


class SCSensorRequestTime : public SCSensorTally
{
  public:
    SCSensorRequestTime (const char *pName,
                         const SCDuration interval = 1.0,
                         const SCObject *father = NULL) :
      SCSensorTally (pName, interval, father) {}

    void  Update (const SCRequest *request)
    {
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             request->GetCreationTime());
    }
    void  Update (SCReal sample)
    {
      SCSensorTally::Update (sample);
    }
};


typedef class SCSensorCounter SCSensorRequestCount;

/* Globale Sensoren */

typedef class SCSensorRequestFreq SCSensorGlobalRequestFreq;
typedef class SCSensorCounter SCSensorGlobalRequestCount;


#endif // __SCSENSORREQUEST__
