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
| SCMachineLtd  |   SCMachineLtd.h  | 03. Nov 1996  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     03.11.96    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCMACHINELTD__

#define __SCMACHINELTD__

#include "SCMachine.h"

class SCMachineLtd : public SCMachine
{
  public:
                          SCMachineLtd (SCRunnableID pMachineID,
                                        SCDiscipline pDiscipline,
                                        SCNatural pServers,
                                        SCNatural pServices,
                                        const SCDuration *const pSpeeds,
                                        const char *pName,
                                        SCObject *parent = NULL);
    virtual              ~SCMachineLtd (void);

    SCBoolean             HasWaitingQueue (void) const { return true; }

    SCRequestSaveList *   GetWaitingQueue (void) const { return waitingQueue; }

    friend SCStream&       operator<< (SCStream& pStream,
                                      const SCMachineLtd& pData);
    virtual SCStream&      Display (SCStream &pStream) const;

    SCBoolean             Save (SCMem &storage) const;
    SCBoolean             Restore (SCMem &storage);
    void                  Size(SCSize *pSize) const;

    virtual void          NewRequest (class SCAutomaton *caller,
                                      const class SCRequestType *serviceType,
                                      SCDuration serviceAmount,
                                      SCNatural priority) = 0;

  protected:
    virtual void          EnqueueRequest (SCRequest *request) = 0;
    SCBoolean             InterruptRequest (SCRequest *request, SCBoolean lcfs);
    void                  StartRequest (void);
    void                  UpdateWork (void);
    void                  Body (void);

  private:
    SCRequestSaveList *   waitingQueue;   // Liste der momentan wartenden
                                          // Requests
};


#endif  //  __SCMACHINELTD__
