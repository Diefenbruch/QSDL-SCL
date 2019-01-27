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
|SCDelayedSignal|SCDelayedSignal.cc | 20. Apr 1997  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCSignal.h"
#include "SCDelayedSignal.h"
#include "SCProcess.h"
#include "SCProcessType.h"
#include "SCScheduler.h"
#include "SCSignalTable.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCDelayedSignal.inl.h"
#endif

#include "SCDebug.h"


/*----- Konstruktor -----*/

SCDelayedSignal::SCDelayedSignal (SCSignal *pSignal,
                                  const SCProcessID pReceiverID,
                                  const SCProcessType * const pReceiverType,
                                  const SCTime pDelayedUntil,
                                  const SCObject* pParent) :
  SCObject(SC_DELAYED_SIGNAL, pParent),
  signal(pSignal),
  receiverID(pReceiverID),
  receiverType((SCProcessType *)pReceiverType),
  delayedUntil(pDelayedUntil)
{
}


SCDelayedSignal::SCDelayedSignal(SCMem &saveArea, const SCObject *pParent):
  SCObject(SC_DELAYED_SIGNAL, pParent),
  signal(NULL),
  receiverType(NULL)
{  
  Restore(saveArea);
}


SCDelayedSignal::~SCDelayedSignal (void)
{
  if (signal)
    delete signal;
}


SCStream& operator<< (SCStream& pStream, const SCDelayedSignal& pData)
{
  assert(pData.GetSignal());

  pStream << *pData.GetSignal() << ", delayed until " << pData.delayedUntil;
  
  return pStream;
}


SCStream& SCDelayedSignal::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


void SCDelayedSignal::Size(SCSize *curSize) const
{
  assert(signal);
  signal->Size(curSize);
  
#if _SC_VALIDATION_TIME == 1
  curSize->size += sizeof(SCDuration);               // remainingTime
#endif

  // history system state members:
  curSize->historySize += (sizeof(SCProcessID) +     // Member receiverID
                           sizeof(SCProcessType *) + // Member receiverType
                           sizeof(SCTime));          // Member delayedUntil
}


SCBoolean SCDelayedSignal::Save (SCMem& saveArea) const
{
#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif

  assert(signal);
  signal->Save(saveArea);

  saveArea.HistoryStore(&receiverID, sizeof(SCProcessID));
  saveArea.HistoryStore(&receiverType, sizeof(SCProcessType *));
  saveArea.HistoryStore(&delayedUntil, sizeof(SCTime));

#if _SC_VALIDATION_TIME == 1
  remainingTime = delayedUntil - SCScheduler::GetCurrentTime();
  assert(remainingTime >= 0.0);
  saveArea.Store(&remainingTime, sizeof(SCDuration));
#endif

  return true;
}


SCBoolean SCDelayedSignal::Restore (SCMem& saveArea)
{
#if _SC_VALIDATION_TIME == 1
  SCDuration remainingTime;
#endif

  if (signal)  // not recreated ?
  {
    signal->Restore(saveArea);
  }
  else         // recreated!
  {
    signal = new SCSignal(saveArea);
    assert(signal);
  }

  saveArea.HistoryRestore(&receiverID, sizeof(SCProcessID));
  saveArea.HistoryRestore(&receiverType, sizeof(SCProcessType *));
  saveArea.HistoryRestore(&delayedUntil, sizeof(SCTime));

#if _SC_VALIDATION_TIME == 1
  saveArea.Restore (&remainingTime, sizeof (SCDuration));
#endif

  return true;
}
