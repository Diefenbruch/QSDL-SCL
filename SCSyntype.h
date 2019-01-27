/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1995 Joerg Ruehl                                                        |
|       University of Essen, Germany                                            |
|                                                                               |
|   SCL-Version (c) 1995 Marc Diefenbruch, Wolfgang Textor                      |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|   SCSyntype   |   SCSyntype.h     | 28. Jul 1995  |   SCL             |       |
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

#ifndef __SCSYNTYPE__

#define __SCSYNTYPE__

#include "SCDataType.h"


template<class T> class SCSyntype : public SCDataType
{
  public:
                          SCSyntype (const T pMinimum,
                                     const T pMaximum,
                                     const SCObject* father = NULL);
                          SCSyntype (const T pMinimum,
                                     const T pMaximum,
                                     const T pValue,
                                     const SCObject* father = NULL);
                          SCSyntype (const T pMinimum,
                                     const T pMaximum,
                                     const SCBoolean pNoMin,
                                     const SCBoolean pNoMax,
                                     const T pValue,
                                     const SCObject* father = NULL);
                          SCSyntype (const T pValue);
    virtual              ~SCSyntype (void);

    SCBoolean             Equal (const SCDataType& second) const;
    SCDataType *          Clone (void) const;

// doesn't work with explicit template instantiation!
//    friend SCStream &      operator<< (SCStream &pStream,
//                                      const SCSyntype<T> &pData);
    virtual SCStream &     Display (SCStream &pStream) const;

    T                     GetValue (void) const;
    void                  SetValue (const T pValue);
    void                  SetMin (const T pMinimum);
    void                  SetNoMin (void);
    void                  SetMax (const T pMaximum);
    void                  SetNoMax (void);

    SCBoolean             operator==  (const SCSyntype<T>& second) const;
    SCBoolean             operator!=  (const SCSyntype<T>& second) const;
    SCBoolean             operator>   (const SCSyntype<T>& second) const;
    SCBoolean             operator<   (const SCSyntype<T>& second) const;
    SCBoolean             operator>=  (const SCSyntype<T>& second) const;
    SCBoolean             operator<=  (const SCSyntype<T>& second) const;

    SCBoolean             operator==  (const T second) const;
    SCBoolean             operator!=  (const T second) const;
    SCBoolean             operator>   (const T second) const;
    SCBoolean             operator<   (const T second) const;
    SCBoolean             operator>=  (const T second) const;
    SCBoolean             operator<=  (const T second) const;

    friend SCBoolean      operator==  (const T first, const SCSyntype<T>& second)
    {
      return (first == second.value);
    }
    friend SCBoolean      operator!=  (const T first, const SCSyntype<T>& second)
    {
      return (first != second.value);
    }
    friend SCBoolean      operator>   (const T first, const SCSyntype<T>& second)
    {
      return (first > second.value);
    }
    friend SCBoolean      operator<   (const T first, const SCSyntype<T>& second)
    {
      return (first < second.value);
    }
    friend SCBoolean      operator>=  (const T first, const SCSyntype<T>& second)
    {
      return (first >= second.value);
    }
    friend SCBoolean      operator<=  (const T first, const SCSyntype<T>& second)
    {
      return (first <= second.value);
    }
    SCSyntype<T>&         operator=   (const T orig);
    SCSyntype<T>&         operator=   (const SCSyntype<T>& orig);
                          operator T  (void) const;

    SCSyntype<T>          operator+ (const SCSyntype<T>& second) const;
    SCSyntype<T>          operator- (const SCSyntype<T>& second) const;
    SCSyntype<T>          operator* (const SCSyntype<T>& second) const;
    SCSyntype<T>          operator/ (const SCSyntype<T>& second) const;
    SCSyntype<T>          operator% (const SCSyntype<T>& second) const;
    SCSyntype<T>          operator+ (const T second) const;
    SCSyntype<T>          operator- (const T second) const;
    SCSyntype<T>          operator* (const T second) const;
    SCSyntype<T>          operator/ (const T second) const;
    SCSyntype<T>          operator% (const T second) const;

    friend SCSyntype<T>   operator+ (const T first, const SCSyntype<T>& second)
    {
      SCSyntype<T>    s (second);
      s.SetValue (first + second.value);
      return (s);
    }
    friend SCSyntype<T>   operator- (const T first, const SCSyntype<T>& second)
    {
      SCSyntype<T>    s (second);
      s.SetValue (first - second.value);
      return (s);
    }
    friend SCSyntype<T>   operator* (const T first, const SCSyntype<T>& second)
    {
      SCSyntype<T>    s (second);
      s.SetValue (first * second.value);
      return (s);
    }
    friend SCSyntype<T>   operator/ (const T first, const SCSyntype<T>& second)
    {
      SCSyntype<T>    s (second);
      s.SetValue (first / second.value);
      return (s);
    }
    friend SCSyntype<T>   operator% (const T first, const SCSyntype<T>& second)
    {
      SCSyntype<T>    s (second);
      s.SetValue (first % second.value);
      return (s);
    }

  protected:
    void                  copy (const SCSyntype<T>&);
    T                     value;
    T                     minimum;
    T                     maximum;
    SCBoolean             noMin;
    SCBoolean             noMax;
};


template<class T> inline SCBoolean SCSyntype<T>::Equal (const SCDataType &second) const
{
  return (value == ((SCSyntype<T> *)&second)->value);
}

template<class T> inline SCDataType *SCSyntype<T>::Clone (void) const
{
  return (new SCSyntype<T> (*this));
}


#if !_SC_NOINLINES
    #include "SCSyntype.inl.h"
#endif

#ifndef __GNUC__
#include "SCSyntype.tmpl.h"  // Sun CC Compiler needs this
#endif

#endif  // __SCSYNTYPE__
