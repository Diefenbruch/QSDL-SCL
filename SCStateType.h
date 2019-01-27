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
| SCSignalType  |  SCSignalType.h   | 2. Apr 1995   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     02.04.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#ifndef __SCSTATETYPE__

#define __SCSTATETYPE__

#include "SCType.h"
#include "SCStateTypeTable.h"
#include "SCTransitionList.h"
#include "SCTransitionListTable.h"
#include "SCSignalTypeTable.h"

class SCStateType : public SCType
{
  public:
                               SCStateType (SCNatural typeKey,
                                            const char *typeName = NULL,
                                            const SCNatural pSaveSetSize = 0,
                                            const SCSignalID *const pSaveSet = NULL,
                                            const SCBoolean pSaveAll = false,
                                            const SCBoolean pIntermediate = false,
                                            const SCObject* father = NULL);
                              ~SCStateType(void);

    void                       AddTransition (class SCTransition *pTransition);
    SCTransitionList *         GetNormalInputs (void) const;
    SCTransitionList *         GetPriorityInputs (void) const;
    SCTransitionList *         GetInputNones (void) const;
    SCTransitionList *         GetContSignals (void) const;
    SCTransitionListTable *    GetNormalInputsTable (void) const;
    SCTransitionListTable *    GetPriorityInputsTable (void) const;
    SCSignalTypeTable *        GetSaveSetTable (void) const;
    SCBoolean                  HasSpontaneous (void) const;
    SCTransitionID             GetMaxTransitionID (void) const;
    SCNatural                  GetSaveSetSize (void) const;
    const SCSignalID *         GetSaveSet (void) const;
    SCBoolean                  GetSaveAll (void) const;
    SCBoolean                  IsReached (void) const;
    void                       SetReached (void);
    SCBoolean                  IsIntermediate (void) const;

    friend SCStream&           operator<< (SCStream &pStream,
                                           const SCStateType &pData);

    SCBoolean                  operator== (const SCStateType & second) const;
    SCBoolean                  operator!= (const SCStateType & second) const;

    static SCBoolean           Initialize (SCNatural tableSize);
    static SCBoolean           Finish (void);
    static SCStateTypeTable *  GetStateTypeTable(void); // table of all existing states!
    static void                SetStateTypeTable(SCStateTypeTable *newTable);

  private:
    SCTransitionList *         normalInputs;
    SCTransitionList *         priorityInputs;
    SCTransitionListTable *    normalInputsTable;   // lookup table for signals
    SCTransitionListTable *    priorityInputsTable;
    SCTransitionList *         inputNones;
    SCTransitionList *         contSignals;
    SCNatural                  saveSetSize;
    const SCSignalID *         saveSet;
    SCBoolean                  saveAll;
    SCSignalTypeTable *        saveSetTable;
    SCTransitionID             maxTransitionID;
    SCBoolean                  reached;            // flag indicates if state
                                                   // is reached during analysis
    SCBoolean                  intermediateState;  // flag indicates if this
                                                   // state was produced by the
                                                   // linearization of decisions
    static SCStateTypeTable   *stateTypeTable;     // hash table of all existing
                                                   // state types
};

#if !_SC_NOINLINES
  #include "SCStateType.inl.h"
#endif


#endif  // __SCSTATETYPE__
