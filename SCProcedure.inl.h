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
|   SCProcedure |  SCProcedure.inl.h| 26. Jul 1997  |   SCL             |       |
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


#ifndef __SCPROCEDURE_INL__

#define __SCPROCEDURE_INL__

#include "SCProcess.h"

_SCINLINE_ class SCProcedureType *SCProcedure::GetType (void) const
{
  return (procType);
}


_SCINLINE_ SCProcessID SCProcedure::Sender (void) const
{
  assert(GetOwner());
  
  return (GetOwner()->Sender());
}


_SCINLINE_ SCProcessID SCProcedure::Parent (void) const
{
  assert(GetOwner());
  
  return (GetOwner()->Parent());
}


_SCINLINE_ SCProcessID SCProcedure::Offspring (void) const
{
  assert(GetOwner());
  
  return (GetOwner()->Offspring());
}


_SCINLINE_ SCProcessID SCProcedure::Self (void) const
{
  assert(GetOwner());
  
  return (GetOwner()->Self());
}


#endif  // __SCPROCEDURE_INL__
