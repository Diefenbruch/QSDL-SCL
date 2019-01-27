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
| SCPath        | SCPath.h          | 30. Aug 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     30.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCPATH__

#define __SCPATH__

#include "SCBasicTypes.h"
#include "SCRunnable.h"
#include "SCDelayedSignalList.h"

class SCPath : public SCRunnable
{
  public:
                               SCPath (void);
    virtual                   ~SCPath (void);

    static SCPath *            GetPath (void) { return path; }

    void                       SpawnDelayed (class SCProcess *receiver,
                                             class SCSignal *signal,
                                             const SCTime &pWhen);

    void                       SpawnDelayed (class SCProcessType *receiverType,
                                             class SCSignal *signal,
                                             const SCTime &pWhen);

    void                       Reschedule (void);

    SCDelayedSignalSaveList *  GetDelayQueue (void) const { return delayQueue; }

    friend SCStream &           operator<< (SCStream& pStream,
                                           const SCPath& pData);
    virtual SCStream &          Display (SCStream& pStream) const;

    SCBoolean                  Save (SCMem &) const;
    SCBoolean                  Restore (SCMem &);

    void                       Size (SCSize *pSize) const;

  private:
    void                       Body (void);

    SCDelayedSignalSaveList *  delayQueue;
    
    static SCPath *            path;
};


#endif  //  __SCPATH__
