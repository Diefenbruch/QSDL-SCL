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
|   SCTrace     | SCTrace.inl.h     | 27. Jul 1995  |   SCL             |      |
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

#ifndef __SCTRACEOBJ_INL__

#define __SCTRACEOBJ_INL__


_SCINLINE_ SCInteger SCTrace::GetActionFlags (void) const
{
  return actionFlags;
}


_SCINLINE_ SCTraceCons * SCTrace::GetContainer (void) const
{
  return container;
}

_SCINLINE_ void SCTrace::SetContainer (SCTraceCons *cont)
{
  container = cont;
}


#endif  // __SCTRACEOBJ_INL__
