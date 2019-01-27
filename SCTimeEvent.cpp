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
| SCTimeEvent       | SCTimeEvent.cc    | 16. Aug 1994  |   SCL            |   |
+-------------------+-------------------+---------------+------------------+---+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     16.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCTimeEvent.h"
#include "SCRunnable.h"
#include "SCScheduler.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCTimeEvent::SCTimeEvent (SCRunnable *    pRunnable,
                          const SCTime &  pWhen,
                          const SCNatural pTimerID,
                          const SCObject *pParent) :
  SCObject(SC_TIME_EVENT, pParent),
  runnable(pRunnable),
  when(pWhen),
  timerID(pTimerID),
  container(NULL)
{
  assert(runnable);

  runnableID = runnable->GetID();

  if (runnable->IsAutomaton())
    isAutomaton = true;
  else
    isAutomaton = false;
    
  assert(when == runnable->GetWakeupTime());
}


SCTimeEvent::SCTimeEvent (SCRunnableID pID,
                          const SCTime &pWhen,
                          const SCNatural pTimerID,
                          const SCObject *pParent) :
  SCObject(SC_TIME_EVENT, pParent),
  runnableID(pID),
  runnable(NULL),
  isAutomaton(true),
  when(pWhen),
  timerID(pTimerID),
  container(NULL)
{
}


// This Konstruktor is used by SCActiveRunnable:
SCTimeEvent::SCTimeEvent (SCTimeEvent &orig) :
  SCObject(SC_TIME_EVENT),
  runnableID(orig.runnableID),
  runnable(orig.runnable),
  isAutomaton(orig.isAutomaton),
  when(orig.when),
  timerID(0),
  container(NULL)
{
}


SCTimeEvent::SCTimeEvent (SCMem &saveArea,
                          const SCObject *pParent) :
  SCObject(SC_TIME_EVENT, pParent),
  runnable(NULL),
  container(NULL)
{
  Restore(saveArea);
}


SCTimeEvent::~SCTimeEvent (void)
{
}


SCRunnable *SCTimeEvent::GetRunnable(void) const
{
  SCRunnable *automaton;
  
  // Bei Maschinen und Timer existiert Runnable
  // auf jeden Fall:
  if (!isAutomaton)
    return runnable;

  automaton = (SCRunnable *)SCScheduler::GetRunnableFromID(runnableID);
  assert(automaton);

  return automaton;
}


const SCTime SCTimeEvent::GetTime(void) const
{
  return when;
}


const SCNatural SCTimeEvent::GetTimerID(void) const
{
  return timerID;
}


SCStream& operator<< (SCStream& pStream, const SCTimeEvent& pData)
{
  pStream << "time event <" << pData.GetTime() << ">(#";
  pStream << pData.GetRunnableID() << ")";

  return pStream;
}

SCStream& SCTimeEvent::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


void SCTimeEvent::Size(SCSize *curSize) const
{
  // system state member:

  // history state member:
  curSize->historySize += (sizeof(SCRunnableID) +   // runnableID
                           sizeof(SCRunnable *) +   // runnable
                           sizeof(SCBoolean) +      // isAutomaton
                           sizeof(SCTime));         // when
}


SCBoolean SCTimeEvent::Save(SCMem& saveArea) const
{
  saveArea.HistoryStore(&runnableID, sizeof(SCRunnableID));
  saveArea.HistoryStore(&runnable, sizeof(SCRunnable *));
  saveArea.HistoryStore(&isAutomaton, sizeof(SCBoolean));
  saveArea.HistoryStore(&when, sizeof(SCTime));

  return true;
}


SCBoolean SCTimeEvent::Restore(SCMem& saveArea)
{
  saveArea.HistoryRestore(&runnableID, sizeof(SCRunnableID));
  saveArea.HistoryRestore(&runnable, sizeof(SCRunnable *));
  saveArea.HistoryRestore(&isAutomaton, sizeof(SCBoolean));
  saveArea.HistoryRestore(&when, sizeof(SCTime));

  return true;
}


SCBoolean SCTimeEvent::operator==(const SCTimeEvent &second) const
{
  return (runnableID == second.runnableID);
}


SCBoolean SCTimeEvent::operator!=(const SCTimeEvent &second) const
{
  return (runnableID != second.runnableID);
}

