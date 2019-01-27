/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-95 Wolfgang Textor                                                 |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCMacThread   |  SCMacThread.h    | 5. Jul 1994   |   SCL             |       |
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


#ifndef __SCMACTHREAD__

#define __SCMACTHREAD__


#include "SCThread.h"

#if _SC_PROFILER_SUPPORT
  #include <profiler.h>
#endif


class SCSystemThread : public SCThread
{
  public:
                        SCSystemThread (void);
                        SCSystemThread (const SCObjectType newType,
                                        const SCObject *pParent);
    virtual            ~SCSystemThread (void);

    static SCThreadID   ExecutingThreadID (void);      // Should return some unique thread
                                                       // identity key

    static void         Initialize (void);             // Initialize must be called exactly
                                                       // once at the start of the program

    SCBoolean           IsRunning(void) { return false; }; // Needed for concurrency.
                                                           // Mac is not concurrent at this time.

    void                Cancel (void);                 // How to cancel a thread
    void                Join (void);              // How to join a thread

  protected:
    static pascal void *Execute (SCSystemThread *toExecute);
                                                       // This routine calls the 'main' object code
    static pascal void *CleanUp (SCThreadID thread, SCSystemThread *toDelete);
                                                       // Clean up handler for
                                                       // thread is called
                                                       // whenever the thread
                                                       // exits, either regularly
                                                       // or due to a cancel
    #if _SC_PROFILER_SUPPORT
      static pascal void  SwitchIn (SCThreadID thread, SCSystemThread* myself);
      static pascal void  SwitchOut (SCThreadID thread, SCSystemThread* myself);
      ProfilerThreadRef   profilerThreadID;           // Thread reference for MW Profiler
    #endif
    friend class SCMainThread;
};


#endif  //  __SCMACTHREAD__
