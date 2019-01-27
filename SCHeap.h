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
|   SCHeap      |   SCHeap.h        | 24. Feb 1996  |   SCL             |       |
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

#ifndef __SCHEAP__

#define __SCHEAP__

#include "SCBasicTypes.h"
#include "SCObject.h"

template<class T> class SCHeap: public SCObject
{
  public:
                      SCHeap (SCNatural pNumOfElems = kSCHeapSize,
                              SCObject *pParent = NULL);
    virtual          ~SCHeap (void);

    T *               GetMinimum (void) const;
    T *               RetrieveMinimum (void);
    SCNatural         Insert (T *toInsert);
    SCNatural         NumOfElems(void) const { return numOfElems; }
    SCBoolean         IsEmpty(void) const { return (numOfElems == 0); }

    T *               operator[] (SCNatural index) const { assert(index <= numOfElems - 1); return heap[index]; }

    friend SCStream &  operator<< (SCStream &pStream,
                                  const SCHeap<T> &pData);
    virtual SCStream & Display (SCStream &pStream) const;

  protected:
  
    void              Swap (SCNatural i, SCNatural j);
    void              BubbleUp (SCNatural i);
    void              BubbleDown (SCNatural i);

  private:    
    T **              heap;
    SCNatural         numOfElems;
};

#ifndef __GNUC__
#include "SCHeap.tmpl.h"  // necessary for CC compiler
#endif

#endif  //  __SCHEAP__
