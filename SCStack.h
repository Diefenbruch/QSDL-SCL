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
|  SCStack      |  SCStack.h        | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCSTACK__

#define __SCSTACK__

#include "SCBasicTypes.h"
#include "SCStackElemListTable.h"

class SCStack
{
  public:
                           SCStack (SCNatural = kSCMaxDepth,
                                    SCBoolean doCycleDetection = false);
                          ~SCStack (void);

    SCBoolean              Push (class SCStackElem *);
    class SCStackElem *    Pop (void);
    class SCStackElem *    Top (void) const;
    void                   Empty (void);
    SCBoolean              IsEmpty (void) const { return (current == NULL); }
    class SCStackElem *    Root (void) const { return root; }
    SCNatural              CurrentDepth(void) const;
    SCNatural              MaxDepth(void) const;
    SCNatural              ReachedDepth(void) const;
    class SCStackElem *    Find(SCNatural hashKey, SCMem *state) const;
 
  private:
    SCNatural              maxDepth;
    SCNatural              currentDepth;
    SCNatural              maxReachedDepth;
    class SCStackElem *    root;
    class SCStackElem *    current;
    SCBoolean              cycleDetection;
    
    SCStackElemListTable * stackElemTable; // only for fast cycle detection
                                           // includes a list of stack elems
                                           // for each m-value
};


#endif  // __SCSTACK__
