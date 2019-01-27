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
|   SCRequest   |   SCRequest.h     | 02. Mar 1996  |   SCL             |       |
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


#ifndef __SCREQUEST__

#define __SCREQUEST__


#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCRequestList.h"


class SCRequest: public SCObject
{
  public:
                            SCRequest (const class SCRequestType *pServiceType,
                                       SCDuration pTimeRemaining,
                                       SCNatural pPriority,
                                       class SCAutomaton *pCaller,
                                       SCDuration pRealExecTime,
                                       const SCObject *parent = NULL);
                            SCRequest (SCMem &saveArea,   // fuer Restore
                                       const SCObject *parent = NULL);

    SCNatural                   GetPriority (void) const;
    class SCAutomaton *         GetCaller (void) const;
    const class SCRequestType * GetRequestType (void) const;
    SCNatural                   GetRequestID (void) const;

    SCTime                      GetCreationTime (void) const;
    SCTime                      GetWaitStartTime (void) const;
    void                        SetWaitStartTime (SCTime newValue);
    SCTime                      GetServiceStartTime (void) const;
    void                        SetServiceStartTime (SCTime newValue);

    SCDuration                  GetRemainingTime (void) const;
    SCDuration                  GetRemainingService (void) const;

    SCRequestCons *             GetContainer (void) const;
    void                        SetContainer (SCRequestCons *cont);

    friend class SCStream &      operator<< (SCStream& pStream,
                                            const SCRequest& pData);
    virtual class SCStream &     Display (class SCStream &pStream) const;
 
    void                        Size (SCSize *pSize) const;
    SCBoolean                   Save (SCMem &saveArea) const;
    SCBoolean                   Restore (SCMem &saveArea);
    static SCBoolean            StaticSave (SCMem &);
    static SCBoolean            StaticRestore (SCMem &);
    static void                 StaticSize (SCSize *pSize);

  private:
    SCNatural                   requestID;
    SCRequestType *             serviceType;
    SCNatural                   priority;
    class SCAutomaton *         caller;
    SCProcessID                 callerID;     // nur temporaer; falls
                                              // Maschine VOR Caller-Prozess
                                              // restauriert wird
    SCTime                  creationTime;     // Zeitpunkt der Erstellung
    SCTime                  waitStartTime;    // Zeitpunkt der letzten
                                              // Einfuegung in den Warteraum
    SCTime                  serviceStartTime; // Zeitpunkt der letzten
                                              // Einfuegung in den Warteraum
    SCDuration              remainingTime;    // Verbliebene Ausfuehrungszeit
                                              // beim letzten Break
    SCDuration              remainingService; // Verbliebener Bedienwunsch
                                              // beim letzten Break
    SCRequestCons *         container;        // actual SCListCons elem
                                              // in which request is
                                              // embedded (do not save!)

    static SCNatural        nextRequestID;

    friend class            SCMachineLtd;
    friend class            SCMachineInf;
};

#if !_SC_NOINLINES
  #include "SCRequest.inl.h"
#endif

#endif  // __SCREQUEST__
