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
|SCProcedureType| SCProcedureType.cc| 9. Aug 1994   |   SCL            |       |
+---------------+-------------------+---------------+------------------+-------+
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
#include "SCProcedureType.h"
#include "SCStateType.h"
#include "SCIndet.h"
#include "SCScheduler.h"
#include "SCProcedure.h"
#include "SCSignal.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


#if _SC_NOINLINES
  #include "SCProcedureType.inl.h"
#endif

SCProcedureTypeList *SCProcedureType::procedureTypeList = NULL;
SCProcedureTypeTable *SCProcedureType::procedureTypeTable = NULL;


SCProcedureType::SCProcedureType (SCNatural        ptID,
                                  char *           pName,
                                  const SCObject * father) :
  SCType (ptID, pName, father)
{
  assert (procedureTypeList);  // allocated in SCProcedureType::Initialize()
  procedureTypeList->InsertAfter(this);

  assert(procedureTypeTable);  // allocated in SCProcedureType::Initialize()
  procedureTypeTable->Insert(ptID, this);

  instanceList = new SCProcedureList(false); // don't delete elems!
  assert(instanceList);
  
  stateList = new SCStateTypeList (false);
  assert (stateList);
}


SCProcedureType::~SCProcedureType (void)
{
  if (instanceList)
  {
    delete instanceList;
  }
  if (stateList)
    delete stateList;
}


SCBoolean SCProcedureType::Allocate (SCProcedure *procedure)
{
  SCProcedureCons *procedureCons;

  assert(instanceList);
  assert(procedure);
    
  // newly created instaces have always the highest
  // ID, so we can append list:
  
  procedureCons = instanceList->InsertAfter(procedure);
  procedure->SetContainer(procedureCons);
  
  return true;
}


SCBoolean SCProcedureType::Deallocate (SCProcedure *procedure)
{
  assert(instanceList);
  assert(procedure->GetContainer());

  instanceList->Remove(procedure->GetContainer());
                                         // remove it from instance list

  return true;
}


SCBoolean SCProcedureType::Save(SCMem& saveArea) const
{
  assert(instanceList);
  
  return instanceList->Save(saveArea);
}


SCBoolean SCProcedureType::Restore(SCMem &saveArea)
{
  assert(instanceList);
  
  return instanceList->Restore(saveArea);
}


void SCProcedureType::Size(SCSize *curSize) const
{
  instanceList->Size(curSize);
}


void SCProcedureType::Stop(void)
{
  SCProcedureIter iter(*instanceList);
  SCProcedure *   procedure;
  
  for (procedure = iter++;
       procedure;
       procedure = iter++)
  {
    procedure->Stop();
  }
}


SCBoolean SCProcedureType::Initialize (SCNatural tableSize)
{
  // allocate list and hash table for all procedure types

  procedureTypeList = new SCProcedureTypeList(false);
  assert(procedureTypeList);

  procedureTypeTable = new SCProcedureTypeTable(tableSize, false);
  assert(procedureTypeTable);

  return true;
}


SCBoolean SCProcedureType::Finish (void) // static!
{
  // delete list and hash table of procedure types

  if (procedureTypeList)
  {
    delete procedureTypeList;
    procedureTypeList = NULL;
  }

  if (procedureTypeTable)
  {
    delete procedureTypeTable;
    procedureTypeTable = NULL;
  }

  return true;
}


SCBoolean SCProcedureType::StaticSave(SCMem& saveArea)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Save(saveArea);
  }
  
  return true;
}


SCBoolean SCProcedureType::StaticRestore(SCMem &saveArea)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Restore(saveArea);
  }
  
  return true;
}


void SCProcedureType::StaticSize(SCSize *curSize)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Size(curSize);
  }
}


void SCProcedureType::StaticStop(void)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->Stop();
  }
}


SCBoolean SCProcedureType::operator==(const SCProcedureType &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCProcedureType::operator!=(const SCProcedureType &second) const
{
  return (GetID() != second.GetID());
}


#if _SC_VALIDATION_OPTIMIZE

void SCProcedureType::StaticSetModified(void)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->SetModified();
  }
}


void SCProcedureType::StaticUnsetModified(void)
{
  SCProcedureTypeIter iter(*procedureTypeList);
  SCProcedureType *   pType;

  for (pType = iter++;
       pType;
       pType = iter++)
  {
    pType->UnsetModified();
  }
}


void SCProcedureType::SetModified(void)
{
  SCProcedureIter iter(*instanceList);
  SCProcedure *   procedure;
  
  for (procedure = iter++;
       procedure;
       procedure = iter++)
  {
    procedure->SetModified();
  }
}


void SCProcedureType::UnsetModified(void)
{
  SCProcedureIter iter(*instanceList);
  SCProcedure *   procedure;
  
  for (procedure = iter++;
       procedure;
       procedure = iter++)
  {
    procedure->UnsetModified();
  }
}

#endif
