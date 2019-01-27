/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-----------------------+---------------+-------------------+---+
|   Module      |       File            |   Created     |   Project         |   |
+---------------+-----------------------+---------------+-------------------+---+
|SCRandBernoulli| SCRandBernoulli.inl.h | 26. Jul 1995  |   SCL             |   |
+---------------+-----------------------+---------------+-------------------+---+
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


#ifndef __SCRANDBERNOULLI_INL__

#define __SCRANDBERNOULLI_INL__


_SCINLINE_ void SCRandBernoulli::Init (SCReal newProb)
{
  prob = newProb;
}


_SCINLINE_ SCRandBernoulli::SCRandBernoulli (SCReal p)
{
  Init (p);
}


_SCINLINE_ SCInteger SCRandBernoulli::operator() (void)
{
  SCReal x = Uniform();

  return (x <= prob ? true : false);
}


_SCINLINE_ void SCRandBernoulli::Draw (SCReal &var)
{
  var = (*this)();
}


_SCINLINE_ void SCRandBernoulli::Draw (SCInteger &var)
{
  var = (*this)();
}


#endif  // __SCRANDBERNOULLI_INL__
