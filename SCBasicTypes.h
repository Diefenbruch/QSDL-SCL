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
| SCBasicTypes  |   SCBasicTypes.h  | 9. Aug 1994   |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     09.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCBASICTYPES__

#define __SCBASICTYPES__


#include "SCEnvironment.h"
#include <stddef.h>                 // Standard Defs (like NULL macro)
#include <limits.h>

#define SCL_VERSION "1.4"
#define SCL_SIMULATOR_VERSION "1.3.0"
#define SCL_VALIDATOR_VERSION "0.6.0"
#define SCL_VERIFIER_VERSION "0.1.0"

#if !_SC_BOOL_BUILTIN
  typedef unsigned char   bool;

  #ifndef true
    #define true  1
    #define false 0
  #endif

#endif

// don't use bool since it
// has different sizes on x86 and sparc:
// x86: 1
// sparc: 4 (why ???)

//typedef bool            SCBoolean;
typedef unsigned char   SCBoolean;
typedef char            SCCharacter;
typedef long            SCInteger;
typedef unsigned long   SCNatural;
typedef double          SCReal;
typedef SCNatural       SCTransitionID;
typedef SCNatural       SCSignalID;
typedef SCNatural       SCRequestID;
typedef SCInteger       SCRunnableID;
typedef SCRunnableID    SCProcessID;
typedef SCRunnableID    SCPid;

#if (!_SC_RESTRICTEDTIME)
typedef SCReal          SCTime;
typedef SCReal          SCDuration;
#endif

typedef SCInteger  integer;
typedef SCBoolean  boolean;
typedef SCReal     real;
typedef SCDuration duration;
//typedef SCTime     time;
typedef SCNatural  natural;
typedef SCCharacter character;
typedef SCProcessID pid;

enum
{
/* Parameter-Konstanten */
  kSCNoTransition         = 0,
  kSCDecisionNone         = 0,
  kSCNoProcessID          = 0,
  kSCUnlimitedProcesses   = -1,
  kSCUnlimitedQueueLength = 0,
#define kSCNoAwakeDelay -1.0
/* Basis-IDs (von SCL benoetigt) */
  kSCProcessTypeIDBase    = 0,
  kSCProcedureTypeIDBase  = 0,
  kSCRequestTypeIDBase    = 0,
  kSCSignalTypeIDBase     = 0,
  kSCStateTypeIDBase      = 0,
  kSCTransitionIDBase     = 1,
/* Basis des Decision-Ergebnisses */
  kSCDecisionBase         = 1,
/* IDs fuer alle lauffaehigen Objekte */
  kSCProcessIDBase        = 1,
  kSCTimerProcessID       = -1,
  kSCPathProcessID        = -2,
  kSCMachineIDBase        = -3,
  kSCNeedsRecalculation   = -1,                   // Flag indication the
                                                  // necessity to recalculate
                                                  // an objects size / historysize
                                                  // value.
  kSCMaxDepth             = 0,                    // Default Validation Depth
                                                  // 0 = unlimited
  kSCHashTablePower       = 16,                   // Default Size of Hashtable (in 2^x)
  kSCHashFunctionIndex    = 1,
  kSCDefaultTableSize     = 4096,
  kSCHeapSize             = 1024,
  kSCMaxCreatedInstances  = 5,
  kSCEmptyTableEntry      = LONG_MAX
};


typedef void            (*SCBodyFuncPtr) (void);

#if _SC_RANGECHECK
  #include <stdio.h>
  #include <stdlib.h>
#endif

enum SCProgress
{
  kSCMaximumProgress = 0,
  kSCFairProgress
};

enum SCObjectType
{
  SC_NONE = 0,
  SC_DATATYPE,
  SC_LIST,
  SC_MACHINE,
  SC_SIGNAL,
  SC_DELAYED_SIGNAL,
  SC_TYPE,
  SC_STREAM,
  SC_PROCESS,
  SC_PROCEDURE,
  SC_RANDOM,
  SC_REQUEST,
  SC_SCHEDULER,
  SC_ENABLED_TRANSITION,
  SC_SENSOR,
  SC_TABLE,
  SC_TIMER,
  SC_TIMER_CONTROL,
  SC_TIME_EVENT,
  SC_PATH,
  SC_TRACE,
  SC_TRANSITION,
  SC_INDET,
  SC_STATESPACE,
  SC_STATE, // only for PEV name lookup

  TL_FORMULA,
  TL_FORMULA_UNARY,
  TL_FORMULA_BINARY,
  TL_FORMULA_UNTIL,
  TL_FORMULA_WAITFOR,
  TL_FORMULA_ZOPER,
  TL_FORMULA_VOPER,
  TL_FORMULA_IMPLIES,
  TL_FORMULA_AND,
  TL_FORMULA_OR,
  TL_FORMULA_NEXTTIME,
  TL_FORMULA_EVENTUALLY,
  TL_FORMULA_ALWAYS,
  TL_FORMULA_NOT,
  TL_FORMULA_ATOMIC_NOT,
  TL_FORMULA_ATOMIC,
  TL_FORMULA_TRUE,
  TL_FORMULA_FALSE,
  
  SC_BITSET,
  TL_BUECHI,
  TL_FORMULA_SET,
  TL_INDET_VERIFY,
  TL_NODE,
  TL_STATE,
  
  SC_TYPE_MAX
};

// for validation purpose:

struct SCSize
{
  SCNatural size;
  SCNatural historySize;

  SCSize(void)
  {
    size = 0;
    historySize = 0;
  }
};

#define SC_MAX_BITINDEX 63

typedef unsigned long long SCBitSet;

#define SC_BS0 0ULL

typedef SCBoolean (*SCPropFuncPtr) (void);

typedef struct
{
  const char*         name;
  SCPropFuncPtr Evaluate;
} SCName2PropFuncPtr;

#endif  // __SCBASICTYPES__
