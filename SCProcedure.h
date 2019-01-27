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
|  SCProcedure  |  SCProcedure.h    | 03. Feb 1997  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     --.--.--    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCPROCEDURE__

#define __SCPROCEDURE__

#include "SCAutomaton.h"
#include "SCProcedureList.h"

class SCProcedure : public SCAutomaton
{
  public:
      /* Destruktor */
    virtual                  ~SCProcedure (void);

    virtual class SCProcess * GetOwner (void) const { return owner; }
    virtual void              Stop (SCBoolean traceIt = true);

    class SCProcedureType *   GetType (void) const;

    void                      Return (class SCDataType *retValue = NULL);
    
    friend SCStream &          operator<< (SCStream& pStream,
                                          const SCProcedure& pData);
    virtual SCStream &         Display (SCStream& pStream) const;

    SCBoolean                 Save (SCMem &) const;
    SCBoolean                 Restore (SCMem &);
   
    void                      Size (SCSize *pSize) const;

    SCProcedureCons *         GetContainer (void) const { return container; }
    void                      SetContainer (SCProcedureCons *cont) { container = cont; }

  protected:
                              SCProcedure (SCAutomaton * pCaller,
                                           class SCProcess *pOwner,
                                           class SCProcedureType *pProcType,
                                           class SCStateType *startState,
                                           class SCDataType *procedureParameters = NULL,
                                           const SCBoolean varSize = false,
                                           const SCObject* pParent = NULL);
                              SCProcedure (const SCBoolean varSize = false,
                                           const SCObject *pParent = NULL);

      /* Zugriff auf Member des Owners */
    SCProcessID               Self (void) const;
    SCProcessID               Sender (void) const;
    SCProcessID               Parent (void) const;
    SCProcessID               Offspring (void) const;

  private:
    SCAutomaton *             caller;
    class SCProcess *         owner;
    class SCProcedureType *   procType;

    SCProcedureCons *   container;                    // actual SCListCons elem
                                                      // in which procedure is
                                                      // embedded (do not save!)
};

#if !_SC_NOINLINES
  #include "SCProcedure.inl.h"
#endif

#endif  // __SCPROCEDURE__
