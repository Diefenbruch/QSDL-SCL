/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|SCPosixMainThread|SCPosixMainThread.h|  04.06.1996 |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     04.06.96     Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCMAINTHREAD__

#define __SCMAINTHREAD__

/* This should automagically include the correct header */

#include "SCThreadTypes.h"

class SCMainThread : public SCSystemThread
{
  public:
                          SCMainThread (void);
                         ~SCMainThread (void);

           void           Suspend (void);
           void           Resume (void);

    static void           Initialize (void);  // Initialize must be called exactly
                                              // once at the start of the program
    static void           Finish (void);      // Finish must be called exactly
                                              // once at the end of the program

    static SCMainThread * GetMainThread (void) { return mainThread; }

    virtual SCStream &     Display(SCStream& pStream) const { return pStream; }

  private:
           void           Body (void) { assert(false); } // should never be called!

    static SCMainThread * mainThread;
};


#endif // __SCMAINTHREAD__
