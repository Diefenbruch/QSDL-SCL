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
| SCIndetVal    | SCStack.cc        | 30. Apr 1996  |   SCL             |      |
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

#include "SCStack.h"
#include "SCStackElem.h"
#include "SCMem.h"

#include "SCDebug.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCStack::SCStack (SCNatural initMaxDepth,
                  SCBoolean doCycleDetection) :
  maxDepth(initMaxDepth),
  currentDepth(0),
  maxReachedDepth(0),
  root(NULL),
  current(NULL),
  cycleDetection(doCycleDetection)
{
  if (cycleDetection)
  {
    stackElemTable = new SCStackElemListTable(maxDepth != kSCMaxDepth ? maxDepth : 10000,
                                              true,   // lists can be destroyed
                                              false); // NO collision detection!
    assert(stackElemTable);
  }
  else
  {
    stackElemTable = NULL;
  }
}


SCStack::~SCStack (void)
{
  Empty();                                  // remove all elements
    
  if (stackElemTable)
    delete stackElemTable;
}


SCBoolean SCStack::Push (SCStackElem *newElem)
{
  assert(newElem);                          // Bail out, if newElem is a NULL-pointer.

  if (current)                              // Stack not empty.
  {
    current->next = newElem;                // Append new element.
    current->next->prev = current;          // Link new element backwards.
    current = current->next;                // Set current to new last element.
  }
  else                                      // Stack is empty.
  {
    current = newElem;                      // Insert new element.
    current->prev = NULL;                   // No predecessor.
    current->next = NULL;                   // No successor.
    root = current;                         // Set root.
  }

#if _SC_STACKTRACE_DEBUG
  newElem->depth = currentDepth;
#endif

  currentDepth++;                           // Update counter.

  if (currentDepth > maxReachedDepth)
  {
    maxReachedDepth = currentDepth;
  }
  
  if (cycleDetection)
  {
    SCStackElemCons * cons;
    SCStackElemList * list = stackElemTable->Find (newElem->GetHashKey());

    if (!list)                               // first timer for this timer type?
    {
      list = new SCStackElemList(false);     // allocate list for this m-value
      assert(list);

      cons = list->InsertAfter (newElem);
      stackElemTable->Insert (newElem->GetHashKey(), list);
                                             // insert list into hash table
      assert(stackElemTable->Find (newElem->GetHashKey()));
    }
    else                                     // list is present, so other elems
    {                                        // for this m-value have already
                                             // been inserted
      cons = list->InsertAfter (newElem);
    }
    newElem->container = cons;
  }
  
  return true;                               // Whaddayaexpectman ;-)
}


SCStackElem * SCStack::Pop (void)
{
  SCStackElem *temp = current;               // Safeguard if only one element in stack.

  assert (current);

  current = current->prev;                   // Move current to previous element.
  if (current)
    current->next = NULL;                    // No successor.
  else                                       // last element poped
    root = NULL;
    
  currentDepth--;                            // Update depth counter.

  if (cycleDetection)
  {
    SCStackElemList * list = stackElemTable->Find (temp->GetHashKey());

    if (!list)
    {
      std::cerr << "Can't find key " << temp->GetHashKey() << " stack elem table!" << std::endl;

      assert(list);

      return NULL;
    }
    assert(temp->container);

    list->Remove(temp->container);
  }

  return temp;                             // If only one element is in the stack,
                                           // current is now NULL and cannot be dreferenced.
}


SCStackElem * SCStack::Top (void) const
{
  return current;
}


void SCStack::Empty (void)
{
  while (Top())
  {
    delete Pop();
  }
}


SCNatural SCStack::CurrentDepth(void) const
{
  return currentDepth;
}


SCNatural SCStack::MaxDepth(void) const
{
  return maxDepth;
}


SCNatural SCStack::ReachedDepth(void) const
{
  return maxReachedDepth;
}


SCStackElem * SCStack::Find(SCNatural hashKey, SCMem *state) const
{
  SCStackElemList *list;
  SCStackElem *elem;
  
  assert (stackElemTable); // this code is ONLY for cycle detection!
  
  list = stackElemTable->Find(hashKey);
  
  if (!list)          // no stack elem with this m-value?
  {
    return NULL;
  }
  else
  {
    SCStackElemIter iter(*list);
    
    for (elem = iter++;
         elem;
         elem = iter++)
    {
      if (elem->primaryHashKey == hashKey)
      {
        if (*state == *elem->systemState)  // even states with same hash key
        {                                  // can be different!
          return elem;
        }
      }
    }
    return NULL;  
  }
}
