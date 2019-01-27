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
|  SCStateType  | SCStateType.inl.h | 28. Dez 1995  |   SCL             |      |
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

#ifndef __SCSTATETYPE_INL__

#define __SCSTATETYPE_INL__


_SCINLINE_ SCTransitionList *SCStateType::GetNormalInputs (void) const
{
  return (normalInputs);
}


_SCINLINE_ SCTransitionList *SCStateType::GetPriorityInputs (void) const
{
  return (priorityInputs);
}


_SCINLINE_ SCTransitionList *SCStateType::GetInputNones (void) const
{
  return (inputNones);
}


_SCINLINE_ SCTransitionList *SCStateType::GetContSignals (void) const
{
  return (contSignals);
}


_SCINLINE_ SCTransitionListTable * SCStateType::GetNormalInputsTable (void) const
{
  return normalInputsTable;
}


_SCINLINE_ SCTransitionListTable * SCStateType::GetPriorityInputsTable (void) const
{
  return priorityInputsTable;
}


_SCINLINE_ SCSignalTypeTable * SCStateType::GetSaveSetTable (void) const
{
  return saveSetTable;
}


_SCINLINE_ SCBoolean SCStateType::HasSpontaneous (void) const
{
  return (!inputNones->IsEmpty() || !contSignals->IsEmpty());
}


_SCINLINE_ SCTransitionID SCStateType::GetMaxTransitionID (void) const
{
  return (maxTransitionID);
}


_SCINLINE_ SCNatural SCStateType::GetSaveSetSize (void) const
{
  return (saveSetSize);
}


_SCINLINE_ const SCSignalID *SCStateType::GetSaveSet (void) const
{
  return (saveSet);
}


_SCINLINE_ SCBoolean SCStateType::GetSaveAll (void) const
{
  return (saveAll);
}


_SCINLINE_ SCBoolean SCStateType::IsReached (void) const
{
  return reached;
}


_SCINLINE_ void SCStateType::SetReached (void)
{
  reached = true;
}


_SCINLINE_ SCBoolean SCStateType::IsIntermediate (void) const
{
  return intermediateState;
}


_SCINLINE_ SCStateTypeTable * SCStateType::GetStateTypeTable(void)
{
  return stateTypeTable;
}


_SCINLINE_ void SCStateType::SetStateTypeTable(SCStateTypeTable *newTable)
{
  stateTypeTable = newTable;
}


#endif  // __SCSTATETYPE_INL__
