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
|SCTimeEventList| SCTimeEventList.cc| 16. Aug 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     16.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/


#include "SCTimeEventList.h"
#include "SCTimeEvent.h"
#include "SCRunnable.h"
#include "SCList.h"
#include "SCMem.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_PROFILING
extern int save_ineff;
extern int restore_ineff;
extern int size_ineff;
#endif

SCTimeEventSaveList::SCTimeEventSaveList (SCBoolean isFair,
                                          SCNatural pSize,
                                          SCObject *pParent) :
  SCList<SCTimeEvent>(true, pParent),
  fair(isFair)
{
  // lookupTable = new SCTimeEventTable(pSize, false, this);
  lookupTable = new SCTimeEventTable(pSize,
                                     false,  // deleteElems
                                     true,   // resolveColl
                                     true,   // resize if full
                                     this);  // parent
  assert(lookupTable);
}


SCTimeEventSaveList::~SCTimeEventSaveList (void)
{
  if (lookupTable)
    delete lookupTable;
}


SCTimeEventCons * SCTimeEventSaveList::Insert (SCTimeEvent *timeEvent)
{
  SCTimeEventCons *prev;
  SCTimeEventCons *position;
  SCTimeEventCons *newCons;

  assert (timeEvent != NULL);
  assert(lookupTable);

  // In LookUp-Tablle einfuegen:  
  lookupTable->Insert(timeEvent->GetRunnableID(),
                      timeEvent);
                      
  // If list is empty, insert at head
  if (IsEmpty())
  {
    newCons = InsertBefore (timeEvent, (SCTimeEventCons *)NULL);
    assert(newCons);

    timeEvent->SetContainer(newCons);
    
    return newCons;
  }

  // Try to insert before (if there is anything scheduled later)
  for (prev = NULL, position = Head();
       position;
       prev = position, position = position->Next())
  {
    if (fair)
    {
      // This is VERY critical!!!
      if ((*position)()->GetTime() > timeEvent->GetTime())
      {
        newCons = InsertBefore (timeEvent, position);
        assert(newCons);

        timeEvent->SetContainer(newCons);
    
        return newCons;
      }
    }
    else // !fair (maximum progress)
    {
      if ((*position)()->GetTime() >= timeEvent->GetTime())
      {
        newCons = InsertBefore (timeEvent, position);
        assert(newCons);

        timeEvent->SetContainer(newCons);
    
        return newCons;
      }
    }
  }

  // Got to insert at the end (currently pointed at by 'prev')
  newCons = InsertAfter (timeEvent, prev);
  assert(newCons);

  timeEvent->SetContainer(newCons);

  return newCons;
}


SCTimeEvent * SCTimeEventSaveList::Remove (SCTimeEvent *timeEvent)
{
  SCTimeEvent *retValue = NULL;

  if (timeEvent && timeEvent->GetContainer())
  {
    retValue = SCList<SCTimeEvent>::Remove (timeEvent->GetContainer());
  }
  else
  {
    retValue = SCList<SCTimeEvent>::Remove (timeEvent);
  }
  
  // Aus LookUp-Tablle entfernen:  
  assert(lookupTable);
  lookupTable->Remove(retValue->GetRunnableID());

  return retValue;
}


SCTimeEvent * SCTimeEventSaveList::Remove (SCTimeEventCons *timeEvent)
{
  SCTimeEvent *retValue;
  
  retValue = SCList<SCTimeEvent>::Remove (timeEvent);
  assert(retValue);

  // Aus LookUp-Tablle entfernen:  
  assert(lookupTable);
  lookupTable->Remove(retValue->GetRunnableID());

  return retValue;
}


void SCTimeEventSaveList::RemoveAllElements(void)
{
  SCTimeEventIter iter(*this);
  SCTimeEvent *   timeEvent;
  
  assert(lookupTable);

  for (timeEvent = iter++;
       timeEvent;
       timeEvent = iter++)
  {
    // Aus LookUp-Tablle entfernen:  
    lookupTable->Remove(timeEvent->GetRunnableID());
  }

  SCList<SCTimeEvent>::RemoveAllElements();
}


SCBoolean SCTimeEventSaveList::Save (SCMem &saveArea) const
{
  SCTimeEvent     *timeEvent;
  SCTimeEventCons *timeEventCons;

  saveArea.HistoryStore(&numOfElems, sizeof(SCNatural));

  for (timeEventCons = head;
       timeEventCons;
       timeEventCons = timeEventCons->Next())
  {
    timeEvent = (*timeEventCons)();
    assert(timeEvent);
    timeEvent->Save(saveArea);
  }

  return true;
}


SCBoolean SCTimeEventSaveList::Restore (SCMem &saveArea)
{
  SCTimeEvent *     timeEvent = NULL;
  SCTimeEventCons * timeEventCons;
  SCNatural         i, number;
  
  ////////////////////
  // 1. Empty list:
  ////////////////////

  RemoveAllElements();

  ////////////////////////////
  // 2. Read number of elems:
  ////////////////////////////

  saveArea.HistoryRestore(&number, sizeof(SCNatural)); // don't store in numOfElems

  ////////////////////
  // 3. Refill list:
  ////////////////////

  for (i = 0; i < number; i++)
  {
    timeEvent = new SCTimeEvent(saveArea, this);
    assert(timeEvent);
    timeEventCons = InsertAfter(timeEvent); // inkrementiert numOfElems!

    timeEvent->SetContainer(timeEventCons);

    // In LookUp-Tabelle einfuegen:  
    lookupTable->Insert(timeEvent->GetRunnableID(),
                        timeEvent);
  }

  return true;
}


void SCTimeEventSaveList::Size(SCSize *curSize) const
{
  SCTimeEvent     *timeEvent = NULL;
  SCTimeEventCons *timeEventCons;

  curSize->historySize += sizeof(SCNatural); // for numOfElems

  for (timeEventCons = head;
       timeEventCons;
       timeEventCons = timeEventCons->Next())
  {
    timeEvent = (*timeEventCons)();
    assert(timeEvent);
    timeEvent->Size(curSize);
  }
}


SCTimeEvent * SCTimeEventSaveList::LookupRunnable(SCRunnable *runnable) const
{
  assert(runnable);
  assert(lookupTable);
  
  return lookupTable->Find(runnable->GetID());
}

