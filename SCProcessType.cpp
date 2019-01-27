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
| SCProcessType |  SCProcessType.c  | 9. Aug 1994   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     09.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCProcessType.h"
#include "SCStateType.h"
#include "SCIndet.h"
#include "SCScheduler.h"
#include "SCProcess.h"
#include "SCSignal.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


#if _SC_NOINLINES
  #include "SCProcessType.inl.h"
#endif

SCProcessTypeList *SCProcessType::processTypeList = NULL;
SCProcessTypeTable *SCProcessType::processTypeTable = NULL;


SCProcessType::SCProcessType (SCNatural        ptID,
                              const char *     pName,
                              SCInteger        pMaxInstances,
                              SCNatural        pMaxInputQLen,
                              const SCObject * father) :
  SCType (ptID, pName, father),
  maxInstances(pMaxInstances),
  maxInputQueueLen(pMaxInputQLen)
{
  assert(processTypeList);  // allocated in SCProcessType::Initialize()
  processTypeList->InsertAfter(this);

  assert(processTypeTable);  // allocated in SCProcessType::Initialize()
  processTypeTable->Insert(ptID, this);

  instanceList = new SCProcessList(false, this); // don't delete elems!
  assert(instanceList);
  
  stateList = new SCStateTypeList(false, this);
  assert (stateList);
}


SCProcessType::~SCProcessType (void)
{
  if (instanceList)
    delete instanceList;

  if (stateList)
    delete stateList;
}


SCProcess * SCProcessType::Allocate (SCProcess *creator,
                                     SCDataType *actualParams)
{
  SCProcess *process;
  SCProcessCons *processCons;
  
  assert(instanceList);
  
  if (maxInstances == kSCUnlimitedProcesses || // further instances allowed ?
      instanceList->NumOfElems() < (SCNatural)maxInstances)
  {
    if (creator)
    {
      process = NewInstance(creator->Self(), actualParams);
    }
    else
    {
      process = NewInstance(kSCNoProcessID, actualParams);
    }
    assert(process);
    
    // newly created instaces have always the highest
    // ID, so we can append list:

    processCons = instanceList->InsertAfter(process);
    process->SetContainer(processCons);

    return process;
  }

  // no further instances are allowed!

  return NULL;
}


SCBoolean SCProcessType::Deallocate (SCProcess *process)
{
  assert (instanceList);
  assert (process->GetContainer());
  
  instanceList->Remove(process->GetContainer());

  return true;
}


SCProcessID SCProcessType::GetAProcessID (void) const
{
  SCNatural     procIndex;
  SCProcessIter iter(*instanceList);
  SCProcess *   process;

  if (maxInstances > 1)
    procIndex = SCScheduler::GetIndet()->NonDetChoice(instanceList->NumOfElems());
  else
    procIndex = 0;

  for (process = iter++;
       process;
       process = iter++, procIndex--)
  {
    if (procIndex == 0)
      return process->GetID();
  }
  
  return kSCNoProcessID;
}


SCProcess * SCProcessType::GetAProcess (void) const
{
  SCNatural     procIndex;
  SCProcessIter iter(*instanceList);
  SCProcess *   process;

  if (maxInstances > 1)
    procIndex = SCScheduler::GetIndet()->NonDetChoice (instanceList->NumOfElems());
  else
    procIndex = 0;

  for (process = iter++;
       process;
       process = iter++, procIndex--)
  {
    if (procIndex == 0)
      return process;
  }
  
  return (SCProcess *)NULL;
}


SCBoolean SCProcessType::Save(SCMem& saveArea) const
{
  assert(instanceList);
  
  return instanceList->Save(saveArea);
}


SCBoolean SCProcessType::Restore(SCMem &saveArea)
{
  assert(instanceList);
  
  return instanceList->Restore(saveArea);
}


void SCProcessType::Size(SCSize *curSize) const
{
  instanceList->Size(curSize);
}


void SCProcessType::Stop(void)
{
  SCProcessIter iter(*instanceList);
  SCProcess *   process;
  
  for (process = iter++;
       process;
       process = iter++)
  {
    ((SCAutomaton *)process)->Stop();
  }
}

#if _SC_VALIDATION_OPTIMIZE

void SCProcessType::SetModified(SCBoolean setQueues)
{
  SCProcessIter iter(*instanceList);
  SCProcess *   process;
  
  for (process = iter++;
       process;
       process = iter++)
  {
    process->SetModified(setQueues);
  }
}


void SCProcessType::UnsetModified(SCBoolean setQueues)
{
  SCProcessIter iter(*instanceList);
  SCProcess *   process;
  
  for (process = iter++;
       process;
       process = iter++)
  {
    process->UnsetModified(setQueues);
  }
}

#endif


SCBoolean SCProcessType::Initialize (SCNatural tableSize)
{
  // allocate list for all process types

  processTypeList = new SCProcessTypeList(false);
  assert(processTypeList);

  processTypeTable = new SCProcessTypeTable (tableSize, false);
  assert (processTypeTable);

  return true;
}


SCBoolean SCProcessType::Finish (void) // static!
{
  // delete list and table of process types

  if (processTypeList)
  {
    delete processTypeList;
    processTypeList = NULL;
  }

  if (processTypeTable)
  {
    delete processTypeTable;
    processTypeTable = NULL;
  }

  return true;
}


SCBoolean SCProcessType::StaticSave(SCMem& saveArea)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Save(saveArea);
  }
  
  return true;
}


SCBoolean SCProcessType::StaticRestore(SCMem &saveArea)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Restore(saveArea);
  }
  
  return true;
}


void SCProcessType::StaticSize(SCSize *curSize)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Size(curSize);
  }
}


void SCProcessType::StaticStop(void)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Stop();
  }
}


SCBoolean SCProcessType::operator==(const SCProcessType &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCProcessType::operator!=(const SCProcessType &second) const
{
  return (GetID() != second.GetID());
}


#if _SC_VALIDATION_OPTIMIZE

void SCProcessType::StaticSetModified(SCBoolean setQueues)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->SetModified(setQueues);
  }
}


void SCProcessType::StaticUnsetModified(SCBoolean setQueues)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->UnsetModified(setQueues);
  }
}

#endif
