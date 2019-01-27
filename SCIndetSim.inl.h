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
|  SCIndetSim   | SCIndetSim.inl.h  | 25. Apr 1996  |   SCL             |       |
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


#ifndef __SCINDETSIM_INL__

#define __SCINDETSIM_INL__


#include "SCRandUniform.h"
#include "SCScheduler.h"
#include "SCSignalType.h"
#include "SCProcessType.h"
#include "SCStateType.h"
#include "SCTransition.h"


_SCINLINE_ SCNatural SCIndetSim::NonDetChoice (const SCNatural range)
{
  return ((SCNatural)((*randomizer)() * (SCReal)range));
}


_SCINLINE_ SCNatural SCIndetSim::ChooseOneTrue (const SCNatural choiceSize,
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


_SCINLINE_ void SCIndetSim::StartMessage (void)
{
  SCScheduler::outputStream << *this;
}


_SCINLINE_ void SCIndetSim::EndMessage (void)
{
  SCScheduler::outputStream << *this;
}


#endif  // __SCINDETSIM_INL__
