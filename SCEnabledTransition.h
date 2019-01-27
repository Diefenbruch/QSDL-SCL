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
|SCEnabledTransition| SCEnabledTransition.h | 25. Feb 1996 |  SCL     |       |
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

#ifndef __SCENABLEDTRANSITION__

#define __SCENABLEDTRANSITION__

#include "SCBasicTypes.h"
#include "SCObject.h"


class SCEnabledTransition: public SCObject
{
  public:
                          SCEnabledTransition (class SCTransition *pTrans,
                                               class SCSignal *pMsg);
                          SCEnabledTransition (const SCEnabledTransition &orig);
                         ~SCEnabledTransition (void);

    class SCTransition *  GetTransition (void) const;
    class SCSignal *      GetSignal (void) const;

    friend SCStream &      operator<< (SCStream& pStream,
                                      const SCEnabledTransition& pData);
    virtual SCStream &     Display (SCStream& pStream) const;

  private:
    class SCTransition *  trans;
    SCNatural             sigID;
};

#if !_SC_NOINLINES
  #include "SCEnabledTransition.inl.h"
#endif

#endif  // __SCENABLEDTRANSITION__
