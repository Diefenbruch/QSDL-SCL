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
|  SCCharstring | SCCharstring.h    | 23. Mai 1994  |   SCL             |       |
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


#ifndef __SCCHARSTRING__

#define __SCCHARSTRING__


#include "SCDataType.h"


class SCCharstring : public SCDataType
{
  public:
                     SCCharstring (const SCNatural maxLength = 0,
                                   const SCObject* father = NULL);
                     SCCharstring (const SCCharacter stringChar,
                                   const SCObject* father = NULL);
                     SCCharstring (const SCCharacter *const string,
                                   const SCObject* father = NULL);
                     SCCharstring (const SCCharstring &orig,
                                   const SCObject* father = NULL);
    virtual         ~SCCharstring (void);

    static SCCharstring & MkString(const SCCharacter stringChar);
    
    SCBoolean        Equal (const SCDataType &second) const;
    SCDataType *     Clone (void) const;

    SCCharacter      First (void) const;
    SCCharacter      Last (void) const;
    SCInteger        Length (void) const;
    SCCharstring     SubString (const SCNatural start, const SCNatural length) const;
    SCCharstring&    operator= (const SCCharstring& orig);
    SCCharstring     operator|| (const SCCharstring& second);
    SCCharacter      operator[] (const SCNatural index) const;
    SCBoolean        operator== (const SCCharstring &second) const;

    SCCharacter *    GetString (void) const;
    
    friend SCStream & operator<< (SCStream &pStream,
                                 const SCCharstring &pData);
    SCStream &        Display (SCStream &pStream) const;

    SCBoolean        Save(SCMem&) const;
    SCBoolean        HistorySave(SCMem&) const; // for hist vars!
    SCBoolean        Restore(SCMem&);
    SCBoolean        HistoryRestore(SCMem&);    // for hist vars!
    void             Size(SCSize *pSize) const;
    void             HistorySize(SCSize *pSize) const;
    
  private:
    SCNatural        maxLength;
    SCNatural        curLength;
    SCCharacter *    s;
};


typedef SCCharstring  charstring;


inline SCBoolean SCCharstring::Equal (const SCDataType &second) const
{
  return (*this == *(SCCharstring *)&second);
}


inline SCDataType *SCCharstring::Clone (void) const
{
  return (new SCCharstring (*this));
}


#if !_SC_NOINLINES
  #include "SCCharstring.inl.h"
#endif


#endif  // __SCCHARSTRING__
