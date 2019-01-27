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
| SCIndetVal    | SCIndetVal.inl.h  | 25. Apr 1996  |   SCL             |       |
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

#ifndef __SCINDETVAL_INL__

#define __SCINDETVAL_INL__


#include "SCStack.h"
#include "SCScheduler.h"

_SCINLINE_ SCNatural SCIndetVal::NonDetChoice (const SCNatural range)
{
  (void)range;

  return 0;
}


_SCINLINE_ SCNatural SCIndetVal::ChooseOneTrue (const SCNatural choiceSize,
                                                const SCNatural numOfTrues,
                                                const SCNatural singleTrue,
                                                const SCBoolean *choiceSet)
{
  SCNatural choice;
  SCNatural z_i;

  switch (numOfTrues)
  {
    case 0:
      choice = kSCDecisionNone;
      break;
    case 1:
      choice = singleTrue + kSCDecisionBase;
      break;
    default:
      choice = NonDetChoice (numOfTrues);
      for (z_i = 0; choice != 0 && z_i < choiceSize; z_i++)
      {
        if (choiceSet[z_i])
          choice--;
      }
      choice = z_i + kSCDecisionBase;
      break;
  }
  return (choice);
}


// this method is called by SCAutomaton::Create() and SCAutomaton::Call():

_SCINLINE_ SCBoolean SCIndetVal::Create (SCProcessID offspring)
{
  if (createdInstances == NULL)
  {
     // maxNumOfCreatedInstances should be calculated here FIXME!
     assert(maxNumOfCreatedInstances > 0);
     createdInstances = new SCProcessID[maxNumOfCreatedInstances];
     assert(createdInstances);
     numOfCreatedInstances = 0;
  }
  
  createdInstances[numOfCreatedInstances++] = offspring;
  
  return true;
}


_SCINLINE_ SCNatural SCIndetVal::CurrentDepth (void) const
{
  return currentStack->CurrentDepth();
}


_SCINLINE_ void SCIndetVal::StartMessage (void)
{
  SCScheduler::outputStream << *this;
}


_SCINLINE_ void SCIndetVal::EndMessage (void)
{
  SCScheduler::outputStream << *this;
}


_SCINLINE_ class SCStack * SCIndetVal::GetStack (void) const
{
  return currentStack;
}


_SCINLINE_ class SCStack * SCIndetVal::GetFirstStack (void) const
{
  return theFirstStack;
}


_SCINLINE_ class SCStack * SCIndetVal::GetSecondStack (void) const
{
  return theSecondStack;
}


_SCINLINE_ class SCStateSpace * SCIndetVal::GetCurrentStateSpace (void) const
{
  return currentStateSpace;
}


_SCINLINE_ class SCStateSpace * SCIndetVal::GetStateSpace (void) const
{
  return theStateSpace;
}


_SCINLINE_ class SCStateSpace * SCIndetVal::GetCycleStateSpace (void) const
{
  return theCycleStateSpace;
}


_SCINLINE_ SCErrorCode SCIndetVal::GetLastError (void) const
{
  return lastError;
}


_SCINLINE_ SCNatural SCIndetVal::GetLastSuccessor (void) const
{
  return lastSuccessor;
}


_SCINLINE_ class SCStackElem * SCIndetVal::GetCycleClosingElem (void) const
{
  return cycleClosingElem;
}




#endif  // __SCINDETVAL_INL__
