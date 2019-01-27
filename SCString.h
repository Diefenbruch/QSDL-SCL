/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1997 Marc Diefenbruch, Wolfgang Textor                                  |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|  SCString     |   SCString.h      | 22. Okt 1997  |   SCL             |       |
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


#ifndef __SCSTRING__

#define __SCSTRING__


#include "SCDataType.h"
#include "SCList.h"

template<class T> class SCString : public SCDataType, public SCList<T>
{
  public:
                        SCString (const SCObject* father = NULL);
                        SCString (const T &,
                                  const SCObject* father = NULL);
                        SCString (const SCString<T>&,
                                  const SCObject* father = NULL);
    virtual            ~SCString (void);

    static SCString<T> & MkString(const T &);

    SCBoolean           Equal (const SCDataType &) const;
    SCDataType *        Clone (void) const;

    T                   First (void) const;
    T                   Last (void) const;
    SCInteger           Length (void) const;
    SCString<T>         SubString (const SCNatural start,
                                   const SCNatural length) const;
    // doesn't work with explicit template instantiation!
//    friend class SCStream &    operator<< (class SCStream & pStream,
//                                    const SCString<T> & pData);
    virtual class SCStream &   Display (class SCStream &pStream) const;

    SCBoolean           operator== (const SCString<T> & second) const;
    SCString<T> &       operator= (const SCString<T> & orig);
    T &                 operator[] (const SCNatural index) const;
    SCString<T>         operator|| (const SCString<T>& second);
    SCString<T> &       Modify (const SCNatural index, const T & item);

    SCBoolean           Save (SCMem &saveArea) const;
    SCBoolean           Restore (SCMem &saveArea);
    void                Size(SCSize *pSize) const;

    SCBoolean           HistorySave (SCMem &saveArea) const { return Save(saveArea); }
    SCBoolean           HistoryRestore (SCMem &saveArea) { return Restore(saveArea); }
    void                HistorySize (SCSize *pSize) const { Size(pSize); }
};


#if !_SC_NOINLINES
  #include "SCString.inl.h"
#endif

#ifdef _SC_NEEDS_TMPL_H
  #include "SCString.tmpl.h"
#endif

#endif  // __SCSTRING__
