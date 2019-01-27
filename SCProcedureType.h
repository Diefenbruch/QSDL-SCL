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
| SCProcedureType |  SCProcedureType.h  | 9. Aug 1994   |   SCL             |       |
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


#ifndef __SCPROCEDURETYPE__

#define __SCPROCEDURETYPE__

#include "SCType.h"
#include "SCStateTypeList.h"
#include "SCProcedureTypeList.h"
#include "SCProcedure.h"
#include "SCProcedureList.h"
#include "SCProcedureTypeTable.h"

class SCProcedureType : public SCType
{
  public:
                                 SCProcedureType (SCNatural pID,
                                                  char *typeName,
                                                  const SCObject* father = NULL);
    virtual                     ~SCProcedureType (void);

    virtual class SCProcedure *  NewInstance (SCMem &saveArea) const = 0;
                                          // Creates new instance of
                                          // this procedure type and 
                                          // restores data from
                                          // saveArea. Implemented
                                          // in generated code.
    SCBoolean                    Allocate (class SCProcedure *procedure);
    SCBoolean                    Deallocate (class SCProcedure *procedure);

    SCProcedureList *            GetProcedureList (void) const { return instanceList; }
    SCStateTypeList *            GetStateList (void) const { return stateList; }
    void                         SetStateList (SCStateTypeList *newStateList);

    SCBoolean                    Save (SCMem &) const; // saves all instances
    SCBoolean                    Restore (SCMem &);    // restores all instances
    void                         Size (SCSize *pSize) const;

    static SCBoolean             StaticSave (SCMem &);
    static SCBoolean             StaticRestore (SCMem &);
    static void                  StaticSize (SCSize *pSize);

    void                         Stop(void);
    static void                  StaticStop (void);
 
#if _SC_VALIDATION_OPTIMIZE
    void                         SetModified (void);
    void                         UnsetModified (void);

    static void                  StaticSetModified (void);
    static void                  StaticUnsetModified (void);
#endif
    
       /* static Methods */
    static SCBoolean             Initialize (SCNatural tableSize);
    static SCBoolean             Finish (void);
    static SCProcedureTypeList * GetProcedureTypeList (void) { return procedureTypeList; }
    static void                  SetProcedureTypeList (SCProcedureTypeList *newProcedureTypeList) {procedureTypeList = newProcedureTypeList; }
    static SCProcedureTypeTable *GetProcedureTypeTable(void) { return procedureTypeTable; } // table of all existing process!
    static void                  SetProcedureTypeTable(SCProcedureTypeTable *newTable) { procedureTypeTable = newTable; }

    SCBoolean                    operator== (const SCProcedureType & second) const;
    SCBoolean                    operator!= (const SCProcedureType & second) const;

  private:
    SCProcedureList *            instanceList;
    SCStateTypeList *            stateList;     // list of all states

    static SCProcedureTypeList * procedureTypeList;  // list of all procedure types
    static SCProcedureTypeTable *procedureTypeTable; // hash table of all existing
                                                     // procedure types
};

#if !_SC_NOINLINES
  #include "SCProcedureType.inl.h"
#endif


#endif  //  __SCPROCEDURETYPE__
