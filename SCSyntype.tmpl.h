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
|   SCSyntype   |   SCSyntype.tmpl  | 28. Jul 1995  |   SCL             |       |
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


#ifndef __SCSYNTYPE_TMPL__

#define __SCSYNTYPE_TMPL__


#include "SCSyntype.h"
#include "SCStream.h"

#if _SC_NOINLINES
    #include "SCSyntype.inl.h"
#endif


template<class T> SCSyntype<T>::SCSyntype (const T pMinimum,
                                           const T pMaximum,
                                           const SCObject* father):
SCDataType(father)
{
  SetMin (pMinimum);
  SetMax (pMaximum);
}

template<class T> SCSyntype<T>::SCSyntype (const T pMinimum,
                                           const T pMaximum,
                                           const T pValue,
                                           const SCObject* father):
SCDataType(father)
{
  SetMin (pMinimum);
  SetMax (pMaximum);
  value = pValue;
#if _SC_RANGECHECK
  if (!noMin && value < minimum)
  {
    fprintf (stderr, "template<T> SCSyntype::SCSyntype(min,max,value): value[%ld] < minimum[%ld]\n",
             value, minimum);
    exit (1);
  }
  if (!noMax && value > maximum) {
    fprintf (stderr, "template<T> SCSyntype::SCSyntype(min,max,value): value[%ld] > maximum[%ld]\n",
             value, maximum);
    exit (1);
  }
#endif
}

template<class T> SCSyntype<T>::SCSyntype (const T pMinimum,
                                           const T pMaximum,
                                           const SCBoolean pNoMin,
                                           const SCBoolean pNoMax,
                                           const T pValue,
                                           const SCObject* father):
SCDataType(father)
{
  if (pNoMin)
    SetNoMin();
  else
    SetMin (pMinimum);
  if (pNoMax)
    SetNoMax();
  else
    SetMax (pMaximum);
  value = pValue;
}

template<class T> SCSyntype<T>::SCSyntype (const T pValue)
{
  SetNoMin();
  SetNoMax();
  value = pValue;
}

template<class T> SCSyntype<T>::~SCSyntype (void)
{
}


template<class T> SCStream &operator<< (SCStream &pStream,
                                       const SCSyntype<T> &pData)
{
  pStream << pData.GetValue();

  return (pStream);
}



// Operators can't be virtual, so this serves as a virtual interface to the
// <<-operator.

template<class T> SCStream &SCSyntype<T>::Display (SCStream &pStream) const
{
  return (pStream << *this);
}

/* Arithmetische Operationen zwischen zwei Syntypes */
template<class T> SCSyntype<T>& SCSyntype<T>::operator= (const SCSyntype<T>& s)
{
  if (this == &s)
    return (*this);
  copy (s);
  return (*this);
}

template<class T> SCSyntype<T> SCSyntype<T>::operator* (const SCSyntype<T>& second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value * second.value);
  return (s);
}

template<class T> SCSyntype<T> SCSyntype<T>::operator/ (const SCSyntype<T>& second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value / second.value);
  return (s);
}

template<class T> SCSyntype<T> SCSyntype<T>::operator+ (const SCSyntype<T>& second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value + second.value);
  return (s);
}

template<class T> SCSyntype<T> SCSyntype<T>::operator- (const SCSyntype<T>& second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value - second.value);
  return (s);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator% (const SCSyntype<T>& second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value % second.value);
  return (s);
}


/* Arithmetische Operationen zwischen einem Syntype ueber T und der
 * konstruierenden Klasse T
 */
template<class T> SCSyntype<T>& SCSyntype<T>::operator= (const T second)
{
  SCSyntype<T> tmp (second);                 /* noetig, damit auch Range-  */
                                             /* Verletzung erkannt wird    */ 
  copy (second);
  return (*this);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator* (const T second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value * second);
  return (s);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator/ (const T second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value / second);
  return (s);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator+ (const T second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value + second);
  return (s);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator- (const T second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value - second);
  return (s);
}


template<class T> SCSyntype<T> SCSyntype<T>::operator% (const T second) const
{
  SCSyntype<T> s (*this);

  s.SetValue (value % second);
  return (s);
}

#endif  // __SCSYNTYPE_TMPL__
