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
|   SCRunnable  |   SCRunnable.h    | 16. Aug 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     16.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCRUNNABLE__

#define __SCRUNNABLE__

#include "SCBasicTypes.h"
#include "SCRunnableTypes.h"
#include "SCObject.h"

enum {
  kSCScheduleNow,
  kSCScheduleTime,
  kSCScheduleWaiting,
  kSCScheduleBlocked
};

class SCRunnable : public SCSystemRunnable
{
  public:
    virtual              ~SCRunnable (void);

    void                  Schedule (SCNatural how, SCTime time = 0.0);

    SCTime                GetWakeupTime (void) const;
    SCBoolean             IsBlocked (void) const;
    SCBoolean             IsWaiting (void) const;

    void                  Stop (void);

    SCRunnableID          GetID (void) const;
    SCBoolean             IsProcess (void) const;
    SCBoolean             IsProcedure (void) const;
    SCBoolean             IsMachine (void) const;
    SCBoolean             IsAutomaton (void) const;

    void                  TraceOn (void);
    void                  TraceOff (void);
    SCBoolean             IsTraceOn (void) const;

    friend SCStream&      operator<< (SCStream& pStream,
                                      const SCRunnable& pData);
    virtual SCStream &    Display (SCStream& pStream) const;

    SCBoolean             Save (SCMem &) const;
    SCBoolean             Restore (SCMem &);
    void                  Size(SCSize *pSize) const;

#if _SC_VALIDATION_OPTIMIZE
    SCBoolean             IsModified (void) const { return modified; }
    void                  SetModified (SCBoolean setQueues = false);
    void                  UnsetModified (SCBoolean setQueues = false);

    void                  SetCurrentSize (SCNatural newSize) const
                            { SCRunnable *mutable_this = (SCRunnable *)this;
                              mutable_this->currentSize = newSize;
                            }
    SCNatural             GetCurrentSize (void) const
                            { return currentSize; }
    void                  SetCurrentHistorySize (SCNatural newSize) const
                            { SCRunnable *mutable_this = (SCRunnable *)this;
                              mutable_this->currentHistorySize = newSize;
                            }
    SCNatural             GetCurrentHistorySize (void) const
                            { return currentHistorySize; }

#if _SC_VALIDATION_OPTIMIZE >= 3
    size_t                GetLastOffset (void) const
                            { return lastOffset; }
    void                  SetLastOffset (SCNatural newOffset)
                            { lastOffset = newOffset; }
    size_t                GetLastHistoryOffset (void) const
                            { return lastHistoryOffset; }
    void                  SetLastHistoryOffset (SCNatural newOffset)
                            { lastHistoryOffset = newOffset; }
#endif

#endif

  protected:
                          SCRunnable (SCRunnableID runnableID,
                                      SCBoolean sleeping = false,
                                      const SCObjectType newType = SC_NONE,
                                      const SCObject *pParent = NULL);
                          SCRunnable (const SCObjectType newType = SC_NONE,
                                      const SCObject *pParent = NULL);

    SCBoolean             IsVerbose (void) const;

  private:
    SCRunnableID          runnableID;
    SCTime                wakeupTime;
    SCBoolean             blocked;          // blocked by request
    SCBoolean             waiting;          // waiting for input

    SCBoolean             traceOn;

#if _SC_VALIDATION_OPTIMIZE
  protected:
    SCBoolean             modified;         // Flag, whether runnable has
                                            // changed in current transition.
                                            // Don't save this. It is done in
                                            // SCRunnableList.cc
  public:
    mutable SCNatural     currentSize;      // Result of last call to Size.
                                            // If modified == false, this value
                                            // is valid. Don't save it, it is
                                            // calculated on the fly!
    mutable SCNatural     currentHistorySize;
                                            // Result of last call to HistorySize.
                                            // If modified == false, this value
                                            // is valid.  Don't save it, it is
                                            // calculated on the fly!
  private:
#if _SC_VALIDATION_OPTIMIZE >= 3
    size_t                lastOffset;       // Offset in last save-area for this
                                            // runnable.
                                            // If modified == false, this value
                                            // is valid.
                                            // Don't save this. It is set in
                                            // SCRunnableList.cc
    size_t                lastHistoryOffset;// Offset in last hist-area for this
                                            // runnable.
                                            // If modified == false, this value
                                            // is valid.
                                            // Don't save this. It is set in
                                            // SCRunnableList.cc
#endif

#endif
};


#if !_SC_NOINLINES
  #include "SCRunnable.inl.h"
#endif


#endif  //  __SCRUNNABLE__
