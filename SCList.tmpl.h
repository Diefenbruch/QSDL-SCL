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
|   SCList      |   SCList.tmpl.h   | 24. Feb 1996  |   SCL             |       |
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

#ifndef	__SCLIST_TMPL__

#define	__SCLIST_TMPL__

#if _SC_PROFILING
extern int list_ineff;
#endif

#include "SCList.h"

#include "SCObject.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCStream.h"

template<class T> SCList<T>::SCList (SCBoolean pDeleteElems,
                                     const SCObject* father) :
  SCObject(SC_LIST, father),                      // Base class initialization.
  head (NULL),
  tail (NULL),
  deleteElems (pDeleteElems),
  numOfElems(0)                                   // List is empty.
{
}


template<class T> SCList<T>::~SCList (void)
{
  RemoveAllElements();
}


template<class T> void SCList<T>::RemoveAllElements(void)
{
  SCListCons<T> *loop;
  SCListCons<T> *nextLoop;

  loop = head;
  while (loop)
  {
    nextLoop = loop->Next();
    if (deleteElems)
      delete (*loop)();
    delete loop;
    loop = nextLoop;
#if _SC_PROFILING
    list_ineff++;
#endif
  }
  head = NULL;
  tail = NULL;
  numOfElems = 0;
}


template<class T> SCListCons<T> * SCList<T>::InsertBefore (T *toInsert, T *before)
{
  SCListCons<T> *newCons;
  SCListCons<T> *p;

  assert (toInsert != NULL);

  std::cerr << "Inefficient use of SCList<T>::InsertBefore()" << std::endl;

  for (p = head;
       p;
       p = p->Next())
  {
    if ((*p)() == before)
    {
      newCons = new SCListCons<T> (toInsert, p->Prev(), p, this);
      assert (newCons != NULL);

      if (p->Prev())
        p->Prev()->SetNext (newCons);
      else
        head = newCons;
      p->SetPrev (newCons);
      numOfElems++;

      return newCons;
    }
#if _SC_PROFILING
    list_ineff++;
#endif
  }
  return NULL;
}


template<class T> SCListCons<T> * SCList<T>::InsertBefore (T *toInsert, SCListCons<T> *before)
{
  SCListCons<T> *newCons;

  assert (toInsert != NULL);
  if (before)
  {
    assert (before->List() == this);                // before part of THIS list?

    newCons = new SCListCons<T> (toInsert, before->Prev(), before, this);
    assert (newCons != NULL);

    if (before->Prev())
      before->Prev()->SetNext (newCons);
    else
      head = newCons;
    before->SetPrev (newCons);
  }
  else
  {
    newCons = new SCListCons<T> (toInsert, NULL, NULL, this);
    assert (newCons != NULL);

    if (head)
    {
      head->SetPrev (newCons);
      newCons->SetNext (head);
      head = newCons;
    }
    else
    {
      head = newCons;
      tail = newCons;
    }
  }
  numOfElems++;

  return newCons;
}


template<class T> SCListCons<T> * SCList<T>::InsertAfter (T *toInsert, T *after)
{
  SCListCons<T> *newCons;
  SCListCons<T> *p;

  assert (toInsert != NULL);

  std::cerr << "Inefficient use of SCList<T>::InsertAfter()" << std::endl;

  for (p = head;
       p;
       p = p->Next())
  {
    if ((*p)() == after)
    {
      newCons = new SCListCons<T> (toInsert, p, p->Next(), this);
      assert (newCons != NULL);

      if (p->Next())
        p->Next()->SetPrev (newCons);
      else
        tail = newCons;
      p->SetNext (newCons);
      numOfElems++;

      return newCons;
    }
#if _SC_PROFILING
    list_ineff++;
#endif
  }

  return NULL;
}


template<class T> SCListCons<T> * SCList<T>::InsertAfter (T *toInsert, SCListCons<T> *after)
{
  SCListCons<T> *newCons;

  assert (toInsert != NULL);

  if (after)
  {
    assert (after->List() == this);                // after part of THIS list?

    newCons = new SCListCons<T> (toInsert, after, after->Next(), this);
    assert (newCons != NULL);

    if (after->Next())
      after->Next()->SetPrev (newCons);
    else
      tail = newCons;
    after->SetNext (newCons);
  }
  else
  {
    newCons = new SCListCons<T> (toInsert, NULL, NULL, this);
    assert (newCons != NULL);

    if (tail)
    {
      tail->SetNext (newCons);
      newCons->SetPrev (tail);
      tail = newCons;
    }
    else
    {
      tail = newCons;
      head = newCons;
    }
  }
  numOfElems++;

  return newCons;
}


template<class T> T *SCList<T>::Remove (T *element)
{
  T             *p = NULL;
  SCListCons<T> *ptr;

  if (!head)
    return NULL;

  if (element == NULL)                            // Default is to remove head
                                                  // of list
  {
    return Remove (head);                         // this is NOT recursive
                                                  // as head is of type SCListCons<T>!
  }

  std::cerr << "Inefficient use of SCList<T>::Remove()" << std::endl;

  for (ptr = head; ptr; ptr = ptr->Next())        // Scan through list...
  {
    if ((*ptr)() == element)
    {
      if (ptr->Prev())
        ptr->Prev()->SetNext (ptr->Next());
      else
        head = ptr->Next();
      if (ptr->Next())
        ptr->Next()->SetPrev (ptr->Prev());
      else
        tail = ptr->Prev();
      p = (*ptr)();
      delete ptr;
      numOfElems--;
      break;
    }
#if _SC_PROFILING
    list_ineff++;
#endif
  }
  return p;
}


template<class T> T *SCList<T>::Remove (SCListCons<T> *element)
{
  T *p = NULL;

  if (!head)
    return NULL;

  if (element == NULL)
  {
    element = head;
  }

  assert (element->List() == this);   // Check if the element is in THIS list.
                                      // Every SCListCons<T> object knows into
                                      // which list it was inserted.
                                      // HERE: Double insertions?
                                      // HERE: Insertion into multiple lists?

  if (element->Prev())
  {
    element->Prev()->SetNext (element->Next());
  }
  else
  {
    head = element->Next();
  }
  if (element->Next())
  {
    element->Next()->SetPrev (element->Prev());
  }
  else
  {
    tail = element->Prev();
  }
  p = (*element)();
  delete element;
  numOfElems--;

  return p;
}


template<class T> SCStream &operator<< (SCStream &pStream, const SCList<T> &pData)
{
  SCListCons<T> *loop;

  pStream << "[";
  loop = pData.Head();
  while (loop)
  {
    assert((*loop)());
    pStream << *(*loop)();
    loop = loop->Next();
    if (loop)
      pStream << ", ";
  }
  pStream << "]";

  return pStream;
}


template<class T> SCStream& SCList<T>::Display(SCStream& pStream) const
{
  return (pStream << *this);
}


template<class T> SCBoolean SCList<T>::operator==(const SCList<T> &second) const
{
  if (NumOfElems() != second.NumOfElems())
    return false;

  return true;
}


template<class T> SCBoolean SCList<T>::operator!=(const SCList<T> &second) const
{
  return !(*this == second);
}


template<class T> SCListCons<T> *SCList<T>::GetElem(SCNatural index) const
{
  SCListCons<T> *loop;

  if (index >= numOfElems)
    return NULL;
    
  for (loop = head;
       loop && index;
       loop = loop->Next(), index--)
  {
  }
  
  assert(loop);
  
  return loop;
}


template<class T> T *SCList<T>::Remove (SCNatural index)
{
  return Remove(GetElem(index));
}

#endif  // __SCLIST_TMPL__
