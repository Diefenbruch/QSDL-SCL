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


#ifndef __SCINDETSTACKTRACE_INL__

#define __SCINDETSTACKTRACE_INL__

#include "SCStream.h"
#include "SCIndetVal.h"
#include "SCStack.h"
#include "SCDebug.h"

_SCINLINE_ SCNatural SCIndetStackTrace::NonDetChoice (const SCNatural range)
{
  (void)range;

  return 0;
}


_SCINLINE_ void SCIndetStackTrace::StartMessage (void)
{
  std::cout << std::endl << "Dumping MSC for ";
  switch (theIndetVal->GetLastError())
  {
    case scErrorDeadlock:
      std::cout << "deadlock";
      break;
    case scErrorSignalDrop:
      std::cout << "implicit signal consumption";
      break;
    case scErrorSignalLose:
      std::cout << "queue overflow";
      break;
    case scErrorProcessCreationFailed:
      std::cout << "failed process creation";
      break;
    case scErrorNoSignalReceiver:
      std::cout << "no signal receiver";
      break;
    case scErrorAssertionFailed:
      std::cout << "failed assertion";
      break;
    case scErrorCycleDetected:
      std::cout << "detected cycle";
      break;
    default:
      std::cout << "unknown error";
      break;
  }
  if (theIndetVal->isInCycleDetection)
  {
    std::cout << " (depth = " << theIndetVal->GetFirstStack()->CurrentDepth() +
                            theIndetVal->GetSecondStack()->CurrentDepth() << ") ...";
  }
  else
  {
    std::cout << " (depth = " << theIndetVal->GetStack()->CurrentDepth() << ") ...";
  }
  std::cout.flush();
}


_SCINLINE_ void SCIndetStackTrace::EndMessage (void)
{
  std::cout << "done." << std::endl << std::endl;
}


_SCINLINE_ SCNatural SCIndetStackTrace::ChooseOneTrue (const SCNatural choiceSize,
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


#endif  // __SCINDETSTACKTRACE_INL__
