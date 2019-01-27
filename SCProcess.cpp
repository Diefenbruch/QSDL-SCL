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
|   SCProcess   |   SCProcess.cc    | 5. Jul 1994   |   SCL             |      |
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

#include "SCStream.h"
#include "SCProcess.h"
#include "SCDataType.h"
#include "SCIndet.h"
#include "SCSignal.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCStateType.h"
#include "SCTimer.h"
#include "SCTimerType.h"
#include "SCDebug.h"
#include "SCMem.h"

#include "SCTraceControl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCProcess.inl.h"
#endif

#if _SC_PROFILING
extern int timer_ineff;
extern int remove_timer_ineff;
extern int trans_ineff;
#endif


/************************************ SCProcess *******************************/

/*----- Konstruktor -----*/

SCProcess::SCProcess (const SCProcessID creator,
                      const SCProcessType *pProcType,
                      SCStateType *startState,
                      SCDataType *processParameters,
                      const SCBoolean varSize,
                      const SCObject* pParent) :
  SCAutomaton (startState,
  	       processParameters,
  	       SC_PROCESS,
           varSize,
  	       pParent),		 // parent
  parentID(creator),
  senderID(kSCNoProcessID),
  offspringID(kSCNoProcessID),
  procType((SCProcessType *)pProcType)
{

  inputQueue = new SCSignalSaveList(true, this);
  assert(inputQueue);
  timerQueue = new SCTimerSaveList(true, this);
  assert(timerQueue);

  if (creator == kSCNoProcessID)
  {
    callScheduler = true; // Scheduler muss zunaechst nur bei den statisch
                          // beim Systemstart erzeugten Prozessen
                          // noch mal gesondert aufgerufen werden,
                          // da Scheduler noch nicht laeuft.
                          // (statisch erzeugte Prozesse werden vom
                          // Main-Thread angestartet, damit sie
                          // in ihrer State-Methode stehen)
  }
  else
  {
    callScheduler = false; // dynamisch erzeugt Prozesse rufen den Scheduler
                           // bei der ersten(!) Aktivierung nicht auf, da
                           // der Scheduler bereits nach Beendigung
                           // der erzeugenden Transition aufgerufen wird
                           // (ChooseRunnable() bereits durchgefuehert,
                           // nur noch ChooseTransition() notwendig)
  }

  // Eintragung in die Prozess-Tabelle erfolgt im generierten Code,
  // damit das Objekt erst fertig konstruiert wird, bevor es
  // zugreifbar wird!
}

// Der folgende Konstruktor legt einen "leeren" Prozess an.
// Dies dient als Vorbereitung fuer ein folgendes Restore

SCProcess::SCProcess (const SCBoolean varSize,
                      const SCObject *pParent):
  SCAutomaton(SC_PROCESS, varSize, pParent) // Invoke constructor of
                                            // base class
{
  inputQueue = new SCSignalSaveList(true, this);
  assert(inputQueue);
  timerQueue = new SCTimerSaveList(true, this);
  assert(timerQueue);

  // callScheduler _MUST_ be set to false here because
  // recreated processes are not resumed by SCProcessList::Restore()
  // callScheduler should only be true if we recreate a process which is
  // in its Request-Method (this IS possible). In this case
  // we correct the false setting made here in the generated
  // code with a call of SetCallScheduler(true) behind the
  // call of the Request-method (which is the entry-point for
  // recreated processes in Request-method)

  callScheduler = false;

  // Eintragung in die Prozess-Tabelle erfolgt im generierten Code,
  // damit das Objekt erst fertig konstruiert wird, bevor es
  // zugreifbar wird!
}

SCProcess::~SCProcess (void)
{
  if (inputQueue)
    delete inputQueue;
  if (timerQueue)
    delete timerQueue;
}


/*----- Member-Funktionen -----*/

void SCProcess::Stop (SCBoolean traceIt) // executed by process or by scheduler
                                         // parameter indicates if tracing
                                         // should be done (false if called
                                         // from SCIndetVal::ChooseRunnable())
{
  assert (procType);

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " is stopping" << std::endl;
#endif

  procType->Deallocate (this);   // free instance

  if (traceIt && IsTraceOn())
    SCTraceControl::LogEvent (scTraceProcessStop, this);

  GetAutomatonTable()->Remove(GetID());
                                // prevent this instance of being
                                // accessed

  if (inputQueue)               // better do this here because
  {                             // of global use of signalTable!
    delete inputQueue;          // frees all entries in signalTable
    inputQueue = NULL;
  }
  if (timerQueue)               // better do this here because
  {                             // of global use of signalTable!
    delete timerQueue;          // frees all entries in signalTable
    timerQueue = NULL;
  }

  SCRunnable::Stop();           // may not return...
}


/* Protected Memberfunktionen */

void SCProcess::Receive (SCSignal *sig)   // executed by another process
{                                          // or a self-sending process
  assert (sig != NULL);

  // check for queue overflow:
  if (procType->GetMaxInputQueueLength() == kSCUnlimitedQueueLength ||
      inputQueue->NumOfElems() < procType->GetMaxInputQueueLength())
  {
#if _SC_VALIDATION_OPTIMIZE
    SetModified();
#endif

#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this << " queueing ";
    scDebugLog << *sig << std::endl;
#endif

    inputQueue->InsertAfter (sig); // append signal to inputQueue

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalReceive, this, sig);

    // test if the receiver is waiting for this
    // signal in his State() method; isInState can
    // be false if
    // 1. Process is executing a request method
    // 2. Process is doing a self-send and executing
    //    this method itself
    if (IsInState())
    {
      // enabledTransitionList holds the enabled transition
      // of this process until they are used (and retrieved)
      // by the Indet-objects. They are caculated here and
      // in the State()-method (see above)

      if (!GetEnabledTransitions() ||
          GetWakeupTime() > Now())
      {
        SetEnabledTransitions (EnabledTransitions (true)); // true = use last signal

        // test if process can consume this signal:
        if (GetEnabledTransitions())
        {
          Schedule (kSCScheduleNow);
        }
      }
    }
  }
  else                  // queue overflow!
  {
#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this;
    scDebugLog << " queue overflow, rejecting " << *sig << std::endl;
#endif

    if (IsTraceOn())
      SCTraceControl::LogEvent (scTraceSignalLose, this, sig);

    SCScheduler::GetIndet()->LogError (scErrorSignalLose, this, sig);

    delete sig;
  }
}


void SCProcess::ReceiveFromTimer (SCNatural timerID)
{
  SCTimer *  timer;
  SCSignal *signal;

  (void)timerID;

  if (!timerQueue->IsEmpty())
  {
#if _SC_VALIDATION_OPTIMIZE
    SetModified();
#endif

    timer = timerQueue->Remove();  // remove head from list

    signal = new SCSignal (Self(),                // id of sender
                           GetType(),             // type of sender
                           timer->GetTimerType(), // signal type
                           timer->RetrieveData(), // signal data
                           timer->GetTimerID());  // timer ID
    assert (signal);

#if _SC_DEBUGOUTPUT
    scDebugLog << Now() << ": " << *this << " ";
    scDebugLog << *timer << " fired" << std::endl;
#endif

    Receive (signal);
    
    delete timer;
  }
}


SCTimerCons *SCProcess::LocateTimer (const SCTimerType *const timerType,
                                     const SCDataType *const  data) const
{
  SCTimerCons      *cons;
  SCTimer          *elem;
  SCTimerList      *list;
  SCTimerListTable *table;

  assert (timerType);

  table = timerType->GetTimerListTable();
  assert (table);
  list = table->Find (GetID());  // list contains only running timer instances
                                 // of type timerType

  if (list && !list->IsEmpty())
  {
    for (cons = list->Head();
         cons != NULL;
         cons = cons->Next())
    {
      elem = (*cons)();
      if (data == NULL && elem->GetData() == NULL)
      {
        return (elem->GetContainer());  // nicht cons returnieren!
      }
      if (data != NULL &&
          elem->GetData() != NULL &&
          data->Equal (*elem->GetData()))
      {
        return (elem->GetContainer());  // nicht cons returnieren!
      }

#if _SC_DEBUGOUTPUT
      scDebugLog << Now() << ": " << *this << " found ";
      scDebugLog << *elem << " but data is different" << std::endl;
#endif

#if _SC_PROFILING
      timer_ineff++;
      locate_timer_ineff++;
#endif
    } // for (...)
  } // if (list)
#if _SC_DEBUGOUTPUT
  else
  {
    scDebugLog << Now() << ": " << *this;
    scDebugLog << " no timers of type " << *timerType << std::endl;
  }
#endif
  return NULL;
}


void SCProcess::Size(SCSize *curSize) const
{
  // base class:
  SCAutomaton::Size(curSize);

  // simple state data members:
  curSize->historySize += sizeof(SCProcessID);      // senderID
  curSize->historySize += sizeof(SCProcessID);      // parentID
  curSize->historySize += sizeof(SCProcessID);      // offspringID

  // queues
  assert(inputQueue);
  inputQueue->Size(curSize);                 // inputQueue
  assert(timerQueue);
  timerQueue->Size(curSize);                 // timerQueue

  // simple history state data members:
  curSize->historySize += sizeof(SCProcessType *) ;    // procType
}


SCBoolean SCProcess::Save(SCMem& saveArea) const
{
  SCAutomaton::Save(saveArea);
  
  // Process Identifier of last Sender:
  saveArea.HistoryStore(&senderID, sizeof(SCProcessID));
  
  // Process Identifier of Parent:
  saveArea.HistoryStore(&parentID, sizeof(SCProcessID));
  
  // Process Identifier of Child Process created last:
  saveArea.HistoryStore(&offspringID, sizeof(SCProcessID));
  
  // nur Zeiger speichern, da statisches Objekt! 
  saveArea.HistoryStore(&procType, sizeof(SCProcessType *));

  assert(inputQueue);
  inputQueue->Save(saveArea);
  assert(timerQueue);
  timerQueue->Save(saveArea);

  return true;
}


SCBoolean SCProcess::Restore(SCMem &saveArea)
{
  SCAutomaton::Restore(saveArea);
  
  // Process Identifier of last Sender:
  saveArea.HistoryRestore(&senderID, sizeof(SCProcessID));
  
  saveArea.HistoryRestore(&parentID, sizeof(SCProcessID));
  
  saveArea.HistoryRestore(&offspringID, sizeof(SCProcessID));
  
  saveArea.HistoryRestore(&procType, sizeof(SCProcessType *));

  assert(inputQueue);
  inputQueue->Restore(saveArea);                  // Empty and Refill list with data
                                                  // from saveArea.
  assert(timerQueue);
  timerQueue->Restore(saveArea);                  // Empty and Refill list with data
                                                  // from saveArea.

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCProcess& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCProcess::Display(SCStream& pStream) const
{
  assert(procType);

  pStream << "process <" << *procType << ">";
  
  return SCAutomaton::Display(pStream);
}
