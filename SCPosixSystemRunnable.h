/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1995 Marc Diefenbruch, Wolfgang Textor                                  |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCPosixThread |  SCPosixThread.h  | 7. Mai 1995   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     05.07.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef _SCSYSTEMRUNNABLE_

#define _SCSYSTEMRUNNABLE_

/* This should automagically include the correct header */

#include "SCThreadTypes.h"

class SCSystemRunnable : public SCSystemThread
{
  public:
                      SCSystemRunnable (const SCObjectType newType = SC_NONE,
                                        const SCObject* pParent = NULL);
                                        
    virtual          ~SCSystemRunnable (void);

    void              Suspend (void);                 // How to suspend a thread
    void              Resume (void);                  // How to resume a suspended thread

    static SCThread * GetInteractionThread(void);     // partner thread (at startup
                                                      // this should be the main thread
                                                      // later the scheduler thread)
    static void       SetInteractionThread(SCThread *iThread);

  private:
    static SCThread * interactionThread;
};

#endif  //  _SCSYSTEMRUNNABLE_
