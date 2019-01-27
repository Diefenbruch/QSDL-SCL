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
|   SCSyntype   |  SCSyntype.inl.h  | 28. Jul 1995  |   SCL             |       |
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

#ifndef __SCSYNTYPE_INL__

#define __SCSYNTYPE_INL__


/* Zuweisung mit optionaler Bereichspruefung */

template <class T> _SCINLINE_ void SCSyntype<T>::copy (const SCSyntype<T>& s)
{
#if _SC_RANGECHECK
  if (!noMin && s.value < minimum)
  {
    fprintf (stderr, "template<T> SCSyntype::copy(): value[%ld] < minimum[%ld]\n",
             s.value, minimum);
    assert (false);
  }
  if (!noMax && s.value > maximum)
  {
    fprintf (stderr, "template<T> SCSyntype::copy(): value[%ld] > maximum[%ld]\n",
             s.value, maximum);
    assert (false);
  }
#endif

  value = s.value;
}


/* Hilfsfunktionen */

template<class T> _SCINLINE_ T SCSyntype<T>::GetValue (void) const
{
    return (value);
}


template<class T> _SCINLINE_ void SCSyntype<T>::SetValue (const T pValue)
{
    value = pValue;
}


template<class T> _SCINLINE_ void SCSyntype<T>::SetMin (const T pMinimum)
{
    minimum = pMinimum;
    noMin = false;
}


template<class T> _SCINLINE_ void SCSyntype<T>::SetNoMin (void)
{
    noMin = true;
}


template<class T> _SCINLINE_ void SCSyntype<T>::SetMax (const T pMaximum)
{
    maximum = pMaximum;
    noMax = false;
}


template<class T> _SCINLINE_ void SCSyntype<T>::SetNoMax (void)
{
    noMax = true;
}


/* Umwandlung von Syntype<T> in T */

template<class T> _SCINLINE_ SCSyntype<T>::operator T (void) const
{
    return (value);
}


/* Vergleichsoperatoren zwischen zwei Syntypes */

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator== (const SCSyntype<T>& second) const
{
    return (value == second.value);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator!= (const SCSyntype<T>& second) const
{
    return (value != second.value);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator> (const SCSyntype<T>& second) const
{
    return (value > second.value);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator< (const SCSyntype<T>& second) const
{
    return (value < second.value);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator>= (const SCSyntype<T>& second) const
{
    return (value >= second.value);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator<= (const SCSyntype<T>& second) const
{
    return (value <= second.value);
}


/* Vergleichsoperatoren zwischen einem Syntype ueber T und der
 * konstruierenden Klasse T
 */
 
template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator== (const T second) const
{
    return (value == second);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator!= (const T second) const
{
    return (value != second);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator> (const T second) const
{
    return (value > second);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator< (const T second) const
{
    return (value < second);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator>= (const T second) const
{
    return (value >= second);
}

template<class T> _SCINLINE_ SCBoolean SCSyntype<T>::operator<= (const T second) const
{
    return (value <= second);
}


#endif  // __SCSYNTYPE_INL__
