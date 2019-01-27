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
|  SCPowerset   |   SCPowerset.h    | 7. Jun 1994   |   SCL             |       |
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


#ifndef __SCPOWERSET__

#define __SCPOWERSET__

#include "SCDataType.h"

class SCPowerset : public SCDataType
{
  public:
                          SCPowerset (const SCPowerset& orig,
                                      const SCObject* father = NULL);
                          SCPowerset (const SCInteger low,
                                      const SCInteger high,
                                      const SCObject* father = NULL);
                         ~SCPowerset (void);

    SCBoolean             Equal (const SCDataType &second) const;
    SCDataType *          Clone (void) const;

    SCPowerset&           operator= (const SCPowerset& orig);
    SCBoolean             In (const SCInteger item) const;
    void                  Incl (const SCInteger item);
    void                  Del (const SCInteger item);

    SCBoolean             operator== (const SCPowerset& second) const;
    SCBoolean             operator!= (const SCPowerset& second) const;
    SCBoolean             operator< (const SCPowerset& superSet) const;
    SCBoolean             operator<= (const SCPowerset& superSet) const;
    SCBoolean             operator> (const SCPowerset& subSet) const;
    SCBoolean             operator>= (const SCPowerset& subSet) const;
    SCPowerset            operator& (const SCPowerset& second) const;
    SCPowerset            operator| (const SCPowerset& second) const;
    
    friend SCStream &      operator<< (SCStream &pStream,
                                      const SCPowerset &pData);
    virtual SCStream &     Display (SCStream &pStream) const;

    SCBoolean             Save(SCMem&) const;
    SCBoolean             HistorySave(SCMem&) const;  // von SCDataType fuer History-Variablen
    SCBoolean             Restore(SCMem&);
    SCBoolean             HistoryRestore(SCMem&);     // von SCDataType fuer History-Variablen
    void                  Size(SCSize *pSize) const;
    void                  HistorySize(SCSize *pSize) const; // von SCDataType fuer History-Variablen

  protected:
    SCInteger             lowBound;
    SCInteger             highBound;
    unsigned char *       set;                    // Bitset.
};


#if !_SC_NOINLINES
    #include "SCPowerset.inl.h"
#endif


#endif  // __SCPOWERSET__
