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
|   SCMachine   |  SCMachine.inl.h  | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCMACHINE_INL__

#define __SCMACHINE_INL__


#include "SCRequestType.h"


_SCINLINE_ SCDiscipline SCMachine::GetDiscipline (void) const
{
  return (discipline);
}


_SCINLINE_ SCNatural SCMachine::NumOfServers (void) const
{
  return (servers);
}


_SCINLINE_ SCNatural SCMachine::NumOfServices (void) const
{
  return (services);
}


_SCINLINE_ SCDuration *SCMachine::GetSpeeds (void) const
{
  return (speeds);
}


_SCINLINE_ const char *SCMachine::GetName (void) const
{
  return (name);
}


_SCINLINE_ SCRequestSaveList *SCMachine::GetServiceQueue (void) const
{
  return (serviceQueue);
}


_SCINLINE_ SCDuration SCMachine::Speed (const SCRequestType *requestType) const
{
  return (speeds[requestType->GetID()]);
}


_SCINLINE_ SCNatural SCMachine::NumOfFreeServers (void) const
{
  switch (GetDiscipline())
  {
    case scDiscIS: // should never be called!
      return NumOfServers();
    case scDiscPS:
      // for PS machines it IS possible that the number
      // of elements in the service queue is greater
      // than the number of servers!!
      if (GetServiceQueue()->NumOfElems() >= NumOfServers())
      {
        return 0;
      }
      // no break here!
    default:
      return (NumOfServers() - GetServiceQueue()->NumOfElems());
  }
}

_SCINLINE_ SCMachine * SCMachine::GetMachineFromName (const char * name)
{
  SCMachineIter iter(*machineList);
  SCMachine *   machine;
  
  if (name == NULL)
    return NULL;
    
  for (machine = iter++;
       machine;
       machine = iter++)
  {
    if (!strcmp(machine->GetName(), name))
      return machine;
  }
  
  return NULL;
}


#endif  // __SCMACHINE_INL
