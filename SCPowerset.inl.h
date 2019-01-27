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
|  SCPowerset   | SCPowerset.inl.h  | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCPOWERSET_INL__

#define __SCPOWERSET_INL__


_SCINLINE_ SCBoolean SCPowerset::operator!= (const SCPowerset& second) const
{
    return (!(*this == second));
}


_SCINLINE_ SCBoolean SCPowerset::operator> (const SCPowerset& subSet) const
{
    return (subSet < *this);
}


_SCINLINE_ SCBoolean SCPowerset::operator>= (const SCPowerset& subSet) const
{
    return (subSet <= *this);
}


_SCINLINE_ SCBoolean SCPowerset::Equal (const SCDataType &second) const
{
  return (*this == *(SCPowerset *)&second);
}


_SCINLINE_ SCDataType *SCPowerset::Clone (void) const
{
  return (new SCPowerset (*this));
}


#endif  // __SCPOWERSET_INL__
