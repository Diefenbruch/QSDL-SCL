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
| SCRandConst   | SCRandConst.inl.h | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCRANDCONST_INL__

#define __SCRANDCONST_INL__


_SCINLINE_ void SCRandConst::Init (SCReal newValue)
{
  value = newValue;
}


_SCINLINE_ SCRandConst::SCRandConst (SCReal v)
{
  Init (v);
}


_SCINLINE_ SCReal SCRandConst::operator() (void)
{
  return (value);
}


_SCINLINE_ void SCRandConst::Draw (SCReal &var)
{
  var = value;
}


_SCINLINE_ void SCRandConst::Draw (SCInteger &var)
{
  var = (SCInteger)value;
}

#endif  // __SCRANDCONST_INL__
