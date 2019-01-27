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
|   SCListSave  |   SCListSave.h    | 24. Feb 1996  |   SCL             |       |
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

#ifndef __SCLISTSAVE__

#define __SCLISTSAVE__

#include "SCList.h"

#if _SC_LOCKDATA
  #include "SCMutexTypes.h"
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2

enum SCQueueStatus
{
  kSCQueueUnmodified,
  kSCQueueEnqueued,
  kSCQueueDequeued,
  kSCQueueCorrupted
};

#endif

template<class T> class SCListSave: public SCList<T>
#if _SC_LOCKDATA
, public SCMutex
#endif
{
  public:
                    SCListSave (SCBoolean pDeleteElems = true,
                                const SCObject* father = NULL);
           virtual ~SCListSave (void);

    void            RemoveAllElements(void);

    SCListCons<T> * InsertBefore (T* toInsert,
                                  T* before);
    SCListCons<T> * InsertBefore (T* toInsert,
                                  class SCListCons<T>* before = NULL);
    SCListCons<T> * InsertAfter (T* toInsert,
                                 T* after);
    SCListCons<T> * InsertAfter (T* toInsert,
                                 class SCListCons<T>* after = NULL);
    T *             Remove (T* element);
    T *             Remove (class SCListCons<T>* element = NULL);
    T *             Remove (SCNatural index);

//    T *             operator[] (SCNatural index) const {return (*GetElem(index))(); }

    SCBoolean       Save (SCMem &saveArea) const;
    SCBoolean       Restore (SCMem &saveArea);
    void            Size(SCSize *pSize) const;

#if _SC_VALIDATION_OPTIMIZE
    SCNatural       GetCurrentSize (void) const
                      { return currentSize; }
    void            SetCurrentSize (SCNatural newSize)
                      { currentSize = newSize; }
    SCNatural       GetCurrentHistorySize (void) const
                      { return currentHistorySize; }
    void            SetCurrentHistorySize (SCNatural newSize)
                      { currentHistorySize = newSize; }
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
    SCQueueStatus   GetModified (void) const { return modified; }
    void            SetModified (SCQueueStatus mdfy) { modified = mdfy; }
    void            UnsetModified (void) { modified = kSCQueueUnmodified; }
#endif

  private:

#if _SC_VALIDATION_OPTIMIZE
    SCNatural       currentSize;
    SCNatural       currentHistorySize;
#endif

#if _SC_VALIDATION_OPTIMIZE >= 2
    SCQueueStatus   modified;
#endif

#if _SC_VALIDATION_OPTIMIZE
    class SCRunnable * runnable; // father is always a runnable!
    SCSize elemSize;             // only for temorary use!
#endif
};

#ifndef __GNUC__
#include "SCListSave.tmpl.h"  // necessary for CC compiler
#endif

#endif  //  __SCLISTSAVE__
