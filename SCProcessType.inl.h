/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCProcessType |SCProcessType.inl.h| 16. Aug 1995  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     --.--.--    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCPROCESSTYPE_INL__

#define __SCPROCESSTYPE_INL__


_SCINLINE_ SCNatural SCProcessType::GetMaxInputQueueLength (void) const
{
  return (maxInputQueueLen);
}


_SCINLINE_ void SCProcessType::SetStateList (SCStateTypeList *list)
{
  stateList = list;
}


_SCINLINE_ SCInteger SCProcessType::NumOfInstances (void) const
{
  if (instanceList)
    return instanceList->NumOfElems();
  else
    return 0;
}


_SCINLINE_ SCProcessType * SCProcessType::GetProcessTypeFromName (const char * name)
{
  SCProcessTypeIter iter(*processTypeList);
  SCProcessType *   processType;
  
  if (name == NULL)
    return NULL;
    
  for (processType = iter++;
       processType;
       processType = iter++)
  {
    if (!strcmp(processType->GetName(), name))
      return processType;
  }
  
  return NULL;
}

#endif  // __SCPROCESSTYPE_INL__
