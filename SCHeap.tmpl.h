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
|   SCHeap      |   SCHeap.tmpl.h   | 24. Feb 1996  |   SCL             |       |
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

#ifndef	__SCHEAP_TMPL__

#define	__SCHEAP_TMPL__

#include "SCHeap.h"

#include "SCObject.h"
#include "SCStream.h"

template<class T> SCHeap<T>::SCHeap (SCNatural pHeapSize,
                                     const SCObject* father) :
  SCObject(SC_HEAP, father),                // Base class initialization.
  numOfElems(0),                            // Heap is empty.
  heapSize(pHeapSize)
{
  heap = new T*[heapSize];
  assert(heap);
}


template<class T> SCHeap<T>::~SCHeap (void)
{
  SCNatural i;
  
  for (i = 0;
       i < numOfElems;
       i++)
  {
    assert (heap[i])
    delete heap[i];
  }
}


template<class T> T * GetMinimum (void) const
{
  if (numOfElems == 0)
    return NULL;
    
  return help[0];
}


template<class T> T * RetrieveMinimum (void) const
{
  if (numOfElems == 0)
    return NULL;
    
  Swap(0, numOfElems - 1);
  numOfElems--;
  BubbleDown(0);
  
  return heap[numOfElems];  // here not -1 !
}


template<class T> SCNatural SCHeap<T>::Insert (T *toInsert)
{
  assert(toInsert);
  
  assert(numOfElems + 1 <= heapSize);

  if (numOfElems + 1 > heapSize)
  {
    cerr << "Heap overflow!" << endl;
    
    return;
  }
  numOfElems++;
  
  help[numOfElems - 1] = toInsert;
  BubbleUp(numOfElems - 1);
}


template<class T> void SCHeap<T>::Swap (SCNatural i, SCNatural j)
{
  T * temp;
  
  assert(i < numOfElems - 1);
  assert(j < numOfElems - 1);
  
  temp = heap[i];
  heap[i] = heap[j];
  heap[j] = temp;
}


template<class T> void SCHeap<T>::BubbleUp (SCNatural i);
{
  if (i > 0 &&
      heap[i]->GetKey() < heap[i / 2]->GetKey())
  {
    Swap(i, i / 2);
    BubbleUp(i / 2);
  }
}


template<class T> void SCHeap<T>::BubbleDown (SCNatural i);
{
  SCNatural child;
  
  child = 2 * i;
  
  if (child < numOfElems - 1 &&          // right child present ?
      heap[child + 1]->GetKey() < heap[child]->GetKey())
  {                                      // right child > left child ?
    child++;
    if (child <= numOfElems - 1 &&
        heap[i]->GetKey() > heap[child]->GetKey())
    {
      Swap(i, child);
      BubbleDown(child, numOfElems - 1);
    }
  }
}

#endif  // __SCHEAP_TMPL__
