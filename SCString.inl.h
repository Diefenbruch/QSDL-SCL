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
|  SCString     |   SCString.inl.h  | 22. Okt 1997  |   SCL             |       |
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


#ifndef __SCSTRING_INL__

#define __SCSTRING_INL__


template<class T> _SCINLINEC_ SCString<T> & SCString<T>::MkString(const T & initValue)
{
  return *(new SCString<T>(initValue));
}

template<class T> _SCINLINE_ SCBoolean SCString<T>::Equal (const SCDataType &second) const
{
  return (*this == *(SCString<T> *)&second);
}


template<class T> _SCINLINE_ SCDataType *SCString<T>::Clone (void) const
{
  return (new SCString<T> (*this));
}


template<class T> _SCINLINE_ T SCString<T>::First(void) const
{
  T *elem;

  assert(this.Head());

//  elem = (*Head())();
  elem = (this->Head())();
  assert(elem);

  return *elem;
}


template<class T> _SCINLINE_ T SCString<T>::Last(void) const
{
  T *elem;

  assert(this.Tail());

//  elem = (*Tail())();
  elem = (this->Tail())();
  assert(elem);

  return *elem;
}


template<class T> _SCINLINE_ SCInteger SCString<T>::Length(void) const
{
  return this.NumOfElems();
}

#endif  // __SCSTRING_INL__
