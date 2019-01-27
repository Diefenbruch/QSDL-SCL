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
|  SCListIter   |   SCListIter.h    | 24. Feb 1996  |   SCL             |       |
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

#ifndef __SCLISTITER__

#define __SCLISTITER__

#include <iostream>
#include "SCListCons.h"
#include "SCList.h"

template<class T> class SCListIter
{
  private:
    class SCListCons<T> * ptr;
    const SCList<T>&      myList;   // The list we are working on.

  public:
                          SCListIter (const SCList<T> &list,
                                      SCBoolean startAtHead = true);
    T *                   operator++ (int);
    T *                   operator-- (int);
    SCListCons<T>*        GetCurrentCons (void) const;
    SCBoolean             SetCurrentCons (SCListCons<T> *newCons);
    T *                   GoToFirst (void);
    T *                   GoToLast (void);
};


template<class T> inline SCListIter<T>::SCListIter (const SCList<T> &list,
                                                    SCBoolean startAtHead) :
  myList (list)
{
  if (startAtHead)            // This allows us to choose
                              // whether we want to start
                              // from the start or the end
                              // of the list.
  {
    ptr = myList.Head();
  }
  else
  {
    ptr = myList.Tail();
  }
}


template<class T> inline T *SCListIter<T>::operator++ (int)
{
  if (ptr)
  {
    SCListCons<T>  *p = ptr;

    ptr = ptr->Next();
    return (*p)();
  }
  return NULL;
}


template<class T> inline T *SCListIter<T>::operator-- (int)
{
  if (ptr)
  {
    SCListCons<T>  *p = ptr;

    ptr = ptr->Prev();
    return (*p)();
  }
  return NULL;
}


template<class T> SCListCons<T>* SCListIter<T>::GetCurrentCons (void) const
{
  return ptr;
}


template<class T> SCBoolean SCListIter<T>::SetCurrentCons (SCListCons<T> * newCons)
{
  assert(newCons);
  
  if (!newCons)
    return false;
    
  if (newCons->myList != &myList)
  {
    std::cerr << "Cannot set iterator to elem not in list!" << std::endl;
    
    assert(false);
    
    return false;
  }
  
  ptr = newCons;
  
  return true;
}


template<class T> inline T* SCListIter<T>::GoToFirst (void)
{
  ptr = myList.Head();

  if (ptr)
    return (*ptr)();
  
  return NULL;
}


template<class T> inline T* SCListIter<T>::GoToLast (void)
{
  ptr = myList.Tail();
  
  if (ptr)
    return (*ptr)();

  return NULL;
}


#endif  // __SCLISTITER__

