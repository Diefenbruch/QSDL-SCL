/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                 |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|  SCTemplates  |  SCTemplates.cc   | 29. Feb 1996  |   SCL             |      |
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

#ifdef __GNUC__

#include "SCRunnable.h"
#include "SCMem.h"
#include "SCSignalType.h"
#include "SCRequestType.h"
#include "SCStateType.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCStackElem.h"
#include "SCStackElemList.h"
#include "SCTimer.h"
#include "SCSignal.h"
#include "SCDelayedSignal.h"
#include "SCRequest.h"
#include "SCEnabledTransition.h"
#include "SCTransition.h"
#include "SCActiveRunnable.h"
#include "SCProcess.h"
#include "SCProcedure.h"
#include "SCMachine.h"
#include "SCTimeEvent.h"
#include "SCAutomaton.h"
#include "SCTrace.h"
#include "SCSensor.h"

#include "SCList.tmpl.h"
#include "SCListSave.tmpl.h"
#include "SCTable.tmpl.h"
//#include "SCHeap.tmpl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

template class SCList<SCProcess>;
template class SCList<SCProcedure>;
template class SCList<SCMachine>;
template class SCList<SCEnabledTransition>;
template class SCList<SCTransition>;
template class SCList<SCStateType>;
template class SCList<SCProcessType>;
template class SCList<SCProcedureType>;
template class SCList<SCActiveRunnable>;
template class SCList<SCTrace>;
template class SCList<SCStackElem>;
template class SCList<SCSensor>;

template class SCListCons<SCProcess>;
template class SCListCons<SCProcedure>;
template class SCListCons<SCMachine>;
template class SCListCons<SCEnabledTransition>;
template class SCListCons<SCTransition>;
template class SCListCons<SCStateType>;
template class SCListCons<SCProcessType>;
template class SCListCons<SCProcedureType>;
template class SCListCons<SCActiveRunnable>;
template class SCListCons<SCTrace>;
template class SCListCons<SCStackElem>;
template class SCListCons<SCSensor>;

template class SCListIter<SCProcess>;
template class SCListIter<SCProcedure>;
template class SCListIter<SCMachine>;
template class SCListIter<SCEnabledTransition>;
template class SCListIter<SCTransition>;
template class SCListIter<SCStateType>;
template class SCListIter<SCProcessType>;
template class SCListIter<SCProcedureType>;
template class SCListIter<SCActiveRunnable>;
template class SCListIter<SCTrace>;
template class SCListIter<SCStackElem>;
template class SCListIter<SCSensor>;

template class SCList<SCTimer>;
template class SCListCons<SCTimer>;
template class SCListIter<SCTimer>;
template class SCListSave<SCTimer>;
template class SCList<SCSignal>;
template class SCListCons<SCSignal>;
template class SCListIter<SCSignal>;
template class SCListSave<SCSignal>;
template class SCList<SCDelayedSignal>;
template class SCListCons<SCDelayedSignal>;
template class SCListIter<SCDelayedSignal>;
template class SCListSave<SCDelayedSignal>;
template class SCList<SCRequest>;
template class SCListCons<SCRequest>;
template class SCListIter<SCRequest>;
template class SCListSave<SCRequest>;
template class SCList<SCTimeEvent>;
template class SCListCons<SCTimeEvent>;
template class SCListIter<SCTimeEvent>;
template class SCListSave<SCTimeEvent>;

template class SCTable<SCSignal>;
template class SCTable<SCTimer>;
template class SCTable<SCAutomaton>;
template class SCTable<SCTransitionList>;
template class SCTable<SCStackElemList>;
template class SCTable<SCTimerList>;
template class SCTable<SCProcessType>;
template class SCTable<SCProcedureType>;
template class SCTable<SCSignalType>;
template class SCTable<SCRequestType>;
template class SCTable<SCStateType>;
template class SCTable<SCTimeEvent>;
template class SCTable<SCSensor>;
template class SCTable<SCMem>;

// operator<< ist NOT a member function of the classes,
// so we MUST instantiate it seperately (at least if
// we want to compile the SCL with -fno-implicit-templates):


template class SCStream & operator<< (class SCStream &, const SCList<SCProcess> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCProcedure> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCMachine> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCEnabledTransition> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCTransition> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCStateType> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCProcessType> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCProcedureType> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCActiveRunnable> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCTrace> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCStackElem> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCSensor> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCSignal> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCTimer> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCRequest> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCDelayedSignal> &);
template class SCStream & operator<< (class SCStream &, const SCList<SCTimeEvent> &);

template class SCStream & operator<< (class SCStream &, const SCTable<SCSignal> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCTimer> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCAutomaton> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCTransitionList> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCStackElemList> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCTimerList> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCProcessType> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCProcedureType> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCSignalType> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCRequestType> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCStateType> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCTimeEvent> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCSensor> &);
template class SCStream & operator<< (class SCStream &, const SCTable<SCMem> &);

//template class SCHeap<SCTimeEvent>;

#endif
