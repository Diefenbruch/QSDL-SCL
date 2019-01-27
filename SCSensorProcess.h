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
|  SCSensorProcess  | SCSensorProcess.h | 04. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORPROCESS__

#define __SCSENSORPROCESS__

#include "SCSensorDynFreq.h"
#include "SCSensorStatFreq.h"
#include "SCSensorQueueLength.h"
#include "SCSensorTally.h"

#include "SCProcess.h"
#include "SCScheduler.h"
#include "SCStateType.h"
#include "SCProcessType.h"
#include "SCTransition.h"

#include "SCSensorSignal.h"
#include "SCSensorRequest.h"

/* Prozessorientierte Sensoren */

class SCSensorSignalQueueLength : public SCSensorQueueLength
{
  public:
    SCSensorSignalQueueLength (const char *pName,
                               const SCProcess *pProcess,
                               const SCDuration interval = 1.0) :
      SCSensorQueueLength (pName, (SCObject *)pProcess, interval)
    {
      assert (pProcess);
      process = pProcess;
    }

    // the embedded tally sensor must known the old
    // length of the input queue, so we
    // give it the changeValue parameter to
    // tell it the change according to the old value:

    void Update (SCInteger changeValue)
    {
      SCSensorQueueLength::Update (process->GetInputQueue(), changeValue);
    }

  private:
    const SCProcess *process;
};


class SCSensorStateFreq : public SCSensorStatFreq
{
  public:
    SCSensorStateFreq (const char *pName,
                       const SCNatural pMinStateID,
                       const SCNatural pMaxStateID,
                       const SCObject *father = NULL) :
      SCSensorStatFreq (pName, pMinStateID, pMaxStateID, father) {}

    void Update (SCNatural sample, const SCStateType *state)
    {
      SCSensorStatFreq::Update (sample, state->GetID());
    }
    SCNatural GetAbsFreq (const SCStateType *state) const  // Absolute Frequenz fuer value
    {
      return SCSensorStatFreq::GetAbsFreq (state->GetID());
    }
    SCReal GetRelFreq (const SCStateType *state) const  // Relative Frequenz fuer value
    {
      return SCSensorStatFreq::GetRelFreq (state->GetID());
    }
    SCStream & DisplayValue (SCStream &pStream, SCNatural pValue) const
    {
      SCStateType *stateType;

      stateType = SCStateType::GetStateTypeTable()->Find(pValue);
      assert(stateType);

      pStream << *stateType;

      return pStream;
    }
};


class SCSensorProcessFreq : public SCSensorStatFreq
{
  public:
    SCSensorProcessFreq (const char *pName,
                         const SCNatural pMinProcessTypeID,
                         const SCNatural pMaxProcessTypeID,
                         const SCObject *father = NULL) :
      SCSensorStatFreq (pName, pMinProcessTypeID, pMaxProcessTypeID, father) {}

    void Update (SCNatural sample, const SCProcessType *processType)
    {
      SCSensorStatFreq::Update (sample, processType->GetID());
    }
    SCNatural GetAbsFreq (const SCProcessType *process) const  // Absolute Frequenz fuer value
    {
      return SCSensorStatFreq::GetAbsFreq (process->GetID());
    }
    SCReal GetRelFreq (const SCProcessType *process) const  // Relative Frequenz fuer value
    {
      return SCSensorStatFreq::GetRelFreq (process->GetID());
    }
    SCStream & DisplayValue (SCStream &pStream, SCNatural pValue) const
    {
      SCProcessType *processType;

      processType = SCProcessType::GetProcessTypeTable()->Find(pValue);
      assert(processType);

      pStream << *processType;

      return pStream;
    }
};


class SCSensorProcessTime : public SCSensorTally
{
  public:
    SCSensorProcessTime (const char *pName,
                         const SCDuration interval = 1.0,
                         const SCObject *father = NULL) :
      SCSensorTally (pName, interval, father) {}

    void Update (const SCProcess *process)
    {
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             process->GetCreationTime());
    }
};


class SCSensorStateTime : public SCSensorTally
{
  public:
    SCSensorStateTime (const char *pName,
                       const SCDuration interval = 1.0,
                       const SCObject *father = NULL) :
      SCSensorTally (pName, interval, father) {}

    void Update (void)
    {
      assert(GetParent());

      // don't use LastUpdateTime of Sensor here
      // because StateTime sensors may be state specific!
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             ((SCAutomaton *)GetParent())->GetStateTime());
    }
};


class SCSensorTransitionFreq : public SCSensorStatFreq
{
  public:
    SCSensorTransitionFreq (const char *pName,
                            const SCNatural pMinTransitionID,
                            const SCNatural pMaxTransitionID,
                            const SCObject *father = NULL) :
      SCSensorStatFreq (pName, pMinTransitionID, pMaxTransitionID, father) {}

    void Update (SCNatural sample, const SCTransition *transition)
    {
      SCSensorStatFreq::Update (sample, transition->GetID());
    }
};

typedef class SCSensorCounter SCSensorProcessCount;

typedef class SCSensorSignalFreq SCSensorSignalReceiveFreq;
typedef class SCSensorSignalFreq SCSensorSignalSendFreq;
typedef class SCSensorSignalFreq SCSensorSignalDropFreq;
typedef class SCSensorSignalFreq SCSensorSignalConsumeFreq;
typedef class SCSensorSignalFreq SCSensorSignalSaveFreq;
typedef class SCSensorSignalFreq SCSensorSignalLoseFreq;
typedef class SCSensorSignalFreq SCSensorSignalQueueContent;

typedef class SCSensorCounter SCSensorSignalReceiveCount;
typedef class SCSensorCounter SCSensorSignalSendCount;
typedef class SCSensorCounter SCSensorSignalDropCount;
typedef class SCSensorCounter SCSensorSignalConsumeCount;
typedef class SCSensorCounter SCSensorSignalSaveCount;
typedef class SCSensorCounter SCSensorSignalLoseCount;

#endif // __SCSENSORPROCESS__
