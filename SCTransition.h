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
| SCTransition  | SCTransition.h    | 05. Dez 1995  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     95.12.05    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCTRANSITION__

#define __SCTRANSITION__

#include <limits.h>

#include "SCObject.h"
//#include "SCTransitionList.h"
//#include "SCTransitionListTable.h"

/* Auswahl der verschiedenen Transitions-Typen:
 * 1. signalkonsumierend: pInputSetSize >= 0
 *                        pInputSet enthaelt Vektor mit pInputSetSize Stimuli
 *                        pPriority == kSCPrioNormalInput : normales input
 *                                     kSCPrioPriorityInput : priority input
 * 2. spontan (none)    : pInputSetSize == -1
 *                        pInputSet == NULL
 *                        pPriority == kSCPrioInputNone : Pseudo-Prioritaet fuer input none
 * 3. continuous signal : pInputSetSize == -1
 *                        pInputSet == NULL
 *                        pPriority == kSCPrioNoPriority : Keine Prioritaet angegeben
 *                                  oder: Prioritaet des continuous signal
 * Eine moegliche enabling condition fuer ein signalkonsumierendes input
 * und der Bedingungsterm fuer das continuous signal werden mit Hilfe einer
 * Methode von SCProcess realisiert.
 */

enum
{
  kSCPrioNormalInput = 0,
  kSCPrioPriorityInput = 1,
  kSCPrioInputNone = LONG_MIN,
  kSCPrioNoPriority = LONG_MAX
};

class SCTransition : public SCObject
{
  public:
                          SCTransition (SCNatural id,
                                        class SCStateType *pStateType,
                                        SCInteger pInputSetSize = 0,
                                        SCSignalID *pInputSet = NULL,
                                        SCInteger pPriority = kSCPrioNormalInput,
                                        const SCObject* father = NULL);

    SCInteger             GetPriority (void) const;
    SCInteger             GetInputSetSize (void) const;
    SCSignalID *          GetInputSet (void) const;
    SCNatural             GetID (void) const { return id; }
    SCBoolean             IsExecuted (void) const { return executed; }
    void                  SetExecuted (void) { executed = true; }

    friend SCStream&       operator<< (SCStream &pStream,
                                      const SCTransition &pData);
    virtual SCStream&      Display (SCStream& pStream) const;

    SCBoolean             operator== (const SCTransition & second) const;
    SCBoolean             operator!= (const SCTransition & second) const;

  private:
    SCNatural             id;
    SCInteger             priority;
    SCInteger             inputSetSize;
    SCSignalID *          inputSet;
    SCBoolean             executed;      // flag indicates if
                                         // transition was executed
                                         // during analysis (used for
                                         // detection of dead code)
};


#if !_SC_NOINLINES
  #include "SCTransition.inl.h"
#endif


#endif  // __SCTRANSITION__
