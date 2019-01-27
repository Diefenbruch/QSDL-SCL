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
|  SCArray      |   SCArray.h       | 7. Jun 1994   |   SCL             |       |
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


#ifndef __SCARRAY__

#define __SCARRAY__


#include "SCDataType.h"


template<class T> class SCArray : public SCDataType
{
  public:
                        SCArray (const SCInteger lowBound, // for T with constructor
                                 const SCInteger highBound,
                                 const SCObject* father = NULL);
                        SCArray (const SCInteger lowBound, // for T without constructor
                                 const SCInteger highBound,
                                 const T         initValue,
                                 const SCObject* father = NULL);
                        SCArray (const SCArray<T>& orig,
                                 const SCObject* father = NULL);
                       ~SCArray (void);

    SCBoolean           Equal (const SCDataType &second) const;
    SCDataType *        Clone (void) const;

    SCInteger           GetLowBound(void) const {return lowBound; }
    SCInteger           GetHighBound(void) const {return highBound; }

//    friend SCStream &    operator<< (SCStream & pStream,
//                                    const SCArray<T> & pData);
    virtual SCStream &   Display (SCStream &pStream) const;

    SCBoolean           operator== (const SCArray<T> & second) const;
    SCArray<T> &        operator= (const SCArray<T> & orig);
    T &                 operator[] (const SCInteger index) const;
                                                // Save and Restore must be
                                                // implemented by derived classes
                                                // as it is impossible to
                                                // determine ahead whether
                                                // a simple type or a class
                                                // type is used to instantiate the
                                                // template.
                                                // Same holds for Size and HistorySize.

  protected:
    SCInteger           lowBound,
                        highBound;
    T *                 array;
};


#if !_SC_NOINLINES
  #include "SCArray.inl.h"
#endif

#ifdef _SC_NEEDS_TMPL_H
  #include "SCArray.tmpl.h"
#endif

#endif  // __SCARRAY__
