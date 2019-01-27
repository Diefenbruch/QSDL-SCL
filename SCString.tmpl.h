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
|  SCString     |   SCString.tmpl   | 22. Okt 1997  |   SCL             |       |
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


#ifndef __SCSTRING_TMPL__

#define __SCSTRING_TMPL__

#include "SCMem.h"
#include "SCString.h"
#include "SCStream.h"

#if _SC_NOINLINES
  #include "SCString.inl.h"
#endif

// for T with constructor:

template<class T> SCString<T>::SCString (const SCObject* father):
  SCDataType(father),
  SCList<T>(true, father) // true = delete elems
{
}

// for T without constructor:

template<class T> SCString<T>::SCString (const T       & initValue,
                                         const SCObject* father):
  SCDataType(father),
  SCList<T>(true, father) // true = delete elems
{
  T *elem;
  
  elem = (T *)initValue.Clone();
  assert(elem);
  
  InsertAfter(elem);
}


template<class T> SCString<T>::SCString (const SCString<T>& orig,
                                         const SCObject* father):
  SCDataType(father),
  SCList<T>(true, father) // true = delete elems
{
  *this = orig;
}


template<class T> SCString<T>::~SCString (void)
{
}


template<class T> SCString<T>& SCString<T>::operator= (const SCString<T>& orig)
{
  SCListCons<T> *cur_pos = orig.Head();
  T * elem;
  
  if (this == &orig)
  {
    return (*this);
  }
  
  this.RemoveAllElements();
  
  while (cur_pos)
  {
    elem = (T *)(*cur_pos)()->Clone();
    InsertAfter(elem);
    
    cur_pos = cur_pos->Next();
  }
  
  return (*this);
}


template<class T> SCBoolean SCString<T>::operator== (const SCString<T> &second) const
{
  SCListCons<T> *cur_pos_orig = second.Head();
  SCListCons<T> *cur_pos_this = this.Head();

  if (this.NumOfElems() != second.NumOfElems())
    return false;

  while (cur_pos_orig && cur_pos_this)
  {
    if (!((*cur_pos_orig)() == (*cur_pos_this)()))
    {
      return false;
    }
    cur_pos_orig = cur_pos_orig->Next();
    cur_pos_this = cur_pos_this->Next();
  }

  return true;
}


template<class T> T & SCString<T>::operator[] (const SCNatural index) const
{
  SCListCons<T> *elem;

  assert(index >= 1);

  elem = this.GetElem(index - 1);
  assert(elem);

  return *(*elem)();
}


template<class T> SCString<T> SCString<T>::operator|| (const SCString<T>& second)
{
  SCListCons<T> *cur_pos = second.Head();
  T * elem;

  while (cur_pos)
  {
    elem = (T *)(*cur_pos)()->Clone();
    InsertAfter(elem);

    cur_pos = cur_pos->Next();
  }

  return *this;
}


template<class T> SCString<T> SCString<T>::SubString (const SCNatural start,
                                                      const SCNatural length) const
{
  SCString<T> substring;
  T * elem;
  SCNatural todo = length;
  SCListCons<T> *cur_pos = this.GetElem(start - 1); // SDL-Strings start with 1 !

  while (cur_pos && todo > 0)
  {
    elem = (T *)(*cur_pos)()->Clone();
    substring.InsertAfter(elem);

    cur_pos = cur_pos->Next();
    todo--;
  }

  return substring; // this calls copy-constructor!
                    // we don't want to dynamically allocate
                    // substring because that may cause memory
                    // leaks if the return value is only
                    // used temporarily!
}


template<class T> SCString<T>& SCString<T>::Modify (const SCNatural index,
                                                    const T & item)
{
  (*this)[index] = item;

  return (*this);
}


template<class T> SCStream &operator<< (SCStream &pStream, const SCString<T> &pData)
{
  return pData.SCList<T>::Display(pStream);  // virtuelle Methode von SCList aufrufen
}


template<class T> SCStream& SCString<T>::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


template<class T> SCBoolean SCString<T>::Save (SCMem &saveArea) const
{
  T *p;
  SCListCons<T> *ptr;

//  saveArea.HistoryStore(&numOfElems, sizeof(SCNatural));
  saveArea.HistoryStore(&this.numOfElems, sizeof(SCNatural));

  for (ptr = this.Head(); ptr; ptr = ptr->Next())
  {
    p = (*ptr)();
    assert(p);
    p->T::Save(saveArea);
  }
  return true;
}


template<class T> SCBoolean SCString<T>::Restore (SCMem &saveArea)
{
  T *p = NULL;
  SCNatural i, number;
  
  ////////////////////
  // 1. Empty list:
  ////////////////////

  this.RemoveAllElements();

  ////////////////////////////
  // 2. Read number of elems:
  ////////////////////////////

  saveArea.HistoryRestore(&number, sizeof(SCNatural));   // nicht in numOfElems einlesen!

  ////////////////////
  // 3. Refill list:
  ////////////////////

  for (i = 0; i < number; i++)
  {
    p = new T;
    p->T::Restore(saveArea);
    assert(p);
    InsertAfter(p); // inkrementiert numOfElems!
  }

  return true;
}


template<class T> void SCString<T>::Size(SCSize *curSize) const
{
  T *p = NULL;
  SCListCons<T> *ptr;

  curSize->historySize += sizeof(SCNatural); // for numOfElems

  for (ptr = this.Head(); ptr; ptr = ptr->Next())
  {
    p = (*ptr)();
    assert(p);
    p->T::Size(curSize);
  }
}

#endif  //  __SCSTRING_TMPL__
