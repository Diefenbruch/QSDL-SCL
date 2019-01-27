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
| SCMachineInf  |   SCMachineInf.h  | 03. Nov 1996  |   SCL             |       |
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


#ifndef __SCMACHINEINF__

#define __SCMACHINEINF__

#include "SCMachine.h"

#define kSCInfiniteServers 0

class SCMachineInf : public SCMachine
{
  public:
                          SCMachineInf (SCRunnableID pMachineID,
                                        SCDiscipline pDiscipline,
                                        SCNatural pServers,
                                        SCNatural pServices,
                                        const SCDuration *const pSpeeds,
                                        const char *pName,
                                        SCObject *parent = NULL);
    virtual              ~SCMachineInf (void);

    friend SCStream&       operator<< (SCStream& pStream,
                                      const SCMachineInf& pData);
    virtual SCStream&      Display (SCStream &pStream) const;

    SCBoolean             HasWaitingQueue (void) const { return false; }
    void                  Body (void);
    
    SCBoolean             Save (SCMem &storage) const;
    SCBoolean             Restore (SCMem &storage);
    void                  Size(SCSize *pSize) const;

  protected:
    void                  StartRequest (SCRequest* request);
    void                  UpdateWork (void);      // decrease remaining time
                                                  // and remaining service for
                                                  // all request that have been
                                                  // served since last break
    void                  RecalcTimes (void);     // Recalculate the remaining
                                                  // time for all requests
                                                  // because load dependend
                                                  // speed of machine has
                                                  // changed (only PS)
  private:
    SCReal                relativeSpeed;          // Current relative speed of
                                                  // the machine. For IS always 1.
                                                  // For PS load dependend (see
                                                  // implementation of RecalcTimes())
};


#endif  //  __SCMACHINEINF__
