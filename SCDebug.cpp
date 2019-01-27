/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCDebug     |   SCDebug.cc      | 21. Aug 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     21.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCDebug.h"


#if _SC_DEBUGOUTPUT

SCStream scDebugLog ("SCL.log");

#endif


#if _SC_THREAD_DEBUG

SCStream scThreadDebugLog ("THREAD.log");

#endif


#if _SC_VALIDATION_DEBUG

SCStream scValidationDebugLog ("VALIDATION.log");

#endif


#if _SC_VERIFICATION_DEBUG

SCStream scVerificationDebugLog ("VERIFICATION.log");

#endif


#if _SC_SIMULATION_DEBUG

SCStream scSimulationDebugLog ("SIMULATION.log");

#endif


#if _SC_STACKTRACE_DEBUG

SCStream scStackTraceDebugLog ("STACKTRACE.log");

#endif


#if _SC_SOCKET_DEBUG

SCStream scSocketDebugLog ("SOCKET.log");

#endif


#if _SC_SENSOR_DEBUG

SCStream scSensorDebugLog ("SENSOR.log");

#endif
