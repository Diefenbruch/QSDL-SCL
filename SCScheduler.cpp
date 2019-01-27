/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCScheduler  |  SCScheduler.cc   | 12. Jul 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
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

#include "SCScheduler.h"

#include "SCIndet.h"
#include "SCSocket.h"
#include "SCSignal.h"
#include "SCTimer.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCSignalType.h"
#include "SCRequestType.h"
#include "SCStateType.h"
#include "SCMachine.h"
#include "SCProcess.h"
#include "SCProcedure.h"
#include "SCRunnable.h"
#include "SCTimerControl.h"
#include "SCTimeEventList.h"
#include "SCTimeEvent.h"
#include "SCPath.h"
#include "SCMem.h"
#include "SCDataType.h"
#include "SCSensor.h"
#include "SCTraceControl.h"

#include "SCDebug.h"
#include "SCStream.h"

#include <signal.h>

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


// initialize static member:

SCScheduler *         SCScheduler::scheduler = NULL;
SCTime                SCScheduler::schedulerTime = 0.0;
SCRunnableID          SCScheduler::nextRunnableID = kSCProcessIDBase;
SCTimeEventSaveList * SCScheduler::activeQueue = NULL;
SCRunnable *          SCScheduler::current = NULL;
SCIndet *             SCScheduler::indet = NULL;
SCSocket *            SCScheduler::socket = NULL;
SCBoolean             SCScheduler::shutdown = false;
SCBoolean             SCScheduler::verbose = false;
SCProgress            SCScheduler::progress = kSCMaximumProgress;
SCStream              SCScheduler::errorStream(std::cerr);
SCStream              SCScheduler::outputStream(std::cout);

#if _SC_SENSOR_DEBUG
  SCTime nextSensorDisplayTime = 0.0;
  SCTime sensorDisplayInterval = 0.0;
#endif

#if _SC_NOINLINES
  #include "SCScheduler.inl.h"
#endif


/*----- Konstruktoren -----*/

SCScheduler::SCScheduler (SCIndet *indet_type, SCBoolean fairness)
{
  activeQueue = new SCTimeEventSaveList(fairness, kSCDefaultTableSize, this);
  assert (activeQueue != NULL);

  indet = indet_type;
  assert(indet != NULL);

  if (fairness)
    progress = kSCFairProgress;
  else
    progress = kSCMaximumProgress;
}


SCScheduler::~SCScheduler (void)
{
  delete activeQueue;

  scheduler = NULL; // clear static variable (necessary, see Finish())
}


/*----- Member-Funktionen -----*/

void SCScheduler::Body (void)
{
#if _SC_DEBUGOUTPUT
  scDebugLog.GetStream().precision(8);
  scDebugLog << schedulerTime << ": scheduler started" << std::endl;
#endif

  SCScheduler::outputStream << "Running Scheduler..." << std::endl << std::endl;

  while (true)
  {
    current = indet->ChooseRunnable (activeQueue);
                                                  // Choose next runnable to
                                                  // be activated.
                                                  
    if (current == NULL || shutdown)              // No runnable found or PEV
    {                                             // wants us to stop!
#if _SC_DEBUGOUTPUT
      scDebugLog << schedulerTime << ": scheduler stopped" << std::endl;
#endif
      SCScheduler::outputStream << std::endl << std::endl;
      SCScheduler::outputStream << "Shutting down Scheduler..." << std::endl << std::endl;

      shutdown = true;

      Stop();
      break;                                      // Exit main loop.
    }

#if _SC_SENSOR_DEBUG
    if (schedulerTime > nextSensorDisplayTime)
    {
      SCSensor::DisplayAll(scSensorDebugLog);
      nextSensorDisplayTime += sensorDisplayInterval;
    }
#endif
  
    current->Resume();                            // executing current runnable

  } // while (true)
}


void SCScheduler::Stop (void)
{
  SCTraceControl::LogEvent(scTraceSchedulerStop);
                                 // muss vor dem stoppen der runnables
                                 // geschehen, da sonst die PEV abstuerzt :-(

  SCProcessType::StaticStop();   // Instanzen aller Prozesstypen stoppen
  SCProcedureType::StaticStop(); // Instanzen aller Prozeduren stoppen
  SCMachine::StaticStop();       // alle Maschinen stoppen
    
  if (SCPath::GetPath())         // Path-Prozess stoppen (falls
    SCPath::GetPath()->Cancel(); // vorhanden)

  // Timer zuletzt stoppen, da auf ihn in den Stop-Methoden
  // der Prozesse noch zugegriffen wird (SCTimerControl::Kill):

  if (SCTimerControl::GetTimerControl())
    SCTimerControl::GetTimerControl()->Cancel();
}


// this method is used to guarantee that scheduler thread do
// the stopping (and not another thread)

void SCScheduler::Shutdown (int signalID)
{
  if (signalID == SIGINT)
    shutdown = true;        // tell scheduler thread
                            // to stop
}


// this method is used as signal handler
// to print status message

void SCScheduler::Status (int signalID)
{
  if (signalID == SIGHUP)
    indet->EndMessage(); // Display method depends on shutdown!

  if (signal(SIGHUP, SCScheduler::Status) == SIG_ERR)
  {
    std::cerr << "SCScheduler::Status(): Can't catch signal SIGHUP" << std::endl;
  }
}


SCProcessID SCScheduler::Create (SCProcessType * procType,
                                 SCProcess * creator,
                                 SCDataType * actualParams)
{
  SCProcess *newProcess;
  
  assert(procType);

  newProcess = procType->Allocate(creator, actualParams);
  
  if (newProcess)    // upper instance limit not reached?
  {
#if _SC_DEBUGOUTPUT
    if (creator != NULL)
    {
      scDebugLog << schedulerTime << ": " << *creator;
      scDebugLog << " creating " << *newProcess;
      if (newProcess->GetParameters())
        scDebugLog << " with params: " << *newProcess->GetParameters();
      scDebugLog << std::endl;
    }
    else
    {
      scDebugLog << schedulerTime << ": " << *newProcess;
      scDebugLog << " created" << std::endl;
    }
#endif

    assert (activeQueue != NULL);
    if (creator == NULL)                // creation at system startup?
    {
      newProcess->Resume();             // Resume new process. The new
                                        // process is suspended in the
                                        // Execute method. If we resume it,
                                        // it will run
                                        // into its Body method, call the
                                        // State() method and suspend there.
                                        // We don't need to insert the
                                        // process into the activeQueue
                                        // in this case because this will be
                                        // done by the Schedule() method which
                                        // is always called before the Suspend()
                                        // method in the State() method!
    }
    else                                // dynamically created process!
                                        // Do not resume to avoid interleaving
                                        // within create method.
                                        // AMH: Is this correct? Currently
                                        // it is not possible to interleave
                                        // the actions of the newly created
                                        // process with the actions of the
                                        // current transition.
                                        // MD: This is correct, we want no
                                        // intermediate states.
    {
      assert(!newProcess->IsWaiting()); // this should never happen because
                                        // automaton runnables are always
                                        // created with waiting = false
                                        // (see constructor of SCAutomaton)

      activeQueue->Insert(new SCTimeEvent(newProcess,
                                          newProcess->GetWakeupTime()));
    }

    SCTraceControl::LogEvent (scTraceProcessCreate, newProcess, creator);

    return (nextRunnableID++);
  }
  else
  {
#if _SC_DEBUGOUTPUT
    scDebugLog << schedulerTime << ": Process creation rejected, type: ";
    scDebugLog << *procType << std::endl;
#endif

    SCTraceControl::LogEvent (scTraceProcessCreate, (SCProcess *)NULL, creator);

    indet->LogError (scErrorProcessCreationFailed, creator, procType);

    return (kSCNoProcessID);
  }
}


SCProcessID SCScheduler::Call (SCProcedure * newProcedure,
                               SCAutomaton * caller)
{
  assert (newProcedure != NULL);

#if _SC_DEBUGOUTPUT
  scDebugLog << schedulerTime << ": " << *caller;
  scDebugLog << " calling " << *newProcedure;
  if (newProcedure->GetParameters())
    scDebugLog << " with params: " << *newProcedure->GetParameters();
  scDebugLog << std::endl;
#else
  (void)caller;
#endif

  assert (activeQueue != NULL);
  assert (!newProcedure->IsWaiting());
  
  activeQueue->Insert(new SCTimeEvent(newProcedure,
                                      newProcedure->GetWakeupTime()));

  SCTraceControl::LogEvent (scTraceProcedureCall, newProcedure, caller);

  return (nextRunnableID++);
}


void SCScheduler::Schedule (SCRunnable *runnable) // called from SCRunnable::SetWakeupTime()
{
  SCTimeEvent *timeEvent;

  assert (runnable != NULL);

  // Objects of running threads can't be in activeQueue,
  // so we can do a fine optimization here:

  if (runnable->Identity() != SCSystemThread::ExecutingThreadID())
  {
    // Check if runnable is already scheduled (this IS possible):

    timeEvent = activeQueue->LookupRunnable(runnable);

    if (timeEvent)
    {
      // Remove it for reschedule below:

#if _SC_VALIDATION_DEBUG
      scValidationDebugLog << "SCScheduler::Schedule(): removing " << *runnable;
      scValidationDebugLog << " from active queue!" << std::endl;
#endif
#if _SC_SIMULATION_DEBUG
      scSimulationDebugLog << "SCScheduler::Schedule(): removing " << *runnable;
      scSimulationDebugLog << " from active queue!" << std::endl;
#endif

      delete activeQueue->Remove(timeEvent);
    }
  }

  //
  // reinsert runnable in active queue:
  //
  if (!runnable->IsBlocked() && !runnable->IsWaiting())
  {
    timeEvent = new SCTimeEvent(runnable,
                                runnable->GetWakeupTime());
    assert(timeEvent);

#if _SC_DEBUGOUTPUT
    scDebugLog << schedulerTime << ": scheduling " << *runnable;
    scDebugLog << " for time " << runnable->GetWakeupTime() << std::endl;
#endif

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCScheduler::Schedule(): scheduling " << *runnable;
    scValidationDebugLog << " for time " << runnable->GetWakeupTime() << std::endl;
#endif
#if _SC_SIMULATION_DEBUG
    scSimulationDebugLog << "SCScheduler::Schedule(): scheduling " << *runnable;
    scSimulationDebugLog << " for time " << runnable->GetWakeupTime() << std::endl;
#endif

    activeQueue->Insert (timeEvent);
  }
}


SCBoolean SCScheduler::Initialize (SCIndet *indetType, // executed by Main-Thread!!!
                                   SCSocket *pSocket,
                                   SCBoolean fairness,
                                   SCBoolean isVerbose,
                                   SCNatural tableSize,
                                   SCReal displayInterval)
{
#if _SC_SENSOR_DEBUG
  sensorDisplayInterval = displayInterval;
#else
  (void)displayInterval;
#endif

  socket = pSocket;

  //
  // install own signal handler for Ctrl-C:
  //
  if (signal(SIGINT, SCScheduler::Shutdown) == SIG_ERR)
  {
    SCScheduler::errorStream << "SCScheduler::Initialize(): Can't catch signal SIGINT" << std::endl;
    return false;
  }

  //
  // install own signal handler for SIGHUP (used for status display):
  //
  if (signal(SIGHUP, SCScheduler::Status) == SIG_ERR)
  {
    SCScheduler::errorStream << "SCScheduler::Initialize(): Can't catch signal SIGHUP" << std::endl;
    return false;
  }

  if (socket)
  {
    if (!socket->Open())
    {
      SCScheduler::errorStream << "SCScheduler::Initialize(): Can't open " << *socket << std::endl;
      return false;
    }
  }  

  scheduler = new SCScheduler(indetType, fairness);
  assert (scheduler);

  verbose = isVerbose;

  SCAutomaton::Initialize(tableSize);
  SCSignal::Initialize(tableSize);
  SCTimer::Initialize(tableSize);
  SCProcessType::Initialize(tableSize);
  SCProcedureType::Initialize(tableSize);
  SCSignalType::Initialize(tableSize);
  SCRequestType::Initialize(tableSize);
  SCStateType::Initialize(tableSize);
  SCMachine::Initialize();
  SCSensor::Initialize(tableSize);

  SCTraceControl::LogEvent (scTraceSchedulerInit);

  return true;
}


void SCScheduler::Finish (void)                   // executed by Main-Thread!!!
{
  if (scheduler)
    delete scheduler;

  SCAutomaton::Finish();
  SCSignal::Finish();
  SCTimer::Finish();
  SCProcessType::Finish();
  SCProcedureType::Finish();
  SCSignalType::Finish();
  SCRequestType::Finish();
  SCStateType::Finish();
  SCMachine::Finish();
  SCSensor::Finish();

  if (indet)
    delete indet;
  if (socket)
    delete socket;

  SCTraceControl::Finish();

  signal(SIGINT, SIG_DFL);                        // restore default signal handler
  signal(SIGHUP, SIG_DFL);                        // restore default signal handler
}


void SCScheduler::Run (void)                      // executed by Main-Thread!!!
{
  assert(scheduler);
  SCSystemRunnable::SetInteractionThread(scheduler);
                                                  // inform all runnables that they
                                                  // should interact with the scheduler
                                                  // now
  scheduler->Resume();                            // Start scheduler.

  scheduler->Join();                              // main thread wait for scheduler
                                                  // thread to terminate...
}


SCStream& operator<< (SCStream& pStream, const SCScheduler& pData)
{
  assert(pData.activeQueue);
  
  pStream << "<scheduler> (" << std::endl;

  pStream << "activeQueue = [" << *pData.activeQueue << "]" << std::endl;

  if (pData.current)
  {
    pStream << "current = [" << pData.current << "]" << std::endl;
  }
  pStream << "schedulerTime = " << pData.schedulerTime << ")" << std::endl;

  return (pStream);
}


SCStream& SCScheduler::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


void SCScheduler::SetCurrentTime (SCTime newTime)
{
  if (schedulerTime != newTime)
  {
    schedulerTime = newTime;

#if _SC_DEBUGOUTPUT
    SCScheduler::outputStream << "Scheduler Time is now " << schedulerTime << std::endl;
#endif

    if (schedulerTime > 0.0)
      SCTraceControl::LogEvent (scTraceTimeChange, schedulerTime);
  }
}


const SCRunnable *SCScheduler::GetRunnableFromID (SCRunnableID id)
{
  SCAutomaton * automaton;
  
  automaton = SCAutomaton::GetAutomatonTable()->Find(id);
  
  return automaton;
}


/* ================== Methods specific to validation ============== */

SCBoolean SCScheduler::StaticSave(SCMem & saveArea)
{
  saveArea.HistoryStore(&schedulerTime, sizeof(SCTime));
  saveArea.HistoryStore(&nextRunnableID, sizeof(SCRunnableID));

  activeQueue->Save(saveArea);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCScheduler::StaticSave(): saved scheduler data ";
  scValidationDebugLog << ", offset now " << saveArea.GetOffset();
  scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << std::endl;
#endif

  return true;
}


SCBoolean SCScheduler::StaticRestore(SCMem &saveArea)
{
#if _SC_VALIDATION_DEBUG >= 2
  SCNatural pos = saveArea.GetOffset();
  SCNatural hpos = saveArea.GetHistoryOffset();
#endif

  saveArea.HistoryRestore(&schedulerTime, sizeof(SCTime));
  saveArea.HistoryRestore(&nextRunnableID, sizeof(SCRunnableID));

  activeQueue->Restore(saveArea);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCScheduler::StaticRestore(): restored scheduler data ";
  scValidationDebugLog << ", offset now " << saveArea.GetOffset() << "(+" << saveArea.GetOffset() - pos << ")";
  scValidationDebugLog << " histoffset now " << saveArea.GetHistoryOffset() << "(+" << saveArea.GetHistoryOffset() - hpos << ")" << std::endl;
#endif

  return true;
}


void SCScheduler::StaticSize(SCSize *curSize)
{
#if _SC_VALIDATION_DEBUG >= 2
  SCNatural size = curSize->size;
  SCNatural hsize = curSize->historySize;
#endif

  curSize->historySize += (sizeof(SCTime) +        // Member schedulerTime
                           sizeof(SCRunnableID));  // Member nextRunnableID

  activeQueue->Size(curSize);

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCScheduler::StaticSize(): added scheduler data";
  scValidationDebugLog << ", size now " << curSize->size << "(+" << curSize->size - size << ")";
  scValidationDebugLog << ", hist size now " << curSize->historySize << "(+" << curSize->historySize - hsize << ")" << std::endl;
#endif
}
