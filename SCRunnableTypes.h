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
|               | SCRunnableTypes.h | 4. Jun 1996   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     04.06.96    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCRUNNABLETYPES__

#define __SCRUNNABLETYPES__

#include "SCEnvironment.h"


#define SCPosixThreads       1
#define SCMacintoshThreads   2


#ifdef  _SC_THREAD_TYPE

  #if (_SC_THREAD_TYPE == SCPosixThreads)
    #include "SCPosixSystemRunnable.h"
  #endif  //  SCPosixThreads

  #if (_SC_THREAD_TYPE == SCMacintoshThreads)
    #include "SCMacSystemRunnable.h"
  #endif  //  SCMacintoshThreads

#else   // _SC_THREAD_TYPE not defined
  #error "_SC_THREAD_TYPE not defined"
#endif


#endif  //  __SCRUNNABLETYPES__
