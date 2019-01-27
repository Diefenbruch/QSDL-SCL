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
|  SCSensorMachine  | SCSensorMachine.h | 04. Sep 1996  |   SCL             |   |
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

#ifndef __SCSENSORMACHINE__

#define __SCSENSORMACHINE__

#include "SCSensorDynFreq.h"
#include "SCSensorStatFreq.h"
#include "SCSensorQueueLength.h"
#include "SCSensorTally.h"

#include "SCMachineLtd.h"
#include "SCScheduler.h"

#include "SCSensorRequest.h"

/* Machinenorientierte Sensoren */

class SCSensorWaitingQueueLength : public SCSensorQueueLength
{
  public:
    SCSensorWaitingQueueLength (const char *pName,
                                const SCMachineLtd *pMachine,
                                const SCDuration interval = 1.0) :
      SCSensorQueueLength (pName, pMachine, interval)
    {
      assert (pMachine);
      machine = pMachine;
    }

    // the embedded tally sensor must known the old value of
    // the waiting queue length, so we
    // give it the changeValue parameter to
    // tell it the change according to the old value:

    void  Update (SCInteger changeValue) // +1 or -1
    {
      SCSensorQueueLength::Update (machine->GetWaitingQueue(), changeValue);
    }

  private:
    const SCMachineLtd *machine;
};


class SCSensorUtilization : public SCSensorTally
{
  public:
    SCSensorUtilization (const char *pName,
                         const SCMachine *pMachine,
                         const SCDuration interval = 1.0) :
      SCSensorTally (pName, interval, pMachine) { assert (pMachine);
                                                  machine = pMachine; }

    // this tally sensor must known the old value of
    // the service queue population, so we
    // give it the changeValue parameter to
    // tell it the change according to the old value:

    void  Update (SCInteger changeValue)
    {
      SCDuration weight = SCScheduler::GetCurrentTime() - GetLastUpdateTime();

      SCReal sample = 1.0 - (((SCReal)machine->NumOfFreeServers() - changeValue)/
                             (SCReal)machine->NumOfServers());
      assert(sample >= 0.0);
      SCSensorTally::Update (sample, weight);
    }

  private:
    const SCMachine *machine;
};

class SCSensorRequestWaitTime : public SCSensorTally
{
  public:
    SCSensorRequestWaitTime (const char *pName,
                             const SCDuration interval = 1.0,
                             const SCObject *father = NULL) :
      SCSensorTally (pName, interval, father) {}

    void  Update (const SCRequest *request)
    {
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             request->GetWaitStartTime());
    }
};

class SCSensorRequestServiceTime : public SCSensorTally
{
  public:
    SCSensorRequestServiceTime (const char *pName,
                                const SCDuration interval = 1.0,
                                const SCObject *father = NULL) :
      SCSensorTally (pName, interval, father) {}

    void  Update (const SCRequest *request)
    {
      SCSensorTally::Update (SCScheduler::GetCurrentTime() -
                             request->GetServiceStartTime());
    }
};

class SCSensorRequestThroughput : public SCSensorRequestCount
{
  public:
    SCSensorRequestThroughput (const char *pName,
                               const SCDuration interval = 1.0,
                               const SCObject *father = NULL) :
      SCSensorRequestCount (pName, interval, father) {}
      
    void  Update (const SCRequest *request)
    {
      (void)request;
      
      SCSensorRequestCount::Update(); // currently we use simply 1
                                      // for update probe; we may
                                      // want to use the actual
                                      // sevice amount here !?
    }
};

typedef class SCSensorRequestFreq  SCSensorRequestQueueContent;

#endif // __SCSENSORMACHINE__
