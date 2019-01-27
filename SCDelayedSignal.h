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
|   SCSignal    |   SCSignal.h      | 24. Feb 1996  |   SCL             |       |
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

#ifndef __SCDELAYEDSIGNAL__

#define __SCDELAYEDSIGNAL__

#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCDelayedSignalList.h"

class SCDelayedSignal: public SCObject
{
  public:
                               SCDelayedSignal (class SCSignal *pSignal,
                                                const SCProcessID pReceiverID = kSCNoProcessID,
                                                const class SCProcessType *const pReceiverType = NULL,
                                                const SCTime pDelayedUntil = 0.0,
                                                const SCObject* pParent = NULL);

                               SCDelayedSignal (SCMem & saveArea, // Constructor for Restore.
                                                const SCObject *pParent = NULL);
    virtual                   ~SCDelayedSignal (void);

    class SCSignal *           GetSignal (void) const { return signal; }
    class SCSignal *           RetrieveSignal (void);
    
    class SCProcess *          GetReceiver (void) const;
    const class SCProcessType *GetReceiverType (void) const;

    SCProcessID                GetReceiverID (void) const;
    void                       SetReceiverID (const SCProcessID pReceiverID);

    SCTime                     GetDelayedUntil (void) const { return delayedUntil; }

    SCDelayedSignalCons *      GetContainer (void) const { return container; }
    void                       SetContainer (SCDelayedSignalCons *cont) { container = cont; }

    friend SCStream &           operator<< (SCStream& pStream,
                                           const SCDelayedSignal& pData);
    virtual SCStream &          Display (SCStream &pStream) const;

    SCBoolean                  Save (SCMem &) const;
    SCBoolean                  Restore (SCMem &);
    void                       Size (SCSize *pSize) const;

  private:
    class SCSignal *           signal;            // embedded signal object
    SCProcessID                receiverID;        // Receiver of this signal
    class SCProcessType *      receiverType;      // important for MSC tracing:
                                                  // if receiver is stopped we can
                                                  // get the name of the receiver
    SCTime                     delayedUntil;      // Time, when this delayed sig should be sent
    SCDelayedSignalCons *      container;         // actual SCListCons elem
                                                  // in which signal is
                                                  // embedded (do not save!)
};


#if !_SC_NOINLINES
  #include "SCDelayedSignal.inl.h"
#endif


#endif  //  __SCDELAYEDSIGNAL__
