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
|   SCRandom    |   SCRandom.h      | 23. Mai 1994  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     23.05.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCRANDOM__

#define __SCRANDOM__

#if _SC_USEDRAND48
  #include <stdlib.h>
#endif

#include "SCBasicTypes.h"
#include "SCDataType.h"

enum
{
  kSeriesSize = 128
};

typedef SCInteger   SCRandID;


class SCRandom: public SCDataType
{
  public:
                         SCRandom (const SCObject* pParent = NULL);

    SCDataType *         Clone (void) const;
    SCBoolean            Equal (const SCDataType &second) const;

    virtual void         Draw (SCReal &var) = 0;
    virtual void         Draw (SCInteger &var) = 0;

    friend SCStream &    operator<< (SCStream &pStream,
                                     const SCRandom &pData);
    virtual SCStream &   Display (SCStream &pStream) const;
                                                  
    SCBoolean            operator== (const SCRandom & second) const;
    SCBoolean            operator!= (const SCRandom & second) const;

    SCBoolean            Save (SCMem &storage) const;
    SCBoolean            HistorySave (SCMem &storage) const; // for hist vars!
    SCBoolean            Restore (SCMem &storage);
    SCBoolean            HistoryRestore (SCMem &storage);    // for hist vars!
    void                 Size(SCSize *pSize) const;

    static SCBoolean     StaticSave(SCMem &);
    static SCBoolean     StaticRestore(SCMem &);
    static void          StaticSize(SCSize *pSize);
    
  protected:
    SCReal               Uniform (void);

    SCRandID             id;

  private:
    SCNatural            ID2SeedIndex (SCInteger id) const;

    static SCRandID      nextID;

#if _SC_USEDRAND48
    unsigned short       curSeed48[3];
    static unsigned short seed48Init[8192][3];
#else
    SCReal               MGen (void);

    SCReal               series[kSeriesSize];
    SCInteger            mSeed,
                         lSeed;
    static SCInteger     mSeedInit[];
    static SCInteger     lSeedInit[];
#endif
};

#endif  // __SCRANDOM__
