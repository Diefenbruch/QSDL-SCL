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
|SCMachineLCFSPP| SCMachineLCFSPP.h | 13. Jul 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     11.07.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCMACHINELCFSPP__

#define __SCMACHINELCFSPP__


#include "SCMachineLtd.h"


class SCMachineLCFSPP : public SCMachineLtd
{
  public:
                          SCMachineLCFSPP (SCRunnableID pMachineID,
                                           SCNatural pServers,
                                           SCNatural pServices,
                                           const SCDuration *const pSpeeds,
                                           const char *pName,
                                           SCObject *father = NULL);
    virtual              ~SCMachineLCFSPP (void);

    virtual void          NewRequest (class SCAutomaton *caller,
                                      const class SCRequestType *serviceType,
                                      SCDuration serviceAmount,
                                      SCNatural priority);

    friend SCStream&       operator<< (SCStream& pStream,
                                      const SCMachineLCFSPP& pData);
    virtual SCStream&      Display (SCStream &pStream) const;

    SCBoolean             Save (SCMem &storage) const;
    SCBoolean             Restore (SCMem &storage);
    void                  Size (SCSize *pSize) const;

  protected:
    virtual void          EnqueueRequest (SCRequest* request);
};


#endif  //  __SCMACHINELCFSPP__
