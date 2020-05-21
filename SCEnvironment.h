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
| SCEnvironment | SCEnvironment.h   | 23. Mar 1995  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     23.03.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#ifndef __SCENVIRONMENT__

#define __SCENVIRONMENT__


/*              !!! ALL FILES SHOULD #include THIS FILE !!!             */


/* Please change the following #defines to reflect the configuration of your
 * machine. Please note that several compilers can be detected automagically!
 * The automatic detection is the preferred method.
 * Currently supported platforms and compilers:
 *  Macintosh, MacOS 7.1 w/ThreadManager, MacOS 7.5:
 *      Metrowerks CodeWarrior >= CW6 (1.2.2)
 *      recommended: PowerMacintosh, MacOS 7.5, CW11 (IDE 1.7.4)
 *  SUN, SunOS 5.x (aka Solaris 2.x): x >= 5
 *      GNU g++ >= 2.6.3
 *      recommended: g++ >= 2.7.2
 *  Any UNIX-Machine w/POSIX-Threads
 *      Any c++-Compiler (GNU g++ recommended)
 */

/* This is the master flag for debugging. Set it to 1 and SCL will be in full
 * debugging mode. If it is set to 0, the individual flags below will define
 * the debugging mode.
 */

#define _SC_DEBUGMASTERSWITCH  0


/* ********************  F E A T U R E   S W I T C H E S  ******************** */

/* Control the use of drand48 for SCRandom. If set to 0, the own implementation
 * will be used. If set to 1, drand48 will be used.
 */

#define _SC_USEDRAND48 1


/* ********************  D E B U G G I N G   S W I T C H E S  ******************** */

/* This flag is similar to the range checking option of most Pascal compilers.
 * If set to 1, it enables range checking code in the provided data types.
 * SCByte is not affected by this.
 */

#define _SC_RANGECHECK  1


/* This flag is translated into the NDEBUG #define internally in the SCL sources.
 * If you set it to 1, additional assertions will be raised by the library!
 */

#define _SC_ASSERT  0


/* This flag controls the generation of inline function definitions. Generally
 * spoken, all of the simple accessor functions for private members and some
 * simple modifier functions are implemented as inlines. Set this to 1 to generate
 * functions calls instead of inline code. This is useful during debugging.
 * This flag will always be set to 1, when _SC_ASSERT is set to 1.
 * see also below: if not building the SCL but including it from the generated code
 * it will be redfined to be 1
 */

#define _SC_NOINLINES  1


/* This flag controls if central data structures like hash tables and
 * lists are locked with mutexes for shared access. Actually you don't
 * need locking until a parallelized version of the SCL is build in future.
 */

#define _SC_LOCKDATA  0


/* This flag controls internal debugging output for several SCL classes. Under
 * certain circumstances this might change the specified behavior of the library!
 * Leave it as it is unless you're sure what you're doing!!!
 */

#define _SC_DEBUGOUTPUT  0


/* This flag controls the use of dmalloc. dmalloc is used for finding memory
 * leaks or range violations by memory operations.
 */

#define _SC_DMALLOC  0


/* This flag enables thread specific tracing messages.
 * Currently implemented for POSIX threads.
 */

#define _SC_THREAD_DEBUG  0


/* This flag enables simulation specific tracing messages.
 */

#define _SC_SIMULATION_DEBUG  0


/* This flag enables validation specific tracing messages.
 * Valid values: 0, 1, 2 or 3
 */

#define _SC_VALIDATION_DEBUG  3


/* This flag changes the handling of time
 * for state space explorations
 * Valid values: 0 = no time informations in system states
 *               1 = keep only relative time informations
 *                   in system state
 *               2 = full time informations in system state
 *                   (not yet supported)
 */

#define _SC_VALIDATION_TIME  0


/*
 * This flag enables verification specific tracing output.
 */
 
#define _SC_VERIFICATION_DEBUG 0


/* This flag enables stack trace specific tracing signals.
 */

#define _SC_STACKTRACE_DEBUG  0


/* This flag enables socket specific debugging output.
 */

#define _SC_SOCKET_DEBUG  0


/* This flag enables sensor specific debugging output.
 */

#define _SC_SENSOR_DEBUG  0



/* This flag enables validation specific optimizations.
 * Valid values: 0 = no optimization
 *               1 = no restore of unchanged runnables   (Restore Optimization)
 *               2 = 1 + no restore of unchanged queues  (Restore Optimization)
 *                     + on the fly size calculation
 *               3 = 2 + use Save-Area of previous stack elem for save
 *                       if runnable is unchanged        (Save Optimization)
 */

#define _SC_VALIDATION_OPTIMIZE 3


/* This flag enables printing of hash marks to show simulation/validation
 * progress
 */

#define _SC_SHOW_PROGRESS 1


/*
 * Defines the number of states after which a progress mark is printed
 */

#define _SC_PROGRESS_STEP 1000


/*
 * Use this to control the generation of simple profiling information
 * in the code.
 */

#define _SC_PROFILING 0


/* Set individual debugging flags to reflect the master switch
 */

#if _SC_DEBUGMASTERSWITCH
  #undef  _SC_RANGECHECK
  #undef  _SC_ASSERT
  #undef  _SC_NOINLINES
  #undef  _SC_DEBUGOUTPUT
  #undef  _SC_DMALLOC
  #undef  _SC_VALIDATION_DEBUG
  #undef  _SC_THREAD_DEBUG

  #define _SC_RANGECHECK       1
  #define _SC_ASSERT           1
  #define _SC_NOINLINES        1
  #define _SC_DEBUGOUTPUT      1
  #define _SC_THREAD_DEBUG     1
  #define _SC_VALIDATION_DEBUG 1
  #define _SC_DMALLOC          0
#endif


// Manual #define for POSIX-Threads
#define _SC_THREAD_TYPE     SCPosixThreads
#define _SC_PLATFORM_UNIX   1

// Any GNU-Compiler ?
#if defined(__GNUC__)
  #define _SC_BOOL_BUILTIN    1
#endif

/* After looking for the platform, we #define the untouched macros to be 0
 */
#ifndef _SC_PLATFORM_MAC
  #define _SC_PLATFORM_MAC  0
#endif
#ifndef _SC_PLATFORM_UNIX
  #define _SC_PLATFORM_UNIX 0
#endif
#ifndef _SC_BOOL_BUILTIN
  #define _SC_BOOL_BUILTIN  0
#endif
#ifndef _SC_PROFILER_SUPPORT
  #define _SC_PROFILER_SUPPORT 0
#endif

/* Configure template expansion. Currently Sun's CC does automatic expansion and
 * thus needs the *.tmpl.h whereever they might be useful. We assume this for
 * every Unix-Compiler but GNU gcc.
 */
#if _SC_PLATFORM_UNIX && !__GNUC__
  #define _SC_NEEDS_TMPL_H
#endif


/* Configure assertion for library only. This line interacts with the end lines
 * of <SCL.h>. If _SC_ASSERT is set, assert macros will be expanded during com-
 * pilation of the library, SCL.h resets NDEBUG and the assertions to whatever
 * it was before. The previous state is saved in the preprocessor variable
 * __SC_OLDASSERTSTATE
 */

#if _SC_ASSERT
/* We will have assertions */
  #undef  _SC_NOINLINES
  #define _SC_NOINLINES   1

  #ifdef  NDEBUG
    #define __SC_OLDASSERTSTATE 0
    #undef  NDEBUG
  #else
    #define __SC_OLDASSERTSTATE 1
  #endif
#else
/* No assertions */
  #ifdef  NDEBUG
    #define __SC_OLDASSERTSTATE 0
  #else
    #define NDEBUG
    #define __SC_OLDASSERTSTATE 1
  #endif
#endif

#include <assert.h>

/* If not building SCL we disable inlining */

#ifndef _SC_BUILD
  #undef _SC_NOINLINES
  #define _SC_NOINLINES 1
#endif

/* Configure _SCINLINE_ function generation */
#if _SC_NOINLINES
  #define _SCINLINE_
  #define _SCINLINEC_ extern
#else
  #define _SCINLINE_  inline
  #define _SCINLINEC_ static inline
#endif


/* Configure mutable for Mac (compilers do not know it) */
#if _SC_PLATFORM_MAC || !__GNUC__
  #define mutable
#endif


/* To get the same results with hashing
 * on all plattforms it is useful to
 * use a definite byte order. To to so,
 * you can set following macro to 1.
 * NOTE: this may slowdown the hash
 * algorithms on plattforms which
 * don't use the standard byte order!
 * For transformation the system macro
 * htonl is used (see man page for further
 * details!) This macro is used in SCHashTable.cpp
 */

#define _SC_USE_STANDARD_BYTEORDER 0

#endif  // __SCENVIRONMENT__
