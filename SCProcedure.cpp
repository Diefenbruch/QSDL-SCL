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
|  SCProcedure  |  SCProcedure.cc   | 03. Feb 1997  |   SCL             |      |
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
#include "SCProcedure.h"
#include "SCProcess.h"
#include "SCProcedureType.h"
#include "SCScheduler.h"
#include "SCDebug.h"
#include "SCDataType.h"
#include "SCMem.h"

#include "SCTraceControl.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCProcedure.inl.h"
#endif

SCProcedure::SCProcedure (SCAutomaton *     pCaller,
                          SCProcess *       pOwner,
                          SCProcedureType * pProcType,
                          SCStateType *     startState,
                          SCDataType *      procedureParameters,
                          const SCBoolean   varSize,
                          const SCObject *  pParent) :
  SCAutomaton (startState,
               procedureParameters,
               SC_PROCEDURE,
               varSize,
               pParent),
  caller (pCaller),
  owner (pOwner),
  procType(pProcType),
  container(NULL)
{
  procType->Allocate(this);
}


SCProcedure::SCProcedure (const SCBoolean varSize,
                          const SCObject * pParent) :
  SCAutomaton(SC_PROCEDURE, varSize, pParent), // Invoke constructor of
  container(NULL)                              // base class
{
}


SCProcedure::~SCProcedure (void)
{
}


void SCProcedure::Stop (SCBoolean traceIt) // executed by procedure or by
                                           // scheduler
{
  assert (procType);

  procType->Deallocate(this);

  GetAutomatonTable()->Remove (GetID()); // prevent this instance of being
                                         // accessed

  assert (caller);
  
  if (traceIt && !SCScheduler::IsShuttingDown())
  {              // traceIt = false means that we actually only
                 // wants to stop the procedure not the calling process
                 // this method is called with traceIt = false from
                 // SCIndetVal::ChooseRunnable()
    caller->Stop(traceIt);
  }

  SCRunnable::Stop();
}


// This method is similar to SCProcess::Stop():

void SCProcedure::Return (SCDataType *retValue)
{
  (void)retValue;

  assert (procType != NULL);

#if _SC_DEBUGOUTPUT
  scDebugLog << Now() << ": " << *this << " is returning";
  if (retValue)
  {
    scDebugLog << " with value " << *retValue;
  }
  scDebugLog << std::endl;
#endif

  procType->Deallocate (this);   // free instance

  if (IsTraceOn())
    SCTraceControl::LogEvent (scTraceProcedureReturn, this);
  
  GetAutomatonTable()->Remove(GetID()); // prevent this instance of being
                                        // accessed

  // Wakeup waiting process:
  assert (caller != NULL);
  caller->Schedule (kSCScheduleNow);

  SCRunnable::Stop();
}


void SCProcedure::Size(SCSize *curSize) const
{
  // base class:
  SCAutomaton::Size(curSize);

  // simple state data members:
  curSize->size += sizeof(SCProcessID);      // callerID
  curSize->size += sizeof(SCProcessID);      // ownerID
}


SCBoolean SCProcedure::Save(SCMem& saveArea) const
{
  SCProcessID procID;

  SCAutomaton::Save(saveArea);

  // Process Identifier of Caller:
  procID = caller->GetID();
  saveArea.Store(&procID, sizeof(SCProcessID));
  
  // Process Identifier of Owner:
  procID = owner->GetID();
  saveArea.Store(&procID, sizeof(SCProcessID));

  return true;
}


SCBoolean SCProcedure::Restore(SCMem &saveArea)
{
  SCProcessID procID;

  SCAutomaton::Restore(saveArea);

  // Process Identifier of last Sender:
  saveArea.Restore(&procID, sizeof(SCProcessID));
  caller = (SCAutomaton *)SCScheduler::GetRunnableFromID (procID);
  
  saveArea.Restore(&procID, sizeof(SCProcessID));
  owner = (SCProcess *)SCScheduler::GetRunnableFromID (procID);

  return true;
}


SCStream& operator<< (SCStream& pStream, const SCProcedure& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCStream& SCProcedure::Display(SCStream& pStream) const
{
  assert(GetType());

  pStream << "procedure <" << *GetType() << ">";
  SCAutomaton::Display(pStream);
  
  return pStream;
}
