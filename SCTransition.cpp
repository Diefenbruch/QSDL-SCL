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
| SCTransition  | SCTransition.cc   | 05. Dez 1995  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     95.12.05    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCTransition.h"
#include "SCStateType.h"


#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


#if _SC_NOINLINES
  #include "SCTransition.inl.h"
#endif


SCTransition::SCTransition (SCNatural tid,
                            SCStateType *pState,
                            SCInteger pInputSetSize,
                            SCSignalID *pInputSet,
                            SCInteger pPriority,
                            const SCObject* father) :
  SCObject (SC_TRANSITION, father),
  id(tid),
  priority(pPriority),
  inputSetSize(pInputSetSize),
  inputSet(pInputSet),
  executed(false)
{
  pState->AddTransition (this);
}


SCStream& operator<< (SCStream& pStream, const SCTransition& pData)
{
  pStream << "transition #" << pData.GetID();

  return (pStream);
}


SCBoolean SCTransition::operator==(const SCTransition &second) const
{
  return (id == second.id);
}


SCBoolean SCTransition::operator!=(const SCTransition &second) const
{
  return (id != second.id);
}
