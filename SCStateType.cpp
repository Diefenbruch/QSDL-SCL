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
| SCStateType   |  SCStateType.cc   | 2. Apr 1995   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     02.04.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStateType.h"

#include "SCTransition.h"
#include "SCTransitionList.h"
#include "SCSignalType.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCStream.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCStateType.inl.h"
#endif


SCStateTypeTable *SCStateType::stateTypeTable = NULL;


SCStateType::SCStateType (SCNatural stateID,
                          const char *pName,
                          const SCNatural pSaveSetSize,
                          const SCSignalID *const pSaveSet,
                          const SCBoolean pSaveAll,
                          const SCBoolean pIntermediate,
                          const SCObject* father) :
  SCType (stateID, pName, father),
  saveSetSize(pSaveSetSize),
  saveSet(pSaveSet),
  saveAll(pSaveAll),
  maxTransitionID(kSCNoTransition),
  reached(false),
  intermediateState(pIntermediate)
{
  SCNatural z_u;
  SCSignalType *signalType;

  // Insert this state type instance into static hash table of
  // all instances:

  assert(stateTypeTable);
  stateTypeTable->Insert (stateID, this);

  normalInputsTable = new SCTransitionListTable (kSCDefaultTableSize, true);
  priorityInputsTable = new SCTransitionListTable (kSCDefaultTableSize, true);

  /**********************************************************/
  /* The elements of the following list are dynamic objects */
  /* allocated (but not freed) in the generated code, so we */
  /* must tell the list that they must delete the elements  */
  /* if the list itself is deleted!                         */
  /**********************************************************/
  normalInputs = new SCTransitionList (true);
  priorityInputs = new SCTransitionList (true);
  inputNones = new SCTransitionList (true);
  contSignals = new SCTransitionList (true);

  if (saveSet != NULL)
  {
    assert (saveSetSize > 0);
    saveSetTable = new SCSignalTypeTable (saveSetSize, false);
    assert (saveSetTable);

    for (z_u = 0;
         z_u < saveSetSize;
         z_u++)
    {
      assert (SCSignalType::GetSignalTypeTable());

      signalType = SCSignalType::GetSignalTypeTable()->Find (saveSet[z_u]);
      assert (signalType);

      saveSetTable->Insert (saveSet[z_u], signalType);
    }
  }
  else
    saveSetTable = NULL;

  if (father && father->GetType() == SC_TYPE)
  {
    if (father->GetParent())
    {
      ((SCProcedureType *)father)->GetStateList()->InsertAfter(this);
    }
    else
    {
      ((SCProcessType *)father)->GetStateList()->InsertAfter(this);
    }
  }
}


SCStateType::~SCStateType (void)
{
  delete normalInputs;
  delete priorityInputs;
  delete normalInputsTable;
  delete priorityInputsTable;
  if (saveSetTable) delete saveSetTable;
  delete inputNones;
  delete contSignals;

  assert(stateTypeTable);
  stateTypeTable->Remove (GetID());
}


void SCStateType::AddTransition (SCTransition *pTransition)
{
  SCTransitionCons * iter;
  SCTransitionList * transList;
  SCNatural          z_i;
  const SCSignalID * inputSet;
  SCNatural          inputSetSize;

  if (pTransition->GetID() > maxTransitionID)
    maxTransitionID = pTransition->GetID();

  // find correct list for pTransition and insert it:

  if (pTransition->GetInputSetSize() == -1) // no signals for transition?
  {
    if (pTransition->GetPriority() == kSCPrioInputNone)
    {
      inputNones->InsertAfter (pTransition);
    }
    else // continuous signal transition!
    {
      // sort by descending priorities:
      for (iter = contSignals->Head();
           iter;
           iter = iter->Next())
      {
        if (pTransition->GetPriority() > (*iter)()->GetPriority())
          break;
      }
      contSignals->InsertBefore (pTransition, iter);
    }
  }
  else if (pTransition->GetPriority() == kSCPrioPriorityInput)
  {
    // start of speedup code for SCProcess::GetActiveTransSignal()
    inputSetSize = pTransition->GetInputSetSize();
    inputSet = pTransition->GetInputSet();
    assert(inputSet);
    for (z_i = 0; z_i < inputSetSize; z_i++)
    {
      transList = priorityInputsTable->Find(inputSet[z_i]);
      if (!transList) // first transition for this signal?
      {
        transList = new SCTransitionList(false);
        assert(transList);

        transList->InsertAfter (pTransition);
        priorityInputsTable->Insert (inputSet[z_i], transList);
      }
      else           // list is present, so other transitions
                     // for this signal are already inserted
        transList->InsertAfter (pTransition);
    }
    // end of speedup code for SCProcess::GetActiveTransSignal()

    priorityInputs->InsertAfter (pTransition);
  }
  else
  {
    // start of speedup code for SCProcess::GetActiveTransSignal()
    inputSetSize = pTransition->GetInputSetSize();
    inputSet = pTransition->GetInputSet();
    assert(inputSet);
    for (z_i = 0; z_i < inputSetSize; z_i++)
    {
      transList = normalInputsTable->Find(inputSet[z_i]);
      if (!transList) // first transition for this signal?
      {
        transList = new SCTransitionList(false);
        assert(transList);

        transList->InsertAfter (pTransition);
        normalInputsTable->Insert (inputSet[z_i], transList);
      }
      else
        transList->InsertAfter (pTransition);
    }
    // end of speedup code for SCProcess::GetActiveTransSignal()

    normalInputs->InsertAfter (pTransition);
  }
}


SCStream& operator<< (SCStream& pStream, const SCStateType& pData)
{
  pStream << "state ";
  return pData.Display(pStream);
}


SCBoolean SCStateType::Initialize (SCNatural tableSize) // static !
{
  // allocate table for all state types

  stateTypeTable = new SCStateTypeTable (tableSize, false);
  assert (stateTypeTable);

  return true;
}


SCBoolean SCStateType::Finish (void) // static!
{
  // delete table of state types

  if (stateTypeTable)
  {
    delete stateTypeTable;
    stateTypeTable = NULL;
  }

  return true;
}


SCBoolean SCStateType::operator==(const SCStateType &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCStateType::operator!=(const SCStateType &second) const
{
  return (GetID() != second.GetID());
}
