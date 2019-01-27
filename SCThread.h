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
|   SCThread    |   SCThread.h      | 23. Mai 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     23.05.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCTHREAD__

#define __SCTHREAD__

#include "SCObject.h"

typedef long SCThreadID;

class SCThread: public SCObject
{
  public:
                        SCThread (const SCObjectType newType = SC_NONE,
                                  const SCObject* father = NULL);
    virtual            ~SCThread (void);

    virtual void        Suspend (void) = 0;       // How to suspend a thread
    virtual void        Resume (void) = 0;        // How to resume a suspended thread
    virtual void        Cancel (void) = 0;        // How to cancel a thread
    virtual void        Join (void) = 0;          // Wait for end of another thread
    virtual void        Body (void) = 0;          // The 'main' part of the code

    SCThreadID          Identity (void) const { return threadID; }
                                                  // Returns the ID of the thread
                                                  // associated with this thread
                                                  // object. NOTE:
                                                  // CurThreadID() != Identity()
                                                  // implies, that a thread other
                                                  // than the associated thread
                                                  // is executing code of this object!

  protected:
    SCThreadID          threadID;
};

#endif  // __SCTHREAD__
