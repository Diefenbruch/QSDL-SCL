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
|SCTimerControl | SCTimerControl.cc | 30. Aug 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     30.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCTimerControl.h"
#include "SCDataType.h"
#include "SCProcess.h"
#include "SCScheduler.h"
#include "SCTimer.h"
#include "SCTimeEvent.h"

#include "SCTraceControl.h"

#if _SC_DEBUGOUTPUT
  #include "SCDebug.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_PROFILING
extern int timer_ineff;
extern int locate_timer_ineff;
extern int kill_timer_ineff;
extern int set_timer_ineff;
#endif


SCTimerControl * SCTimerControl::timerControl = NULL;


/*----- Constructors -----*/

SCTimerControl::SCTimerControl (void) :
  SCRunnable (kSCTimerProcessID,  // runnableID
              true,               // sleeping
              SC_TIMER_CONTROL)   // type
{
#if _SC_VALIDATION_OPTIMIZE
  SCSize mySize;
#endif

  if (IsVerbose())
    SCScheduler::outputStream << "Creating " << *this << std::endl;

  timerControl = this;

  timerEventList = new SCTimeEventSaveList (true, kSCDefaultTableSize, this);
  assert(timerEventList);

#if _SC_VALIDATION_OPTIMIZE
  // Die Initialgroesse des TimerControl-Objekts muss nur einmal
  // beim Konstruieren berechnet werden. Danach sorgt
  // SCListSave automatisch fuer die Anpassung der aktuellen
  // Groesse die in SCRunnable gespeichert ist:

  Size(&mySize);
  SetCurrentSize(mySize.size);
  SetCurrentHistorySize(mySize.historySize);
#endif

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " created" << std::endl;
#endif
}

/*----- Destructor -----*/

SCTimerControl::~SCTimerControl (void)
{
  if (IsVerbose())
    SCScheduler::outputStream << "Deleting " << *this << std::endl;

  if (timerEventList)
    delete timerEventList;
}


/*----- Member-Funktionen -----*/

void SCTimerControl::Reschedule (void)
{
  SCTime nextTime;

  nextTime = -1.0;

  if (!timerEventList->IsEmpty())
  {
    nextTime = (*timerEventList->Head())()->GetTime();
  }

  if (nextTime >= 0.0)
  {
    Schedule (kSCScheduleTime, nextTime);
  }
  else
  {
    Schedule (kSCScheduleWaiting);
  }
}


void SCTimerControl::Body (void)
{
  SCProcess *   receiver;
  SCTimeEvent * timeEvent;

#if _SC_DEBUGOUTPUT
  scDebugLog << SCScheduler::GetCurrentTime() << ": ";
  scDebugLog << *this << " activated" << std::endl;
#endif

  if (IsVerbose())
    SCScheduler::outputStream << "Executing " << *this << std::endl;

  while (true)
  {
    while (!timerEventList->IsEmpty() &&
           (*timerEventList->Head())()->GetTime() == SCScheduler::GetCurrentTime())
    {
      timeEvent = (*timerEventList->Head())(); // Remove is done by SCTimer !!
      assert(timeEvent);

      receiver = (SCProcess *)timeEvent->GetRunnable();
      assert(receiver);

      receiver->ReceiveFromTimer(timeEvent->GetTimerID());
                                     // this removes and deletes
                                     // head from timerEventList !
    } // while (!timerEventList->IsEmpty())

    Reschedule();

    Suspend();
  } // while (true)

#if _SC_DEBUGOUTPUT
  scDebugLog << *this << " finishing" << std::endl;
#endif
}


SCTimeEventCons * SCTimerControl::AddTimerEvent (const SCTimer *pTimer)
{
  SCTimeEvent *     timerEvent;
  SCTimeEventCons * retValue = NULL;
  
  assert(pTimer);
  
  timerEvent = new SCTimeEvent(pTimer->GetProcessID(),
                               pTimer->GetTimeout(),
                               pTimer->GetTimerID());
  assert(timerEvent);

  retValue = timerEventList->Insert(timerEvent);

  return retValue;
}


void SCTimerControl::RemoveTimerEvent (SCTimeEventCons *pElem)
{
  assert(timerEventList);
  delete timerEventList->Remove(pElem);
}


void SCTimerControl::Size(SCSize *curSize) const
{
  SCRunnable::Size(curSize);

  // nothing more to do here because
  // the member timerEventList is
  // kept up to date automagically
  // by calls of AddTimer() in the
  // two constructors of SCTimer

}


SCBoolean SCTimerControl::Save(SCMem& saveArea) const
{
  SCRunnable::Save(saveArea);

  // nothing more to do here because
  // the member timerEventList is
  // kept up to date automagically
  // by calls of AddTimer() in the
  // two constructors of SCTimer
  // and a call to RemoveTimer() in
  // its destructor

  return true;
}


SCBoolean SCTimerControl::Restore(SCMem &saveArea)
{
  SCRunnable::Restore(saveArea);

  // nothing more to do here because
  // the member timerEventList is
  // kept up to date automagically
  // by calls of AddTimer() in the
  // two constructors of SCTimer
  // and a call to RemoveTimer() in
  // its destructor

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCTimerControl& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCTimerControl::Display(SCStream& pStream) const
{
  pStream << "runnable <timer control>";
  
  return SCRunnable::Display(pStream);
}
