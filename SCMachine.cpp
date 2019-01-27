/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+------------------+-------+
|   Module      |   File            |   Created     |   Project        |       |
+---------------+-------------------+---------------+------------------+-------+
|   SCMachine   |   SCMachine.cpp   | 5. Jul 1994   |   SCL            |       |
+---------------+-------------------+---------------+------------------+-------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     05.07.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#include <string.h>
#include <math.h>
#include <limits>

#include "SCStream.h"
#include "SCMachine.h"
#include "SCRequest.h"
#include "SCAutomaton.h"
#include "SCScheduler.h"
#include "SCMem.h"
#include "SCDebug.h"
#include "SCTraceControl.h"

#if _SC_NOINLINES
  #include "SCMachine.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCMachineList *SCMachine::machineList = NULL;


/*----- Konstruktor / Destruktor -----*/

SCMachine::SCMachine (SCRunnableID pMachineID,
                      SCDiscipline pDiscipline,
                      SCNatural pServers,
                      SCNatural pServices,
                      const SCDuration *const pSpeeds,
                      const char *pName,
                      SCObject *father) :
  SCRunnable (pMachineID,               // runnableID
              true,                     // sleeping
              SC_MACHINE,               // object type
              father),                  // parent object
  working(false),
  servers(pServers),
  discipline(pDiscipline),
  services(pServices),
  name(pName)
{
  speeds = new SCDuration[pServices];
  assert(speeds);
#ifdef _SC_DMALLOC
  memcpy ((char *)speeds, (const char *)pSpeeds, sizeof (SCDuration) * pServices);
#else
  memcpy ((void *)speeds, pSpeeds, sizeof (SCDuration) * pServices);
#endif
  serviceQueue = new SCRequestSaveList(true, this);
  assert(serviceQueue);

  lastTime = SCScheduler::GetCurrentTime();

  assert (machineList);  // allocated in SCMachine::Initialize()
  machineList->InsertAfter(this);

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceMachineCreate, this);
}


SCMachine::~SCMachine (void)
{
  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceMachineStop, this);

  delete serviceQueue;
  delete[] speeds;
}


/*----- Member-Funktionen -----*/

void SCMachine::FinishRequests (void)
{
  SCDuration      epsilon;
  SCRequestCons * loop;
  SCRequestCons * loopNext;
  SCRequest *     request;

  loop = GetServiceQueue()->Head();
  epsilon = SCScheduler::GetEpsilon();

  // don't use iterator here because
  // list is modified during iteration!

  while (loop != NULL)
  {
    loopNext = loop->Next();

    if ((*loop)()->GetRemainingTime() <= epsilon)
    {
      request = GetServiceQueue()->Remove (loop);

#if _SC_DEBUGOUTPUT
      scDebugLog << SCScheduler::GetCurrentTime() << ": " << *this;
      scDebugLog << " has finished ";
      scDebugLog << *request << std::endl;
#endif

      // Wakeup waiting process:
      request->GetCaller()->Schedule (kSCScheduleTime,
                                      SCScheduler::GetCurrentTime());

      if (IsTraceOn())
      {
        SCTraceControl::LogEvent (scTraceServiceFinish, this, request);
      }

      delete request;
    }
    
    loop = loopNext;
  }
}


void SCMachine::Reschedule (void)
{
  SCTime        nextTime = DBL_MAX;
  SCTime        jobTime;
  SCRequestIter iter (*GetServiceQueue());
  SCRequest *   request;

  working = false;

  for (request = iter++;
       request;
       request = iter++)
  {
    working = true;

    jobTime = SCScheduler::GetCurrentTime() + request->GetRemainingTime();

    if (nextTime > jobTime)
      nextTime = jobTime;
  }

  if (working)
    Schedule (kSCScheduleTime, nextTime);
  else
    Schedule (kSCScheduleWaiting);
}


void SCMachine::Size (SCSize *curSize) const
{
  SCRunnable::Size(curSize);              // Size of base class.

  curSize->size += sizeof(SCBoolean);     // working

  curSize->historySize += sizeof(SCTime); // Member lastTime

  serviceQueue->Size(curSize);            // Request Queue.
}


SCBoolean SCMachine::Save (SCMem& saveArea) const
{
  // 1. Save base class
  SCRunnable::Save (saveArea);                    // Store base class members.

  // 2. Save simple data members
  saveArea.HistoryStore(&lastTime, sizeof(SCTime));
  saveArea.Store(&working, sizeof(SCBoolean));  

  // 3. Save Service-Queue (serviceQueue)
  assert(serviceQueue);
  serviceQueue->Save(saveArea);

  return true;
}


SCBoolean SCMachine::Restore (SCMem &saveArea)
{
  // 1. Restore base class
  SCRunnable::Restore (saveArea);

  // 2. Restore simple data members
  saveArea.HistoryRestore (&lastTime, sizeof(SCTime));
  saveArea.Restore(&working, sizeof(SCBoolean));  

  // 3. Restore Service Room (serviceRoom)
  assert(serviceQueue);
  serviceQueue->Restore (saveArea);

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCMachine& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCMachine::Display(SCStream& pStream) const
{
  pStream << "machine <" << GetName() << ">";

  return SCRunnable::Display(pStream);
}

/* Static Members: */


SCBoolean SCMachine::Initialize (void)
{
  // allocate list for machines

  machineList = new SCMachineList(false);
  assert(machineList);

  return true;
}


SCBoolean SCMachine::Finish (void)
{
  // delete list for machines

  if (machineList)
    delete machineList;

  return true;
}


SCBoolean SCMachine::StaticSave (SCMem &saveArea)
{
  machineList->Save(saveArea);
  
  return true;
}


SCBoolean SCMachine::StaticRestore (SCMem &saveArea)
{
  machineList->Restore(saveArea);
  
  return true;
}


void SCMachine::StaticSize (SCSize *pSize)
{
  machineList->Size(pSize);
}


void SCMachine::StaticStop (void)
{
  SCMachineIter iter(*machineList);
  SCMachine *   machine;
  
  for (machine = iter++;
       machine;
       machine = iter++)
  {
    machine->Cancel();
  }   
}

#if _SC_VALIDATION_OPTIMIZE

void SCMachine::StaticSetModified (SCBoolean setQueues)
{
  SCMachineIter iter(*machineList);
  SCMachine *   machine;
  
  for (machine = iter++;
       machine;
       machine = iter++)
  {
    machine->SetModified(setQueues);
  }   
}


void SCMachine::StaticUnsetModified (SCBoolean setQueues)
{
  SCMachineIter iter(*machineList);
  SCMachine *   machine;
  
  for (machine = iter++;
       machine;
       machine = iter++)
  {
    machine->UnsetModified(setQueues);
  }   
}

#endif
