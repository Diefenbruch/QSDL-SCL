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
| SCType        |  SCType.inl.h     | 26. Jul 1995  |   SCL             |       |
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

#ifndef __SCNAMETYPE_INL__

#define __SCNAMETYPE_INL__


_SCINLINE_ SCNatural SCType::GetID (void) const
{
  return (typeID);
}


_SCINLINE_ const char *SCType::GetName (void) const
{
  return (typeName);
}


_SCINLINE_ SCStream& SCType::Display (SCStream &pstream) const
{
  return (pstream << *this);
}

#endif  // __SCNAMETYPE_INL__
