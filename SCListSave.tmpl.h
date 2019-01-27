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
|   SCListSave  | SCListSave.tmpl.h | 24. Feb 1996  |   SCL             |       |
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

#ifndef	__SCLISTSAVE_TMPL__

#define	__SCLISTSAVE_TMPL__

#include "SCStream.h"
#include "SCRunnable.h"
#include "SCListSave.h"
#include "SCListCons.h"
#include "SCListIter.h"
#include "SCMem.h"

#if _SC_VALIDATION_DEBUG >= 2
  #include "SCDebug.h"
#endif

#if _SC_PROFILING
extern int list_ineff;
#endif

template<class T> SCListSave<T>::SCListSave (SCBoolean pDeleteElems,
                                             const SCObject* father) :
  SCList<T>(pDeleteElems, father)
{
#if _SC_VALIDATION_OPTIMIZE
  currentSize = 0;
  currentHistorySize = sizeof(SCNatural);        // Member numOfElems
                                                 // from SCList<T>
#if _SC_VALIDATION_OPTIMIZE >= 2
  modified = kSCQueueCorrupted;
#endif

#endif

#if _SC_VALIDATION_OPTIMIZE
  runnable = (SCRunnable *)father;
#endif
}


template<class T> SCListSave<T>::~SCListSave (void)
{
}

template<class T> void SCListSave<T>::RemoveAllElements (void)
{
#if _SC_LOCKDATA
  Lock();
#endif

#if _SC_VALIDATION_OPTIMIZE
  runnable->currentSize -= currentSize;                 // Update size counter.
  runnable->currentHistorySize -= (currentHistorySize - // Update history size counter.
                                   sizeof(SCNatural));
  currentSize = 0;
  currentHistorySize = sizeof(SCNatural);        // Member numOfElems
                                                 // from SCList<T>
#if _SC_VALIDATION_OPTIMIZE >= 2
  modified = kSCQueueCorrupted;
#endif

#endif

  SCList<T>::RemoveAllElements();

#if _SC_LOCKDATA
  Unlock();
#endif
}

template<class T> SCListCons<T> * SCListSave<T>::InsertBefore (T *toInsert,
                                                               T *before)
{
  SCListCons<T> * retValue;

  assert (toInsert != NULL);

#if _SC_LOCKDATA
  Lock();
#endif

  retValue = SCList<T>::InsertBefore (toInsert, before);
                                                  // Let SCList<T> handle the
                                                  // insert operation.
  assert (retValue);

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  toInsert->T::Size(&elemSize);

  currentSize += elemSize.size;               // Update size counter.
  currentHistorySize += elemSize.historySize; // Update history size counter.

  runnable->currentSize += elemSize.size;               // Update size counter.
  runnable->currentHistorySize += elemSize.historySize; // Update history size counter.

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::InsertBefore(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(+" << elemSize.size << ")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(+" << elemSize.historySize << ")" << endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  modified = kSCQueueCorrupted;
#endif

#endif

  toInsert->SetContainer(retValue);               // tell toInsert object
                                                  // its current container
  toInsert->SetParent(this);                      // list is parent

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}


template<class T> SCListCons<T> * SCListSave<T>::InsertBefore (T             *toInsert,
                                                               SCListCons<T> *before)
{
  SCListCons<T> * retValue;

  assert (toInsert != NULL);

#if _SC_LOCKDATA
  Lock();
#endif

  retValue = SCList<T>::InsertBefore (toInsert,   // Let SCList<T> handle the
                                      before);    // insert operation.

  assert (retValue);

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  toInsert->T::Size(&elemSize);

  currentSize += elemSize.size;                // Update size counter.
  currentHistorySize += elemSize.historySize;

  runnable->currentSize += elemSize.size;                // Update size counter.
  runnable->currentHistorySize += elemSize.historySize;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::InsertBefore(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(+" << elemSize.size <<")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(+" << elemSize.historySize << ")" << endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  modified = kSCQueueCorrupted;
#endif

#endif

  toInsert->SetContainer(retValue);               // tell toInsert object
                                                  // its current container
  toInsert->SetParent(this);                      // list is parent

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}


template<class T> SCListCons<T> * SCListSave<T>::InsertAfter (T *toInsert,
                                                              T *after)
{
  SCListCons<T> * retValue;

  assert (toInsert != NULL);

#if _SC_LOCKDATA
  Lock();
#endif

  retValue = SCList<T>::InsertAfter (toInsert, after);

  assert (retValue);

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  toInsert->T::Size(&elemSize);

  currentSize += elemSize.size;
  currentHistorySize += elemSize.historySize;

  runnable->currentSize += elemSize.size;
  runnable->currentHistorySize += elemSize.historySize;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::InsertAfter(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(+" << elemSize.size <<")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(+" << elemSize.historySize << ")" << std::endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  if (modified == kSCQueueUnmodified && // first modification ?
      retValue == this->tail)           // append ?
  {
    modified = kSCQueueEnqueued;
  }
  else
  {
    modified = kSCQueueCorrupted;
  }
#endif

#endif

  toInsert->SetContainer(retValue);                 // tell toInsert object
                                                    // its current container
  toInsert->SetParent(this);                        // list is parent

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}


template<class T> SCListCons<T> * SCListSave<T>::InsertAfter (T             *toInsert,
                                                              SCListCons<T> *after)
{
  SCListCons<T> * retValue;

  assert (toInsert != NULL);

#if _SC_LOCKDATA
  Lock();
#endif

  retValue = SCList<T>::InsertAfter (toInsert, after);

  assert (retValue);

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  toInsert->T::Size(&elemSize);

  currentSize += elemSize.size;
  currentHistorySize += elemSize.historySize;

  runnable->currentSize += elemSize.size;
  runnable->currentHistorySize += elemSize.historySize;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::InsertAfter(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(+" << elemSize.size <<")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(+" << elemSize.historySize << ")" << endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  if (modified == kSCQueueUnmodified && // first modification ?
      retValue == this->tail)           // append ?
  {
    modified = kSCQueueEnqueued;
  }
  else
  {
    modified = kSCQueueCorrupted;
  }
#endif

#endif

  toInsert->SetContainer(retValue);               // tell toInsert object
                                                  // its current container
  toInsert->SetParent(this);                      // list is parent

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}


template<class T> T *SCListSave<T>::Remove (T *element)
{
  T *retValue;

#if _SC_LOCKDATA
  Lock();
#endif

  if (!this->head)
  {
#if _SC_LOCKDATA
    Unlock();
#endif
    return NULL;
  }

  if (element && element->GetContainer())    // possible to use efficient
  {                                          // version of Remove() ?
#if _SC_VALIDATION_OPTIMIZE >= 2
    if (element->GetContainer() == this->head &&   // remove head ?
        modified == kSCQueueUnmodified)      // first modification ?
    {
      modified = kSCQueueDequeued;
    }
    else
    {
      modified = kSCQueueCorrupted;
    }
#endif

    retValue = SCList<T>::Remove (element->GetContainer());
  }
  else                                       // we must use slow version!
  {
#if _SC_PROFILING
    cerr << "SCListSave<T>::Remove(): Using inefficient method!" << std::endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
    modified = kSCQueueCorrupted;
#endif

    retValue = SCList<T>::Remove (element);
  }

  if (!retValue)
  {
#if _SC_LOCKDATA
    Unlock();
#endif
    std::cerr << "SCListSave<T>::Remove(): Can't find element!" << std::endl;
    assert (false);

    return NULL;
  }

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  retValue->T::Size(&elemSize);

  currentSize -= elemSize.size;
  currentHistorySize -= elemSize.historySize;

  runnable->currentSize -= elemSize.size;
  runnable->currentHistorySize -= elemSize.historySize;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::Remove(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(-" << elemSize.size <<")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(-" << elemSize.historySize << ")" << std::endl;
#endif

#endif

  retValue->SetContainer (NULL);
  retValue->SetParent(NULL);

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}

template<class T> T *SCListSave<T>::Remove (SCListCons<T> *element)
{
  T *retValue;

#if _SC_LOCKDATA
  Lock();
#endif

  if (!this->head)
  {
#if _SC_LOCKDATA
    Unlock();
#endif
    return NULL;
  }
  retValue = SCList<T>::Remove (element);

  if (!retValue)
  {
#if _SC_LOCKDATA
    Unlock();
#endif
    return NULL;
  }

  retValue->SetContainer (NULL);
  retValue->SetParent(NULL);

#if _SC_VALIDATION_OPTIMIZE
  elemSize.size = 0;
  elemSize.historySize = 0;

  retValue->T::Size(&elemSize);

  currentSize -= elemSize.size;
  currentHistorySize -= elemSize.historySize;

  runnable->currentSize -= elemSize.size;
  runnable->currentHistorySize -= elemSize.historySize;

#if _SC_VALIDATION_DEBUG >= 2
  scValidationDebugLog << "SCListSave::Remove(): adapting size of runnable " << *runnable;
  scValidationDebugLog << " to size = " << runnable->currentSize << "(-" << elemSize.size <<")";
  scValidationDebugLog << " and hist size = " << runnable->currentHistorySize << "(-" << elemSize.historySize << ")" << endl;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
  if (!element && modified == kSCQueueUnmodified) // dequeue ?
    modified = kSCQueueDequeued;
  else
    modified = kSCQueueCorrupted;
#endif

#endif

#if _SC_LOCKDATA
  Unlock();
#endif

  return retValue;
}


template<class T> T *SCListSave<T>::Remove (SCNatural index)
{
  return Remove(this->GetElem(index));
}


template<class T> SCBoolean SCListSave<T>::Save (SCMem &saveArea) const
{
  T *p;
  SCListCons<T> *ptr;

  saveArea.HistoryStore(&this->numOfElems, sizeof(SCNatural));

  for (ptr = this->head; ptr; ptr = ptr->Next())
  {
    p = (*ptr)();
    assert(p);
    p->T::Save(saveArea);

#if _SC_PROFILING
    list_ineff++;
#endif
  }
  return true;
}

template<class T> SCBoolean SCListSave<T>::Restore (SCMem &saveArea)
{
  T *p = NULL;
  SCNatural i, number;
  
#if _SC_VALIDATION_OPTIMIZE >= 2

  size_t size, historySize;

  switch (modified)
  {
    case kSCQueueUnmodified:           // Liste gegenueber dem
                                       // vorangegangenen System-
                                       // zustand unveraendert ?
      saveArea.SkipOffset(currentSize);
      saveArea.SkipHistoryOffset(currentHistorySize);

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCListSave::Restore(): skipped unchanged list with ";
      scValidationDebugLog << numOfElems << " elements, ";
      scValidationDebugLog << "size = " << currentSize;
      scValidationDebugLog << ", history size = " << currentHistorySize << endl;
      scValidationDebugLog << "SCListSave::Restore(): ";
      scValidationDebugLog << "offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif

      return true;

    case kSCQueueEnqueued:
#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCListSave::Restore(): remove tail from enqueued list with ";
      scValidationDebugLog << numOfElems << " elements, ";
      scValidationDebugLog << "size = " << currentSize;
      scValidationDebugLog << ", history size = " << currentHistorySize << std::endl;
#endif

      delete Remove (this->tail);        // remove last element (decreases current size)

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCListSave::Restore(): removed tail from enqueued list now ";
      scValidationDebugLog << numOfElems << " elements, ";
      scValidationDebugLog << "size = " << currentSize;
      scValidationDebugLog << ", history size = " << currentHistorySize << std::endl;
#endif

      saveArea.SkipOffset(currentSize);
      saveArea.SkipHistoryOffset(currentHistorySize);

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCListSave::Restore(): ";
      scValidationDebugLog << "offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif

      return true;

    case kSCQueueDequeued:
      size = currentSize;
      historySize = currentHistorySize - sizeof(SCNatural);

      saveArea.HistoryRestore(&number,
                              sizeof(SCNatural)); // do not store in numOfElems !
      assert (number > 0);                        // if queue was dequeued it
                                                  // was not empty !
      p = new T(saveArea, this);                  // restore first element
      assert(p);
      InsertBefore(p);                            // insert new head (this
                                                  // increments numOfElems!)
      saveArea.SkipOffset(size);                  // skip rest of list
      saveArea.SkipHistoryOffset(historySize);

#if _SC_VALIDATION_DEBUG >= 2
      scValidationDebugLog << "SCListSave::Restore(): reinsert head of dequeued list with ";
      scValidationDebugLog << numOfElems << " elements, ";
      scValidationDebugLog << "size = " << currentSize;
      scValidationDebugLog << ", history size = " << currentHistorySize << endl;
      scValidationDebugLog << "SCListSave::Restore(): ";
      scValidationDebugLog << "offset now " << saveArea.GetOffset();
      scValidationDebugLog << ", histoffset now " << saveArea.GetHistoryOffset() << endl;
#endif

      return true;


    case kSCQueueCorrupted:           // Queue ist mehrfach oder
                                      // nicht an den Enden veraendert worden
      RemoveAllElements();
      saveArea.HistoryRestore(&number,
                              sizeof(SCNatural));   // nicht in numOfElems einlesen!

      for (i = 0; i < number; i++)
      {
        p = new T(saveArea, this);
        assert(p);
        InsertAfter(p); // inkrementiert numOfElems!

#if _SC_PROFILING
        list_ineff++;
#endif
      }
      return true;

    default:
      assert(false);
  }

#else // ! _SC_VALIDATION_OPTIMIZE >= 2

  ////////////////////
  // 1. Empty list:
  ////////////////////

  RemoveAllElements();

  ////////////////////////////
  // 2. Read number of elems:
  ////////////////////////////

  saveArea.HistoryRestore(&number, sizeof(SCNatural));   // nicht in numOfElems einlesen!

#if _SC_VALIDATION_DEBUG >= 2
    scValidationDebugLog << "SCListSave::Restore(): found " << number << " elements in save area" << endl;
#endif

  ////////////////////
  // 3. Refill list:
  ////////////////////

  for (i = 0; i < number; i++)
  {
    p = new T(saveArea, this);
    assert(p);
    InsertAfter(p); // inkrementiert numOfElems!

#if _SC_PROFILING
    list_ineff++;
#endif
  }
#endif // _SC_VALIDATION_OPTIMIZE >= 2

  return true;
}


template<class T> void SCListSave<T>::Size(SCSize *curSize) const
{
#if _SC_VALIDATION_OPTIMIZE
  curSize->size += currentSize;
  curSize->historySize += currentHistorySize;
#else

  T *p = NULL;
  SCListCons<T> *ptr;

  curSize->historySize += sizeof(SCNatural); // for numOfElems

  for (ptr = head; ptr; ptr = ptr->Next())
  {
    p = (*ptr)();
    assert(p);
    p->T::Size(curSize);

#if _SC_PROFILING
    list_ineff++;
#endif
  }
#endif
}

#endif  // __SCLISTSAVE_TMPL__
