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
|  SCListCons   |   SCListCons.h    | 24. Feb 1996  |   SCL             |       |
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


#ifndef __SCLISTCONS__

#define __SCLISTCONS__


template<class T> class SCList;
template<class T> class SCListIter;

template<class T>class SCListCons
{
  private:
    SCListCons<T> * prev;
    SCListCons<T> * next;
    T *             elem;
    SCList<T> *     myList;

  public:
                    SCListCons (T *e,
                                SCListCons<T> *p,
                                SCListCons<T> *n,
                                SCList<T> *list) :
                      prev (p), next (n), elem(e), myList(list) {}
    T *             operator() (void) const { return elem; };
    SCListCons<T> * Prev (void) const { return prev; };
    SCListCons<T> * Next (void) const { return next; };
    SCList<T> *     List (void) const { return myList; };
    void            SetPrev (SCListCons<T> *p) { prev = p; };
    void            SetNext (SCListCons<T> *n) { next = n; };

  friend class SCListIter<T>;
};

#endif  //  __SCLISTCONS__
