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
| SCMutexTypes  | SCMutexTypes.h    | 5. Jul 1994   |   SCL             |       |
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


#ifndef __SCMUTEXTYPES__

#define __SCMUTEXTYPES__

#include "SCEnvironment.h"


#define SCPosixThreads       1
#define SCMacintoshThreads   2


#ifdef  _SC_THREAD_TYPE

  #if (_SC_THREAD_TYPE == SCPosixThreads)
    #include "SCPosixMutex.h"
  #endif  //  SCPosixThreads

  #if (_SC_THREAD_TYPE == SCMacintoshThreads)
    #include "SCMacMutex.h"
  #endif  //  SCMacintoshThreads

#else   // _SC_THREAD_TYPE not defined
  #error "_SC_THREAD_TYPE not defined"
#endif


#endif  //  __SCMUTEXTYPES__
