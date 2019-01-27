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
| SCProcessType |  SCProcessType.h  | 9. Aug 1994   |   SCL             |       |
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


#ifndef __SCPROCESSTYPE__

#define __SCPROCESSTYPE__

#include <string.h>
#include "SCType.h"
#include "SCSignalList.h"
#include "SCStateTypeList.h"
#include "SCProcessTypeList.h"
#include "SCProcessList.h"
#include "SCProcessTypeTable.h"

class SCProcessType : public SCType
{
  public:
                               SCProcessType (SCNatural pID,
                                              const char *typeName,
                                              SCInteger pMaxProcesses = kSCUnlimitedProcesses,
                                              SCNatural pMaxInputQLength = kSCUnlimitedQueueLength,
                                              const SCObject* father = NULL);
    virtual                   ~SCProcessType (void);

    virtual class SCProcess *  NewInstance (const SCProcessID creatorID = kSCNoProcessID,
                                            class SCDataType *actualParams = NULL) const = 0;
    virtual class SCProcess *  NewInstance (SCMem &saveArea) const = 0;
                                          // Creates new instance of
                                          // this process type and 
                                          // restores data from
                                          // saveArea. Implemented
                                          // in generated code.
    SCProcess *                Allocate (class SCProcess *creator = NULL,
                                         class SCDataType *actualParams = NULL);
    SCBoolean                  Deallocate (class SCProcess *process);

    SCProcess *                GetAProcess (void) const;
    SCProcessID                GetAProcessID (void) const;
    SCNatural                  GetMaxInputQueueLength (void) const;
    SCInteger                  GetMaxInstances (void) const { return maxInstances; }
    SCInteger                  NumOfInstances (void) const;
    SCProcessList *            GetProcessList (void) const { return instanceList; }
    SCStateTypeList *          GetStateList (void) const { return stateList; }
    void                       SetStateList (SCStateTypeList *newStateList);

    SCBoolean                  Save (SCMem &) const; // saves all instances
    SCBoolean                  Restore (SCMem &);    // restores all instances
    void                       Size (SCSize *pSize) const;

    static SCBoolean           StaticSave (SCMem &);
    static SCBoolean           StaticRestore (SCMem &);
    static void                StaticSize (SCSize *pSize);

    void                       Stop(void);
    static void                StaticStop (void);

#if _SC_VALIDATION_OPTIMIZE
    void                       SetModified (SCBoolean setQueues = false);
    void                       UnsetModified (SCBoolean setQueues = false);

    static void                StaticSetModified (SCBoolean setQueues = false);
    static void                StaticUnsetModified (SCBoolean setQueues = false);
#endif

    /* static Methods */
    static SCBoolean           Initialize (SCNatural tableSize);
    static SCBoolean           Finish (void);
    static SCProcessTypeList * GetProcessTypeList (void) { return processTypeList; }
    static void                SetProcessTypeList (SCProcessTypeList *newProcessTypeList) {processTypeList = newProcessTypeList; }
    static SCProcessTypeTable *GetProcessTypeTable(void) { return processTypeTable; } // table of all existing process!
    static void                SetProcessTypeTable(SCProcessTypeTable *newTable) { processTypeTable = newTable; }
    static SCProcessType *     GetProcessTypeFromName (const char * name);

    SCBoolean                  operator== (const SCProcessType & second) const;
    SCBoolean                  operator!= (const SCProcessType & second) const;

  private:
    SCInteger                  maxInstances;
    SCNatural                  maxInputQueueLen;
    SCProcessList *            instanceList;
    SCStateTypeList *          stateList;        // list of all states

    static SCProcessTypeList * processTypeList;  // list of all process types
    static SCProcessTypeTable *processTypeTable; // hash table of all existing
                                                 // process types
};

#if !_SC_NOINLINES
  #include "SCProcessType.inl.h"
#endif


#endif  //  __SCPROCESSTYPE__
