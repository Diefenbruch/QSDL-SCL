/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------------+----------------------+---------------+-----------------+
|   Module            |   File               |   Created     |   Project       |
+---------------------+----------------------+---------------+-----------------+
|  SCEnabledTransition|SCEnabledTransition.cc| 25. Feb 1996  |   SCL           |
+---------------------+----------------------+---------------+-----------------+
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

#include "SCStream.h"
#include "SCEnabledTransition.h"
#include "SCSignal.h"
#include "SCTransition.h"

#if _SC_NOINLINES
  #include "SCEnabledTransition.inl.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCEnabledTransition::SCEnabledTransition (SCTransition *pTrans,
                                          SCSignal *pMsg) :
  SCObject(SC_ENABLED_TRANSITION, NULL),
  trans (pTrans)
{
  if (pMsg)
    sigID = pMsg->GetSignalID();
  else
    sigID = 0;
}


SCEnabledTransition::SCEnabledTransition (const SCEnabledTransition &orig) :
  SCObject(SC_ENABLED_TRANSITION, NULL),
  trans (orig.trans),
  sigID(orig.sigID)
{
}


SCEnabledTransition::~SCEnabledTransition (void)
{
}


SCSignal *SCEnabledTransition::GetSignal (void) const
{
  if (!sigID) return NULL;

  return SCSignal::GetSignalTable()->Find(sigID);
}


SCStream& operator<< (SCStream& pStream, const SCEnabledTransition& pData)
{
  if (pData.GetTransition())
    pStream << *pData.GetTransition();
  else
    pStream << "implicit transition";
    
  if (pData.GetSignal())
    pStream << " consuming " << *pData.GetSignal();

  return (pStream);
}


SCStream& SCEnabledTransition::Display (SCStream &pstream) const
{
  return (pstream << *this);
}
