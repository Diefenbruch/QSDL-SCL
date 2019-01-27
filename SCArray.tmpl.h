/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994 Marc Diefenbruch, Wolfgang Textor                                  |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|  SCArray      |   SCArray.tmpl    | 7. Jun 1994   |   SCL             |       |
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


#ifndef __SCARRAY_TMPL__

#define __SCARRAY_TMPL__

#include "SCArray.h"
#include "SCStream.h"

#if _SC_NOINLINES
  #include "SCArray.inl.h"
#endif

// for T with constructor:

template<class T> SCArray<T>::SCArray (const SCInteger low,
                                       const SCInteger high,
                                       const SCObject* father):
  SCDataType(father)
{
#if _SC_RANGECHECK
    #define NAME(x) #x
    if (low > high)
    {
      fprintf (stderr, "template<" NAME(T) "> SCArray::SCArray(low, high, init): low[%ld] > high[%ld]\n",
               low, high);
      exit (1);
    }
    #undef NAME
#endif

  lowBound = low;
  highBound = high;
  array = (low <= high) ? new T [high - low + 1] : (T *)NULL;
}

// for T without constructor:

template<class T> SCArray<T>::SCArray (const SCInteger low,
                                       const SCInteger high,
                                       const T         initValue,
                                       const SCObject* father):
  SCDataType(father)
{
  SCInteger z_i;

#if _SC_RANGECHECK
    #define NAME(x) #x
    if (low > high)
    {
      fprintf (stderr, "template<" NAME(T) "> SCArray::SCArray(low,high,init): low[%ld] > high[%ld]\n",
               low, high);
      exit (1);
    }
    #undef NAME
#endif
  lowBound = low;
  highBound = high;
  array = (low <= high) ? new T [high - low + 1] : (T *)NULL;
  if (array)
    for (z_i = low; z_i <= high; z_i++)
      (*this)[z_i] = initValue;
}


template<class T> SCArray<T>::SCArray (const SCArray<T>& orig,
                                       const SCObject* father):
  SCDataType(father)
{
  SCInteger   z_i;

  lowBound = orig.lowBound;
  highBound = orig.highBound;
  if (orig.array)
  {
    array = new T [orig.highBound - orig.lowBound + 1];
    for (z_i = lowBound; z_i <= highBound; z_i++)
    	(*this)[z_i] = orig[z_i];
  }
  else
    array = NULL;
}


template<class T> SCArray<T>::~SCArray (void)
{
  if (array)
    delete[] array;
}


template<class T> SCArray<T>& SCArray<T>::operator= (const SCArray<T>& orig)
{
  SCInteger   z_i;

  if (this != &orig)
  {
    if (array)
      delete[] array;
    lowBound = orig.lowBound;
    highBound = orig.highBound;
    if (orig.array)
    {
      array = new T [orig.highBound - orig.lowBound + 1];
      for (z_i = lowBound; z_i <= highBound; z_i++)
        (*this)[z_i] = orig[z_i];
    }
    else
      array = NULL;
  }
  return (*this);
}


template<class T> SCBoolean SCArray<T>::operator== (const SCArray<T> &second) const
{
  SCInteger z_i;
  SCInteger size = highBound - lowBound;

  for (z_i = 0; z_i <= size; z_i++)
    if (!(array[z_i] == second.array[z_i]))
      return (false);
  return (true);
}

/* Funktioniert nicht mit expliziter Template-Instantiierung! */
template<class T> SCStream &operator<< (SCStream &pStream, const SCArray<T> &pData)
{
  SCInteger z_i;

  pStream << "[";
  for (z_i = pData.GetLowBound();
       z_i < pData.GetHighBound();
       z_i++)
  {
    pStream << pData[z_i] << ", ";
  }
  pStream << pData[pData.GetHighBound()] <<  "]";

  return (pStream);
}


// Operators can't be virtual, so this serves as a virtual interface to the
// <<-operator.
template<class T> SCStream &SCArray<T>::Display (SCStream &pStream) const
{
  return (pStream << *this);
}

#endif  //  __SCARRAY_TMPL__
