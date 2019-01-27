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
|    SCTimer    |   SCTimer.h       | 24. Feb 1996  |   SCL             |       |
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


#ifndef __SCTIMER__

#define __SCTIMER__

#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCTimerList.h"
#include "SCTimeEvent.h"  // needed for template instantiation!
#include "SCRunnable.h"
#include "SCTimeEventList.h"
#include "SCTimerTable.h"

class SCTimer: public SCObject
{
  public:
                            SCTimer (const class SCProcess *pProcess,
                                     const SCTime &pWhen,
                                     const class SCTimerType *const pTimerType,
                                     class SCDataType *pData = NULL,
                                     const SCObject *pParent = NULL);
                            SCTimer(SCMem & saveArea,     // for Restore
                                    const SCObject *pParent);
    virtual                ~SCTimer (void);

    const class SCTimerType * GetTimerType (void) const;
    const SCNatural         GetTimerID (void) const;
    const SCTime            GetTimeout (void) const;
    SCProcessID             GetProcessID (void) const;

    class SCDataType *      GetData (void) const;
    class SCDataType *      RetrieveData (void);

    SCTimerCons *           GetContainer (void) const;
    void                    SetContainer (SCTimerCons *cont);

    SCTimeEventCons *       GetTimeEventContainer (void) const;
    void                    SetTimeEventContainer (SCTimeEventCons *cont);

    friend SCStream&        operator<< (SCStream& pStream,
                                       const SCTimer& pData);
    virtual SCStream &      Display (SCStream &pStream) const;

    SCBoolean               operator== (const SCTimer & second) const;
    SCBoolean               operator!= (const SCTimer & second) const;

    SCBoolean               Save (SCMem&) const;
    SCBoolean               Restore (SCMem&);
    void                    Size (SCSize *pSize) const;

    static SCBoolean        StaticSave (SCMem &);
    static SCBoolean        StaticRestore (SCMem &);
    static void             StaticSize (SCSize *pSize);

    static SCBoolean        Initialize (SCNatural pTableSize);
    static SCBoolean        Finish (void);
    static SCTimerTable *   GetTimerTable(void);
    static void             SetTimerTable(SCTimerTable *newTable);

  private:
    void                    AddTimer (void);
    void                    RemoveTimer (void);

    SCTimerCons *           GetTableContainer (void) const;
    void                    SetTableContainer (SCTimerCons *cont);

    SCNatural               timerID;        // ID of timer
    class SCTimerType *     timerType;      // type of timer
    SCProcessID             processID;      // ID of setting process
                                            // (used as hash key for
                                            // fast lookup of timer)
    SCTime                  when;           // firing time
    class SCDataType *      data;           // timer parameter
    SCTimerCons *           container;      // actual SCListCons elem
                                            // in which signal is
                                            // embedded (do not save!)

    SCTimerCons *           tableContainer; // We have a hash table for
                                            // all instances of SCTimer
                                            // for every timer type
                                            // (see SCTimerType.h) .
                                            // The entries of the hash table
                                            // are lists of SCTimers.
                                            // So we need a second container
                                            // pointer here for efficient
                                            // removing elements from those
                                            // lists
    SCTimeEventCons *       timeEventContainer;
    
    static SCTimerTable *   timerTable;     // hash table of all
                                            // living timers
    static SCNatural        nextTimerID;
};


#if !_SC_NOINLINES
  #include "SCTimer.inl.h"
#endif


#endif  //  __SCTIMER__
