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
| SCTransition  |SCTransition.inl.h | 28. Dez 1995  |   SCL             |       |
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

#ifndef __SCTRANSITION_INL__

#define __SCTRANSITION_INL__


_SCINLINE_ SCInteger SCTransition::GetPriority (void) const
{
  return (priority);
}


_SCINLINE_ SCInteger SCTransition::GetInputSetSize (void) const
{
  return (inputSetSize);
}


_SCINLINE_ SCSignalID *SCTransition::GetInputSet (void) const
{
  return (inputSet);
}


_SCINLINE_ SCStream& SCTransition::Display (SCStream &pstream) const
{
  return (pstream << *this);
}

#endif  // __SCTRANSITION_INL__
