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
|   SCDebug     |   SCDebug.h       | 21. Aug 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     21.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCDEBUG__

#define __SCDEBUG__


#include "SCEnvironment.h"
#include "SCStream.h"


#ifndef _SC_DEBUGOUTPUT
#define _SC_DEBUGOUTPUT 0
#endif


#if _SC_DEBUGOUTPUT

#include <fstream>

extern SCStream scDebugLog;

#endif


#if _SC_THREAD_DEBUG

#include <fstream>

extern SCStream scThreadDebugLog;

#endif


#if _SC_VALIDATION_DEBUG

#include <fstream>

extern SCStream scValidationDebugLog;

#endif


#if _SC_VERIFICATION_DEBUG

#include <fstream>

extern SCStream scVerificationDebugLog;

#endif


#if _SC_SIMULATION_DEBUG

#include <fstream>

extern SCStream scSimulationDebugLog;

#endif


#if _SC_STACKTRACE_DEBUG

#include <fstream>

extern SCStream scStackTraceDebugLog;

#endif


#if _SC_SOCKET_DEBUG

#include <fstream>

extern SCStream scSocketDebugLog;

#endif


#if _SC_SENSOR_DEBUG

#include <fstream>

extern SCStream scSensorDebugLog;

#endif


#endif  //  __SCDEBUG__
