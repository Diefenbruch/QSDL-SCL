/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCList      |   SCList.h        | 24. Feb 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#ifndef __SCLIST__

#define __SCLIST__

#include "SCBasicTypes.h"
#include "SCObject.h"
#include "SCListCons.h"


template<class T> class SCList: public SCObject
{
  public:
                           SCList (SCBoolean pDeleteElems = true,
                                   const SCObject* father = NULL);
                  virtual ~SCList (void);

    void                   RemoveAllElements(void);
    SCListCons<T> *        InsertBefore (T* toInsert,
                                         T* before);
    SCListCons<T> *        InsertBefore (T* toInsert,
                                         class SCListCons<T>* before = NULL);
    SCListCons<T> *        InsertAfter (T* toInsert,
                                        T* after);
    SCListCons<T> *        InsertAfter (T* toInsert,
                                        class SCListCons<T>* after = NULL);
    T *                    Remove (T* element);
    T *                    Remove (class SCListCons<T>* element = NULL);
    T *                    Remove (SCNatural index);
    SCListCons<T> *        Head (void) const { return head; };
    SCListCons<T> *        Tail (void) const { return tail; };
    SCNatural              NumOfElems(void) const { return numOfElems; }
    SCBoolean              IsEmpty(void) const { return (numOfElems == 0); }
    SCBoolean              GetDeleteElems(void) const { return deleteElems; }
    void                   SetDeleteElems(SCBoolean value) { deleteElems = value; }
    SCListCons<T> *        GetElem (SCNatural index) const;
    T *                    operator[] (SCNatural index) const {return (*GetElem(index))(); }

    virtual SCStream &     Display (SCStream &pStream) const;
      
    SCBoolean              operator== (const SCList & second) const;
    SCBoolean              operator!= (const SCList & second) const;

  protected:
    SCListCons<T> *        head;
    SCListCons<T> *        tail;
    SCBoolean              deleteElems;
    SCNatural              numOfElems;
};


#ifndef __GNUC__
#include "SCList.tmpl.h"  // necessary for CC compiler
#endif

#endif  //  __SCLIST__
