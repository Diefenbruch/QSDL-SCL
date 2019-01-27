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


#ifndef __SCPOSIXTHREAD__
#define __SCPOSIXTHREAD__

#include "SCThread.h"

extern "C" {
#include <pthread.h>
}

class SCSystemThread : public SCThread
{
  public:
                        SCSystemThread (void);
                        SCSystemThread (const SCObjectType newType,
                                        const SCObject* pParent);
                                        
    virtual            ~SCSystemThread (void);

    static SCThreadID   ExecutingThreadID (void); // Should return some unique
                                                  // thread identity key of the
                                                  // thread executing this method

    static void         Initialize (void);        // Initialize must be called exactly
                                                  // once at the start of the program

    SCBoolean           IsRunning(void) const;    // True, if thread is still
                                                  // active, false otherwise.
    void                Cancel (void);            // How to cancel a thread
    void                Join (void);              // How to join a thread
    
  protected:
    static void *       Execute (SCSystemThread *toExecute);
                                                  // This method is called by
                                                  // the operating system
    static void *       CleanUp (SCSystemThread *toDelete);
                                                  // Clean up handler for
                                                  // thread is called
                                                  // whenever the thread
                                                  // exits, either regularly
                                                  // or due to a cancel
    pthread_mutex_t     mutexSleep;
    pthread_cond_t      condSleep;
    int                 runIt;                    // indicates if the thread
                                                  // belonging to this object
                                                  // is running. We MUST use
                                                  // an integer here!
    friend class SCMainThread;
};

#endif  //  __SCPOSIXTHREAD__
