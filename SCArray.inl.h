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
|  SCArray      |   SCArray.inl.h   | 26. Jul 1995  |   SCL             |       |
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


#ifndef __SCARRAY_INL__

#define __SCARRAY_INL__


template<class T> _SCINLINE_ T& SCArray<T>::operator[] (const SCInteger index) const
{
#if _SC_RANGECHECK
    #define NAME(x) #x
    if (index < lowBound)
    {
      fprintf (stderr, "template<" NAME(T) "> SCArray::operator[](index): index[%ld] < low[%ld]\n",
               index, lowBound);
      assert(false);
    }
    if (index > highBound)
    {
      fprintf (stderr, "template<" NAME(T) "> SCArray::operator[](index): index[%ld] > high[%ld]\n",
               index, highBound);
      assert(false);
    }
    #undef NAME
#endif

  return (array[index - lowBound]);
}


template<class T> _SCINLINE_ SCBoolean SCArray<T>::Equal (const SCDataType &second) const
{
  return (*this == *(SCArray<T> *)&second);
}


template<class T> _SCINLINE_ SCDataType *SCArray<T>::Clone (void) const
{
  return (new SCArray<T> (*this));
}

#endif  // __SCARRAY_INL__
