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
| SCTraceTypes  |  SCTraceTypes.h   | 26. Sep 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     26.09.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCTRACETYPES__
#define __SCTRACETYPES__


#include "SCBasicTypes.h"


enum SCTraceAction
{
  scTraceUndefined = 0,       // Dummy fuer 0
  scTraceSchedulerInit,       //  1 Init-Befehl fuer Scheduler
  scTraceSchedulerStop,       //  2 Stop-Befehl fuer Scheduler
  scTraceTimeChange,          //  3 Simulationszeit hat sich geaendert
  scTraceProcessCreate,       //  4 Prozess wird erzeugt
  scTraceProcessStop,         //  5 Prozess wird zerstoert
  scTraceMachineCreate,       //  6 Maschine wird erzeugt
  scTraceMachineStop,         //  7 Maschine wird zerstoert
  scTraceStateChange,         //  8 Zustandswechsel
  scTraceSignalSend,          //  9 Output an einen anderen Prozess
  scTraceSignalNotSent,       // 10 Output wg. fehlendem Empfaenger fehlgeschlagen
  scTraceSignalReceive,       // 11 Prozess hat ein Signal empfangen
  scTraceSignalLose,          // 12 Signal ist verlorengegangen (Queue-Ueberlauf)
  scTraceSignalConsume,       // 13 Signal wurde konsumiert
  scTraceSignalSave,          // 14 Signal wurde zwischengespeichert (save)
  scTraceSignalDrop,          // 15 Signal wurde verworfen (implizite Konsumierung)
  scTraceSpontTrans,          // 16 Spontantransition (input none) hat gefeuert
  scTraceContSignal,          // 17 Continuous Signal hat gefeuert
  scTraceServiceRequest,      // 18 Request von Maschine empfangen
  scTraceServiceFinish,       // 19 Request ist beendet, Prozess wird freigegeben
  scTraceServiceStart,        // 20 Start der Bearbeitung eines Requests
  scTraceServiceInterrupt,    // 21 Unterbrechung eines Requests
  scTraceTimerSet,            // 22 Timer wurde gesetzt
  scTraceTimerReset,          // 23 Timer wurde geloescht
  scTraceTimerFire,           // 24 Timer wurde ausgeloest
  scTraceTimerRemove,         // 25 Timer in InputQueue geloescht
  scTraceProcedureCall,       // 26 Prozedur wird aufgerufen
  scTraceProcedureReturn,     // 27 Prozedur wird aufgerufen
  scTraceDeadlock,            // 28 Totaler Deadlock des Systems
  scTraceCycleStart,          // 29 Zyklus im Verhalten des Systems (start)
  scTraceCycleEnd,            // 30 Zyklus im Verhalten des Systems (end)
  scTraceMax                  // 31 Dummy fuer letzten Wert
};


/* #defines fuer actionFlags */
#define SCF(a)     ( 1L << ((a)-1) )
enum SCTraceActionFlag {
  scfTraceSchedulerInit     = SCF(scTraceSchedulerInit),
  scfTraceSchedulerStop     = SCF(scTraceSchedulerStop),
  scfTraceTimeChange        = SCF(scTraceTimeChange),
  scfTraceProcessCreate     = SCF(scTraceProcessCreate),
  scfTraceProcessStop       = SCF(scTraceProcessStop),
  scfTraceMachineCreate     = SCF(scTraceMachineCreate),
  scfTraceMachineStop       = SCF(scTraceMachineStop),
  scfTraceStateChange       = SCF(scTraceStateChange),
  scfTraceSignalSend        = SCF(scTraceSignalSend),
  scfTraceSignalNotSent     = SCF(scTraceSignalNotSent),
  scfTraceSignalReceive     = SCF(scTraceSignalReceive),
  scfTraceSignalLose        = SCF(scTraceSignalLose),
  scfTraceSignalConsume     = SCF(scTraceSignalConsume),
  scfTraceSignalSave        = SCF(scTraceSignalSave),
  scfTraceSignalDrop        = SCF(scTraceSignalDrop),
  scfTraceSpontTrans        = SCF(scTraceSpontTrans),
  scfTraceContSignal        = SCF(scTraceContSignal),
  scfTraceServiceRequest    = SCF(scTraceServiceRequest),
  scfTraceServiceFinish     = SCF(scTraceServiceFinish),
  scfTraceServiceStart      = SCF(scTraceServiceStart),
  scfTraceServiceInterrupt  = SCF(scTraceServiceInterrupt),
  scfTraceTimerSet          = SCF(scTraceTimerSet),
  scfTraceTimerReset        = SCF(scTraceTimerReset),
  scfTraceTimerFire         = SCF(scTraceTimerFire),
  scfTraceTimerRemove       = SCF(scTraceTimerRemove),
  scfTraceProcedureCall     = SCF(scTraceProcedureCall),
  scfTraceProcedureReturn   = SCF(scTraceProcedureReturn),
  scfTraceDeadlock          = SCF(scTraceDeadlock),
  scfTraceCycleStart        = SCF(scTraceCycleStart),
  scfTraceCycleEnd          = SCF(scTraceCycleEnd)
};
#undef SCF

#define scfTraceAll                 0xFFFFFFFFL


#endif  //  __SCTRACETYPES__
