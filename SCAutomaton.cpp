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
|  SCAutomaton  |  SCAutomaton.cc   | 5. Jul 1994   |   SCL             |      |
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

#include <stdarg.h>
#include <string.h>

#include "SCStream.h"
#include "SCAutomaton.h"
#include "SCDataType.h"
#include "SCIndet.h"
#include "SCMachine.h"
#include "SCSignal.h"
#include "SCProcessType.h"
#include "SCRequestType.h"
#include "SCScheduler.h"
#include "SCEnabledTransition.h"
#include "SCStateType.h"
#include "SCSignalType.h"
#include "SCTimerControl.h"
#include "SCTimerType.h"
#include "SCTimer.h"
#include "SCTransition.h"
#include "SCDebug.h"
#include "SCPath.h"
#include "SCMem.h"
#include "SCProcedure.h"
#include "SCProcess.h"

#include "SCTraceControl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCAutomaton.inl.h"
#endif

#if _SC_PROFILING
extern int timer_ineff;
extern int remove_timer_ineff;
extern int trans_ineff;
#endif

SCAutomatonTable *SCAutomaton::automatonTable = NULL;


/*----- Construktor -----*/

SCAutomaton::SCAutomaton(SCStateType *startState,
                         SCDataType *processParameters,
                         const SCObjectType pObjectType,
                         const SCBoolean varSize,
                         const SCObject* pParent) :
  SCRunnable (SCScheduler::NewProcessID(), // runnableID
              false,                       // sleeping
              pObjectType,                 // object type
              pParent),                    // parent
  parameters(processParameters),
  lastState (startState),
  recalculateAwakeDelay(true),
  lastInputData(NULL),
  maxEC(0),
  enablingConditions(NULL),
  isInState(true),
  enabledTransitionList(NULL),
  variableSize(varSize)
{
  creationTime = Now();
  stateTime = Now();

  nextWakeupTime = Now();

  // Eintragung in die Automaten-Lookup-Tabelle erfolgt im generierten Code,
  // damit das Objekt erst fertig konstruiert wird, bevor es
  // zugreifbar wird!
}


// Der folgende Konstruktor legt einen "leeren" Prozess an.
// Dies dient als Vorbereitung fuer ein folgendes Restore

SCAutomaton::SCAutomaton(const SCObjectType pObjectType,
                         const SCBoolean varSize,
                         const SCObject *pParent) :
  SCRunnable(pObjectType, pParent),      // Invoke constructor of
                                         // base class
  variableSize(varSize)
{
  lastInputData = NULL;
  enablingConditions = NULL;
  parameters = NULL;

  enabledTransitionList = NULL;

  // Eintragung in die Automaten-Lookup-Tabelle erfolgt im generierten Code,
  // damit das Objekt erst fertig konstruiert wird, bevor es
  // zugreifbar wird!
}


SCAutomaton::~SCAutomaton (void)
{
  if (lastInputData)
    delete lastInputData;
  if (parameters)
    delete parameters;
  if (enablingConditions)
    delete[] enablingConditions;
  if (enabledTransitionList)
    delete enabledTransitionList;
}


// Die folgende Methode wird in SCProcedure redefiniert:

SCProcess *SCAutomaton::GetOwner (void) const
{
  return (GetType() == SC_PROCESS) ? (SCProcess *)this : (SCProcess *)NULL;
}


/*----- Member-Funktionen -----*/


void SCAutomaton::SetEnablingConditions(SCStateType *theState,
                                        SCNatural    numEC,
                                        ...) // executed by process
{
  va_list   args;
  SCNatural z_i;
  SCNatural index;

  if (enablingConditions)
  {
    assert(maxEC > 0);

    delete[] enablingConditions;

#if _SC_VALIDATION_OPTIMIZE
    currentHistorySize -= (sizeof(SCBoolean) * maxEC);
#endif    
  }

  if (numEC)
  {
    maxEC = theState->GetMaxTransitionID();
    enablingConditions = new SCBoolean[maxEC];
    assert(enablingConditions);

    for (z_i = 0; z_i < maxEC; z_i++)
      enablingConditions[z_i] = true;

    va_start (args, numEC);
    for (z_i = 0; z_i < numEC; z_i++)
    {
      index = va_arg (args, int) - kSCTransitionIDBase;
      enablingConditions[index] = va_arg (args, int);
    }
    va_end (args);

#if _SC_VALIDATION_OPTIMIZE
    currentHistorySize += (sizeof(SCBoolean) * maxEC);
#endif    
  }
  else
  {
    enablingConditions = NULL;
    maxEC = 0;
  }
}


SCTransitionID SCAutomaton::State (SCStateType *    theState, // executed by process
                                   const SCDuration awakeDelay,
                                   SCSignalType **  signalRead)
{
  SCTransitionID tid;
  SCNatural      scheduleHow;

  assert (theState != NULL);
  assert(GetOwner());
  
#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " entering " << *theState;
  if (awakeDelay != kSCNoAwakeDelay)
    scDebugLog << ", awake delay: " << awakeDelay << std::endl;
  else
    scDebugLog << std::endl;
#endif

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCAutomaton::State(): " << *this;
  scValidationDebugLog << " entering " << *theState;
  scValidationDebugLog << ", callScheduler: " << (GetOwner()->GetCallScheduler() ? "yes" : "no") << std::endl;
#endif

  /////////////////////////////////////////////////
  // 1. Initialization of variables, tracing, etc.
  /////////////////////////////////////////////////

  isInState = true;
  stateTime = Now();

  theState->SetReached();                         // mark state as reached

  // the senderID will get a new value when the next
  // transition is executed. it is not used till
  // then so we reset senderID here to a neutral value
  // to be nice to the validator:

  if (!theState->IsIntermediate())                // intermediate states
  {                                               // should not change senderID !
    GetOwner()->SetSender(kSCNoProcessID);

    if (IsTraceOn())
    {
      SCTraceControl::LogEvent(scTraceStateChange, GetOwner(), theState, awakeDelay);
    }
  }

  if (lastInputData)                              // destroy old signal parameters
  {
    delete lastInputData;
    lastInputData = NULL;
  }

  if (theState != lastState)                      // Check for state change.
  {
    lastState = theState;                         // Mark state change.
    recalculateAwakeDelay = true;                 // if state changed we must
                                                  // recalculate the awake time
                                                  // (if delay given)
  }

  ////////////////////////////////////////////////////////////////////////
  // 2. Calculate next wakeup time and the enabled transitions of this
  //    process. The member nextWakeupTime is only used in the
  //    EnabledTransitions() method. It is first initialized in the
  //    constructor of SCAutomaton.
  //    The variable scheduleHow discriminates three cases:
  //    1. Process is passive and waiting for an input (kSCScheduleWaiting)
  //    2. Process is active because a consumable signal is in the queue
  //       (kSCScheduleNow)
  //    3. Process is active because the current state has spontaneous
  //       transitions (kSCScheduleTime)
  ////////////////////////////////////////////////////////////////////////

  scheduleHow = kSCScheduleWaiting;               // this is the fallback value
                                                  // used if nothing else (signal
                                                  // consume or spontaneous
                                                  // transition) can happen

  // Method EnabledTransitions() uses nextWakeupTime so
  // calculate it before calling EnabledTransitions() below:

  if (theState->HasSpontaneous() &&
      awakeDelay != kSCNoAwakeDelay)
  {
    scheduleHow = kSCScheduleTime;
    if (recalculateAwakeDelay)
    {
      nextWakeupTime = Now() + awakeDelay;        // awake time is now valid, so
      recalculateAwakeDelay = false;              // don't recalculate it until
                                                  // state changed
    }
  }

  // Maybe the Receive()-method has already build the
  // enabledTransitionList, so we only do it here
  // if enabledTransitionList == NULL.

  if (!enabledTransitionList &&
      GetOwner()->GetCallScheduler())
  {
    enabledTransitionList = EnabledTransitions();
  }

  // if the process has enabled transitions we can activate
  // it now:

  if (enabledTransitionList)
    scheduleHow = kSCScheduleNow;

  /////////////////////////////////////////////////////////
  // 3. Suspending to give scheduler the control to decide
  //    about the next active runnable. By the call of
  //    Schedule() (see below) this process is 
  //    could be reinserted in the activeQueue,
  //    depending on the value of scheduleHow.
  /////////////////////////////////////////////////////////

  if (GetOwner()->GetCallScheduler())             // callScheduler is false if
  {                                               // backtracking from Request state
                                                  // or if State() is called
                                                  // first time after DYNAMIC
                                                  // creation of the process
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCAutomaton::State(): " << *this;
    scValidationDebugLog << " suspending in " << *theState << std::endl;
#endif
    Schedule (scheduleHow, nextWakeupTime);       // only if scheduleHow = kSCScheduleTime
                                                  // the nextWakeupTime
                                                  // is really used!
    Suspend();                                    // May lead to backtracking!

#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCAutomaton::State(): " << *this;
    scValidationDebugLog << " resuming in " << *theState << std::endl;
#endif
  }
  else
  {
    GetOwner()->SetCallScheduler (true);
  }

  assert(GetOwner()->GetCallScheduler() == true);
  
  /////////////////////////////////////////////////////////////////////
  // 4. Testing if a backtracking took place. Three possible
  //    cases:
  //    a) No backtracking => isInState = true
  //    b) Backtracking to request-intermediate-state => isInState = false
  //    c) Backtracking to another process state => isInState = true
  //
  //    In case of b) we must leave this method and bring the control
  //    flow to the correct point in the generated code.
  //    In case of a) or c) we can proceed, because the control
  //    flow is already at the correct point.
  //    NOTE: In case of backtracking the data of the processes
  //          is already restored, we need only a correction of the
  //          control flow!
  /////////////////////////////////////////////////////////////////////

  if (!isInState)                                 // backtracking to Request state?
  {                                               // this IS possible!!
#if _SC_VALIDATION_DEBUG
    scValidationDebugLog << "SCAutomaton::State(): " << *this;
    scValidationDebugLog << " leaving " << *theState;
    scValidationDebugLog << " and go to request/call state" << std::endl;
#endif

    return (kSCNoTransition);                     // bring control flow to the right
                                                  // position in the generated code!
  }

  ////////////////////////////////////
  // 5. Choose Transition to execute:
  ////////////////////////////////////

  tid = Transition (signalRead);                 // sets isInState to false
                                 			     	 		 // it's not sufficient to set
                                 			     			 // isInState to false HERE,
                                 			     			 // since after backtracking only
                                 			     			 // Transition() is called!

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " leaving " << *theState << std::endl;
#endif

  return (tid);                    // return ID of transition
}


SCTransitionID SCAutomaton::Transition (SCSignalType **signalRead)
{
  SCTransition *        transition;
  SCSignal *            sig;
  SCTransitionID        tid;
  SCEnabledTransition * enabTrans;

  assert(SCScheduler::GetIndet());
  
  enabTrans = SCScheduler::GetIndet()->ChooseTransition ();
                                                  // choose a transition for
                                                  // execution
  if (enabTrans == NULL)
  {
#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this;
    scDebugLog << " executing idle transition" << std::endl;
#endif
    *signalRead = NULL;                           // don't confuse generated code

    isInState = false;
  
    recalculateAwakeDelay = true;  // new awake delay must be set

    return kSCNoTransition;
  }
  
  assert (enabTrans != NULL);

  transition = enabTrans->GetTransition();
  sig = enabTrans->GetSignal();

  delete enabTrans;

  if (transition == NULL)                         // implicit signal consumption ?
  {
    assert (sig);

#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this;
    scDebugLog << " dropped " << *sig << std::endl;
#endif

    GetOwner()->GetInputQueue()->Remove (sig);

    if (IsTraceOn())
    {
      SCTraceControl::LogEvent (scTraceSignalDrop, GetOwner(), sig);
    }

    SCScheduler::GetIndet()->LogError (scErrorSignalDrop, GetOwner(), sig);

    *signalRead = NULL;                           // don't confuse generated code
    isInState = false;
  
    delete sig;

    return kSCNoTransition;
  }

  transition->SetExecuted();                      // mark transition as executed

  tid = transition->GetID();

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this;
  if (sig)
  {
    scDebugLog << " consuming " << *sig << std::endl;
  }
  else if (transition->GetPriority() == kSCPrioInputNone)
  {
    scDebugLog << " got signal none" << std::endl;
  }
  else
  {
    scDebugLog << " got continuous signal" << std::endl;
  }

  scDebugLog << Now() << ": " << *this;
  scDebugLog << " executing " << *transition << std::endl;
#endif

  if (sig)                      // signal consuming input ?
  {
    GetOwner()->GetInputQueue()->Remove (sig);   // remove signal from queue

    if (IsTraceOn())
    {
      SCTraceControl::LogEvent (scTraceSignalConsume, GetOwner(), sig, transition);
    }

    *signalRead = (SCSignalType *)sig->GetSignalType();
    assert (*signalRead);

    GetOwner()->SetSender (sig->GetSenderID());
    lastInputData = sig->RetrieveData();

    delete sig;                 // delete signal object
  }
  else                             // input none or continuous signal!
  {
    *signalRead = NULL;

    if (!lastState->IsIntermediate()) // intermediate states should
    {                                 // NOT change the sender variable!
      GetOwner()->SetSender (GetOwner()->Self());
    }
    
    if (IsTraceOn())
    {
      if (transition->GetPriority() == kSCPrioInputNone) // input none?
        SCTraceControl::LogEvent (scTraceSpontTrans, GetOwner(), transition);
      else                                          // continuous signal!
        SCTraceControl::LogEvent (scTraceContSignal, GetOwner(), transition);
    }

    recalculateAwakeDelay = true;  // new awake delay must be set
  }

  isInState = false;
  
  return (tid);                    // return ID of transition
}


SCEnabledTransitionList *SCAutomaton::EnabledTransitions (const SCBoolean useInputTail) const
{
  SCTransitionList *        transitions;
  SCEnabledTransitionList * enabTransList = new SCEnabledTransitionList;
  SCBoolean                 useNones;

  assert (lastState != NULL);

  useNones = lastState->HasSpontaneous() &&
             Now() >= nextWakeupTime;

  //////////////////////////////////////////
  // 1. get transitions of priority inputs:
  //////////////////////////////////////////

  transitions = lastState->GetPriorityInputs();

  if (!transitions->IsEmpty())
    SignalEnabledTransitions (enabTransList, true, useInputTail);

  if (enabTransList->IsEmpty()) // no priority inputs possible?
  {

    ////////////////////////////////////////
    // 2. get transitions of normal inputs:
    ////////////////////////////////////////

    transitions = lastState->GetNormalInputs();

    if (!transitions->IsEmpty())
      SignalEnabledTransitions (enabTransList, false, useInputTail);
  }

  ////////////////////////////////////////
  // 3. add spontaneous transitions:
  //    (priority inputs does NOT disable
  //     spontaneous transitions!)
  ////////////////////////////////////////

  if (useNones)
  {
    transitions = lastState->GetInputNones();

    if (!transitions->IsEmpty())
      SpontaneousEnabledTransitions (transitions, enabTransList);

    ////////////////////////////////////////
    // 4. use continuous signal transitions:
    //    (these are only enabled if no
    //     signal consuming transitions active!)
    ////////////////////////////////////////

    if (enabTransList->IsEmpty())
    {
      transitions = lastState->GetContSignals();

      if (!transitions->IsEmpty())
        SpontaneousEnabledTransitions (transitions, enabTransList);
    }
  }

  if (enabTransList->IsEmpty())
  {
    delete enabTransList;
    enabTransList = NULL;
  }
  return (enabTransList);
}

//
// Output an Prozess-ID:
//

void SCAutomaton::Output (const SCProcessID         receiverID, // executed by process
                          const SCSignalType *const signalType,
                          SCDataType *              data,
                          const SCDuration          delay)
{
  SCProcess * receiver;
  SCSignal *  signal;

  receiver = (SCProcess*) SCScheduler::GetRunnableFromID (receiverID);

  if (!receiver) // kein Prozess zur angegebenen ID ? (ist bei
  {              // gestoppten Process der Fall)
#if _SC_DEBUGOUTPUT
    scDebugLog << std::endl << Now() << ": " << *this << " cannot send signal <";
    scDebugLog << signalType->GetName() << "> since process ID is invalid";
    scDebugLog << " (#" << receiverID << ")" << std::endl << std::endl;
#endif

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalNotSent, GetOwner(), signalType);

    SCScheduler::GetIndet()->LogError (scErrorNoSignalReceiver, GetOwner(), signalType);

    return;
  }

  if (!delay)
  {
    Output (receiver, signalType, data);
  }
  else
  {
    signal = new SCSignal (GetOwner()->Self(),
                           GetOwner()->GetType(),
                           signalType, data, 0);
    assert (signal);

#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this << " sending ";
    scDebugLog << *signal << " to " << *receiver;
    scDebugLog << ", delay: " << delay << std::endl;
#endif

    SCPath::GetPath()->SpawnDelayed (receiver,
                                     signal,
                                     Now() + delay);

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalSend, GetOwner(), receiver, signal, delay);
  }
}


//
// Output an Prozesstyp:
//

void SCAutomaton::Output (SCProcessType *           receiverType, // executed by process
                          const SCSignalType *const signalType,
                          SCDataType *              data,
                          const SCDuration          delay)
{
  SCProcess * receiver = NULL;
  SCSignal * signal;

  receiver = receiverType->GetAProcess();

  if (!receiver)             // kein Prozess zum angegebenen Prozesstyp ?
  {
#if _SC_DEBUGOUTPUT
    scDebugLog << std::endl << Now() << ": " << *this << " cannot send signal <";
    scDebugLog << signalType->GetName() << "> since no living instance of ";
    scDebugLog << *receiverType << std::endl << std::endl;
#endif

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalNotSent, GetOwner(), signalType);

    SCScheduler::GetIndet()->LogError (scErrorNoSignalReceiver, GetOwner(), signalType);

    return;
  }

  if (!delay)
  {
    Output (receiver, signalType, data);    // Output an Instanz
  }
  else
  {
    signal = new SCSignal (GetOwner()->Self(),     // sender ID
                           GetOwner()->GetType(),  // sender type
                           signalType,             // signal type
                           data,                   // signal data
                           0);                     // timer ID

#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this << " sending ";
    scDebugLog << *signal << " to " << *receiver;
    scDebugLog << ", delay: " << delay << std::endl;
#endif

    SCPath::GetPath()->SpawnDelayed (receiverType,
                                     signal,
                                     Now() + delay);

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalSend, GetOwner(), receiver, signal, delay);
  }
}


//
// Output an Prozess-Instanz:
//

void SCAutomaton::Output (SCProcess *               receiver, // executed by process
                          const SCSignalType *const signalType,
                          SCDataType *              data)
{
  SCSignal * signal;

  assert(receiver);

  signal = new SCSignal (GetOwner()->Self(),     // sender ID
                         GetOwner()->GetType(),  // sender type
                         signalType,             // signal type
                         data,                   // signal data
                         0);                     // timer ID
  assert(signal);

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " sending ";
  scDebugLog << *signal << " to " << *receiver << std::endl;
#endif

  receiver->Receive (signal);

  if (IsTraceOn())
  {
    SCTraceControl::LogEvent (scTraceSignalSend, GetOwner(), receiver, signal);
  }
}


void SCAutomaton::Request (SCMachine *machine, // executed by process
                           const SCRequestType *const requestType,
                           const SCDuration serviceAmount,
                           const SCNatural priority)
{
  assert (machine != NULL);

  assert (!isInState);

  assert (serviceAmount >= 0);

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " requesting <";
  scDebugLog << requestType->GetName() << "> from ";
  scDebugLog << *machine << ", amount: ";
  scDebugLog << serviceAmount << ", priority: " << priority << std::endl;
#endif

  machine->NewRequest (this, requestType, serviceAmount, priority);

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCAutomaton::Request(): " << *this;
  scValidationDebugLog << " suspending in request method" << std::endl;
#endif

  Schedule (kSCScheduleBlocked);          // Block until request is completed.
  Suspend();

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCAutomaton::Request(): " << *this;
  scValidationDebugLog << " resuming in request method" << std::endl;

  if (isInState)                                  // backtracking to normal state?
  {                                               // this IS possible!!
    scValidationDebugLog << "SCAutomaton::Request(): " << *this;
    scValidationDebugLog << " leaving request state and go to ";
    scValidationDebugLog << *lastState << std::endl;
  }
#endif

  assert(GetOwner()->GetCallScheduler() == true);
}


void SCAutomaton::Create (SCProcessType * ptype,
                          SCDataType * actualParams)  // executed by process
{
  assert(ptype);
  
  GetOwner()->SetOffspring (SCScheduler::Create (ptype,
                                                 GetOwner(),
                                                 actualParams));
  if (GetOwner()->Offspring() != kSCNoProcessID)
    SCScheduler::GetIndet()->Create (GetOwner()->Offspring()); // store IDs of created instance
}


void SCAutomaton::Call (SCProcedure *newProcedure)
{
  assert(newProcedure);
  assert(!isInState);
  
#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " calls ";
  scDebugLog << *newProcedure << std::endl;
#endif

  SCScheduler::Call (newProcedure, this);
  SCScheduler::GetIndet()->Create (newProcedure->GetID());

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCAutomaton::Call(): " << *this;
  scValidationDebugLog << " suspending in call method" << std::endl;
#endif

  Schedule (kSCScheduleBlocked);          // Block until procedure returns.
  Suspend();

#if _SC_VALIDATION_DEBUG
  scValidationDebugLog << "SCAutomaton::Call(): " << *this;
  scValidationDebugLog << " resuming in call method" << std::endl;

  if (isInState)                                  // backtracking to normal state?
  {                                               // this IS possible!!
    scValidationDebugLog << "SCAutomaton::Call(): " << *this;
    scValidationDebugLog << " leaving call state and go to ";
    scValidationDebugLog << *lastState << std::endl;
  }
#endif
}


void SCAutomaton::SetTimer (const SCTime             when,  // executed by process
                            const SCTimerType *const timerType,
                            SCDataType *             data)
{
  SCTimer *         timer;
  SCTimerCons *     elem;
  SCTimerSaveList * timerQueue = GetOwner()->GetTimerQueue();
  
  ResetTimer(timerType, data, false);  // SET implies RESET!
  
  timer = new SCTimer (GetOwner(),
                       when < Now() ? Now() : when,
                       timerType,
                       data,
                       timerQueue);
  assert (timer);

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *GetOwner();
  scDebugLog << " setting " << *timer << std::endl;
#endif

  for (elem = timerQueue->Head();
       elem != NULL;
       elem = elem->Next())
  {
    if ((*elem)()->GetTimeout() >= when)
    {
      timerQueue->InsertBefore (timer, elem);
      break;
    }
  }
  if (!elem) // neues Maximum? 
  {
    timerQueue->InsertAfter (timer);
  }
  
  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceTimerSet, GetOwner(), timer);

  SCTimerControl::GetTimerControl()->Reschedule();
}


void SCAutomaton::ResetTimer (const SCTimerType *const timerType, // executed by process
                              SCDataType *       data,
                              SCBoolean          rescheduleTimerControl)
{
  SCTimerCons *     removeThis;
  SCTimerSaveList * timerQueue = GetOwner()->GetTimerQueue();

  //
  // test if timer is in timer queue then
  // timer has not fired yet and we need no
  // call to RemoveInputTimer():
  //
  removeThis = GetOwner()->LocateTimer (timerType, data);

  if (removeThis != NULL)
  {
#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *GetOwner();
    scDebugLog << " resetting " << *(*removeThis)() << std::endl;
#endif

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceTimerReset, GetOwner(), (*removeThis)());

    delete timerQueue->Remove (removeThis);
  }
  else
  {
    RemoveInputTimer (timerType, data);
  }
  
  if (rescheduleTimerControl)  // Reset not invoked by new Set ?
    SCTimerControl::GetTimerControl()->Reschedule();
}


SCNatural SCAutomaton::Decision (const SCNatural numOfArgs, ...) const // executed by process
{
  va_list    args;
  SCNatural  numOfTrues = 0;
  SCNatural  z_i;
  SCNatural  lastTrue = 0;
  SCBoolean *argVec;
  SCNatural  selection;

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this;
  scDebugLog << " got decision with " << numOfArgs << " cases: ";
#endif

  argVec = new SCBoolean[numOfArgs];
  assert(argVec);

  va_start (args, numOfArgs);
  for (z_i = 0; z_i < numOfArgs; z_i++)
  {
    argVec[z_i] = va_arg (args, int);
    if (argVec[z_i])
    {
      numOfTrues++;
      lastTrue = z_i;
      #if _SC_DEBUGOUTPUT
        scDebugLog << "T ";
      #endif
    }
    else
    {
      #if _SC_DEBUGOUTPUT
        scDebugLog << "F ";
      #endif
    }
  }
  va_end (args);

  selection = SCScheduler::GetIndet()->ChooseOneTrue (numOfArgs, numOfTrues, lastTrue, argVec);

#if _SC_DEBUGOUTPUT
  scDebugLog << std::endl;
  scDebugLog << Now() << ": " << *this;
  scDebugLog << " choose number " << selection << ".\n";
#endif

  delete[] argVec;
  return (selection);
}


void SCAutomaton::Assert (const SCBoolean theAssertion) const
{
  if (!theAssertion)        // assertion failed ?
  {
    SCScheduler::GetIndet()->LogError (scErrorAssertionFailed);
  }
}


/* Private Memberfunktionen */

SCNatural SCAutomaton::SignalEnabledTransitions (SCEnabledTransitionList *enabTransList,
                                                 const SCBoolean          priorityInputs,
                                                 const SCBoolean          useInputTail) const
{
  SCTransitionList      *transitions;
  SCTransitionListTable *transitionsTable;
  SCSignalSaveList *    inQueue = GetOwner()->GetInputQueue();
  SCTransition *         t;
  SCSignalCons *        curMsgCons;
  SCSignal *            curMsg;
  SCEnabledTransition *  newEnabTrans;
  const SCSignalID *     saveSet;
  SCSignalID             signalID;
  SCNatural              saveSetSize;
  SCNatural              numPossibleTrans;
  SCBoolean              saved;
  SCBoolean              saveAll;
  SCBoolean              condition;

  if (inQueue->IsEmpty())  // input queue is empty ?
  {
    return (0);
  }

  if (priorityInputs)
  {
    saveSetSize = 0;
    saveSet = NULL;
    saveAll = true;
    transitionsTable = lastState->GetPriorityInputsTable();
    assert (transitionsTable);
  }
  else
  {
    saveSetSize = lastState->GetSaveSetSize();
    saveSet = lastState->GetSaveSet();
    saveAll = lastState->GetSaveAll();
    transitionsTable = lastState->GetNormalInputsTable();
    assert (transitionsTable);
  }

  numPossibleTrans = 0;
  curMsgCons = !useInputTail ? inQueue->Head() : inQueue->Tail();

  while (curMsgCons && !numPossibleTrans)
  {
    saved = false;                      // flag indicates if current signal
                                        // has to be saved

    curMsg = (*curMsgCons)();
    assert (curMsg);

    assert (curMsg->GetSignalType());
    signalID = curMsg->GetSignalType()->GetID();

    ///////////////////////////////////////////////////////
    // 1. test if current signal (curMsg) must be saved:
    ///////////////////////////////////////////////////////

    if (saveSetSize > 0 && !saveAll)    // !saveAll: save signals in signal set
    {
      assert (lastState->GetSaveSetTable() != NULL);

      if (lastState->GetSaveSetTable()->Find (signalID))
        saved = true;
    }

    ///////////////////////////////////////////////////
    // 2. if current signal must not be saved then
    //    determine all possible transitions for this
    //    signal (Note: if current signal can ONLY
    //    be consumed by a transition with failed
    //    enabling condition we must save it too!)
    ///////////////////////////////////////////////////

    if (!saved)                         // current signal is not saved?
    {
      transitions = transitionsTable->Find (signalID);
                                        // get transitions for signal

      if (transitions)                  // transition for current
      {                                 // signal present?
        SCTransitionIter iter (*transitions);

        for (t = iter++; t; t = iter++) // this is NOT inefficient
        {                               // don't optimize!!!!!
#if _SC_PROFILING
          trans_ineff++;
#endif

          condition = enablingConditions ?
            enablingConditions[t->GetID() - kSCTransitionIDBase] :
            true;                       // no enabling condition = true!
          if (!condition)               // enabling condition not true ?
          {
            if (numPossibleTrans == 0)  // no other transition can consume this signal yet?
              saved = true;             // we must save this signal (this can
          }                             // be changed below if we find an active
          else                          // transition for this signal later
          {
            newEnabTrans = new SCEnabledTransition (t, curMsg);
            assert(newEnabTrans);
            enabTransList->InsertAfter (newEnabTrans);
            numPossibleTrans++;         // this terminates while-loop
            saved = false;              // release possible save of signal caused by
                                        // transition with failed enabling condition!
          } // else
        }  // for (t = iter++;...)
      } // if (transitions)
    } // if (!saved)

    ///////////////////////////////////////////////////
    // 3. handle 'save *'. Important: Save All can
    //    have exceptions. If saveAll = true then
    //    saveSet contains the exceptions!
    ///////////////////////////////////////////////////

    if (saveAll && numPossibleTrans == 0)  // saved ist hier immer false,
    {                                      // wenn saveAll true ist!
      saved = true;

      ////////////////////////////////////////
      // handle exception signals of saveAll:
      ////////////////////////////////////////

      if (saveSet != NULL &&
          lastState->GetSaveSetTable()->Find (signalID)) // exception ?
      {
        saved = false;
      }

    } // if (saveAll && ...)

    if (saved)
    {
#if _SC_DEBUGOUTPUT
      scDebugLog << Now() << ": " << *this << " saved ";
      scDebugLog << *curMsg << std::endl;
#endif

      if (IsTraceOn())
        SCTraceControl::LogEvent (scTraceSignalSave, GetOwner(), curMsg);

      curMsgCons = curMsgCons->Next();   // look for next next signal in queue

#if _SC_PROFILING
      trans_ineff++;
#endif
    }
    else if (numPossibleTrans == 0) // no possible transition for this signal
    {                               // and no save for this signal? -> implicit transition!
      newEnabTrans = new SCEnabledTransition (NULL, // NULL indicates implicit transition !
                                        curMsg);
      assert(newEnabTrans);
      enabTransList->InsertAfter (newEnabTrans);
      numPossibleTrans++;           // this terminates while-loop
    }
  } // while (curMsgCons...)

  return (numPossibleTrans);
}

// this method is used for spontaneous inputs AND continuous signals!

SCNatural SCAutomaton::SpontaneousEnabledTransitions (SCTransitionList *transitions,
                                                      SCEnabledTransitionList *enabTransList) const
{
  SCEnabledTransition * newEnabTrans;
  SCTransition *        t;
  SCNatural             numPossibleTrans = 0;
  SCInteger             maxPrio = 0;
  SCBoolean             gotOne = false;
  SCTransitionIter      iter (*transitions);

  for (t = iter++;
       t;
       t = iter++)
  {
#if _SC_PROFILING
    trans_ineff++;
#endif

    if (!enablingConditions ||
        enablingConditions[t->GetID() - kSCTransitionIDBase])
    {
      newEnabTrans = new SCEnabledTransition (t, (SCSignal *)NULL);
      assert(newEnabTrans);
      enabTransList->InsertAfter(newEnabTrans);
      numPossibleTrans++;
      if (!gotOne)
      {
        maxPrio = t->GetPriority();
        gotOne = true;
      }
      else if (maxPrio > t->GetPriority())
        break;
    }
  }
  return (numPossibleTrans);
}


void SCAutomaton::RemoveInputTimer (const SCTimerType *const timerType,
                                    SCDataType *data)
{
  SCSignalCons *     loop;
  SCSignal *         sig;
  SCSignalSaveList * inQueue = GetOwner()->GetInputQueue();

  if (!inQueue->IsEmpty())
  {
    for (loop = inQueue->Head();
         loop;
         loop = loop->Next())
    {
      sig = (*loop)();
      if (sig->GetTimerID() > 0 &&                        // is it a timer?
          (sig->GetSignalType()->GetID() == timerType->GetID()) && // signal identical ?
           sig->SameData (data))                          // data identical ?
      {
#if _SC_DEBUGOUTPUT
        scDebugLog << Now() << ": " << *this;
        scDebugLog << " erased " << *sig << ", timerID:";
        scDebugLog << sig->GetTimerID() << std::endl;
#endif

        inQueue->Remove (loop);

        if (IsTraceOn())
          SCTraceControl::LogEvent (scTraceTimerRemove, GetOwner(), sig);

        delete sig;

        return;                                           // timer can only be once in queue!
      }
#if _SC_PROFILING
      timer_ineff++;
      remove_timer_ineff++;
#endif
    } // for
  }
}


void SCAutomaton::Size(SCSize *curSize) const
{
  // base class:
  SCRunnable::Size(curSize);

  // simple state data members:

  // simple history state data members:
  curSize->historySize += sizeof(SCStateType *);       // lastState
  curSize->historySize += sizeof(SCBoolean) ;          // recalculateAwakeDelay
  curSize->historySize += sizeof(SCNatural) ;          // maxEC
  if(enablingConditions)
    curSize->historySize += sizeof(SCBoolean) * maxEC; // Array of enabling conditions
  curSize->historySize += sizeof(SCBoolean);           // isInState
  curSize->historySize += sizeof(SCTime);              // nextWakeUpTime
  curSize->historySize += sizeof(SCTime);              // creationTime
  curSize->historySize += sizeof(SCTime);              // stateTime

#if _SC_VALIDATION_TIME == 1
  curSize->size += sizeof(SCDuration);                 // remainingTime
#endif
}

SCBoolean SCAutomaton::Save(SCMem& saveArea) const
{
  SCRunnable::Save(saveArea);
  
#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif

  // nur Zeiger speichern, da statisches Objekt! 
  saveArea.HistoryStore(&lastState, sizeof(SCStateType *));
  
  // nur Zeiger speichern, da statisches Objekt! 
  saveArea.HistoryStore(&recalculateAwakeDelay, sizeof(SCBoolean));
  
  saveArea.HistoryStore(&maxEC, sizeof(SCNatural));

  if (enablingConditions)
  {
    assert(maxEC > 0);
    saveArea.HistoryStore(enablingConditions, sizeof(SCBoolean) * maxEC);
  }

  saveArea.HistoryStore(&isInState, sizeof(SCBoolean));
  saveArea.HistoryStore(&nextWakeupTime, sizeof(SCTime));
  saveArea.HistoryStore(&creationTime, sizeof(SCTime));
  saveArea.HistoryStore(&stateTime, sizeof(SCTime));

#if _SC_VALIDATION_TIME == 1
  if (!IsBlocked() && !IsWaiting())
    remainingTime = nextWakeupTime - SCScheduler::GetCurrentTime();
  else
    remainingTime = 0;
  saveArea.Store(&remainingTime, sizeof(SCDuration));
#endif

  return true;
}


SCBoolean SCAutomaton::Restore(SCMem &saveArea)
{
#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif

  SCRunnable::Restore(saveArea);
  
  saveArea.HistoryRestore(&lastState, sizeof(SCStateType *));
  
  saveArea.HistoryRestore(&recalculateAwakeDelay, sizeof(SCBoolean));
  
  if (enablingConditions)
  {
    delete[] enablingConditions;

#if _SC_VALIDATION_OPTIMIZE
    currentHistorySize -= (sizeof(SCBoolean) * maxEC);
#endif    
  }

  saveArea.HistoryRestore(&maxEC, sizeof(SCNatural));
  
  if (maxEC > 0)
  {
    enablingConditions = new SCBoolean[maxEC];
    assert(enablingConditions);

    saveArea.HistoryRestore(enablingConditions, sizeof(SCBoolean) * maxEC);

#if _SC_VALIDATION_OPTIMIZE
    currentHistorySize += (sizeof(SCBoolean) * maxEC);
#endif    
  }
  else
  {
    enablingConditions = NULL;
  }
  
  saveArea.HistoryRestore(&isInState, sizeof(SCBoolean));
  saveArea.HistoryRestore(&nextWakeupTime, sizeof(SCTime));
  saveArea.HistoryRestore(&creationTime, sizeof(SCTime));
  saveArea.HistoryRestore(&stateTime, sizeof(SCTime));
  
#if _SC_VALIDATION_TIME == 1
  saveArea.Restore (&remainingTime, sizeof (SCDuration));
#endif

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCAutomaton& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCAutomaton::Display(SCStream& pStream) const
{
  return SCRunnable::Display(pStream);
}


SCBoolean SCAutomaton::Initialize (SCNatural automatonTableSize)
{
  // allocate table for all automaton instances

  automatonTable = new SCAutomatonTable(automatonTableSize, false);
  assert(automatonTable);

  return true;
}


SCBoolean SCAutomaton::Finish (void) // static!
{
  // delete table of automaton instances

  if (automatonTable)
    delete automatonTable;

  return true;
}


SCBoolean SCAutomaton::operator==(const SCAutomaton &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCAutomaton::operator!=(const SCAutomaton &second) const
{
  return !(*this == second);
}
