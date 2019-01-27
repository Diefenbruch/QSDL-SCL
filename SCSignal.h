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
|   SCSignal    |   SCSignal.h      | 24. Feb 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#ifndef __SCSIGNAL__

#define __SCSIGNAL__

#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCSignalList.h"
#include "SCSignalTable.h"

class SCSignal: public SCObject
{
  public:
                               SCSignal (const SCProcessID pSenderID,
                                         const class SCProcessType *const pSenderType,
                                         const class SCSignalType *const pSignalType,
                                         class SCDataType *pData = NULL,
                                         const SCNatural pTimerID = 0,
                                         const SCObject* pParent = NULL);

                               SCSignal (SCMem & saveArea, // Constructor for Restore.
                                         const SCObject *pParent = NULL);
    virtual                   ~SCSignal (void);

    const class SCSignalType * GetSignalType (void) const;
    SCNatural                  GetSignalID (void) const;
    SCNatural                  GetTimerID (void) const;

    class SCProcess *          GetSender (void) const;
    const class SCProcessType *GetSenderType (void) const;
    SCProcessID                GetSenderID (void) const;

    SCBoolean                  SameData (const class SCDataType *const pData) const;
    class SCDataType *         GetData (void) const;
    class SCDataType *         RetrieveData (void);

    SCTime                     GetCreationTime (void) const;

    SCSignalCons *             GetContainer (void) const;
    void                       SetContainer (SCSignalCons *cont);

    static SCBoolean           Initialize (SCNatural pTableSize);
    static SCBoolean           Finish (void);
    static SCSignalTable *     GetSignalTable(void);
    static void                SetSignalTable(SCSignalTable *newTable);

    friend SCStream &          operator<< (SCStream& pStream,
                                           const SCSignal& pData);
    virtual SCStream &         Display (SCStream &pStream) const;

    SCBoolean                  operator== (const SCSignal & second) const;
    SCBoolean                  operator!= (const SCSignal & second) const;

    SCBoolean                  Save (SCMem &) const;
    SCBoolean                  Restore (SCMem &);
    void                       Size (SCSize * pSize) const;
    static SCBoolean           StaticSave (SCMem &);
    static SCBoolean           StaticRestore (SCMem &);
    static void                StaticSize (SCSize * pSize);

  private:
    SCNatural                  signalID;     // ID of signal
    SCNatural                  timerID;      // timer ID (for timer signals)
    class SCSignalType *       signalType;   // type of signal
    SCProcessID                senderID;     // ID of sender process
    class SCProcessType *      senderType;   // important for MSC tracing:
                                             // if sender is stopped we can
                                             // get the name of the sender
    class SCDataType *         data;         // sended signal parameters

    SCTime                     creationTime; // time of sending

    SCSignalCons *             container;    // actual SCListCons elem
                                             // in which signal is
                                             // embedded (do not save!)
    static SCSignalTable *     signalTable;  // hash table of all
                                             // living signals
    static SCNatural           nextSignalID;
};


#if !_SC_NOINLINES
  #include "SCSignal.inl.h"
#endif


#endif  //  __SCSIGNAL__
