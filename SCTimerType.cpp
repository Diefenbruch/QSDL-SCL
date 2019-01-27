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
|  SCTimerType  |  SCTimerType.cc   | 2. Apr 1995   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     02.04.95    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCTimerType.h"
#include "SCTimer.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCTimerType::SCTimerType (SCNatural timID,
                          const char *pName,
                          const SCObject* father) :
  SCSignalType (timID, pName, father)
{
  timerListTable = new SCTimerListTable (kSCDefaultTableSize, true);
  assert(timerListTable);
}


SCTimerType::~SCTimerType (void)
{
  delete timerListTable;
}


SCStream& operator<< (SCStream& pStream, const SCTimerType& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


