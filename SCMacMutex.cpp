/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-96 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCMacMutex  |   SCMacMutex.cc   | 26. Jun 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     26.06.96    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCMutexTypes.h"

#if (_SC_THREAD_TYPE == SCMacintoshThreads)

#include "SCMacMutex.h"


SCMutex::SCMutex(void)
{
}


SCMutex::~SCMutex(void)
{
}


void SCMutex::Lock (void) const
{
}


void SCMutex::Unlock (void) const
{
}


SCBoolean SCMutex::Trylock (void) const
{
  return true;
}

#endif  // _SC_THREAD_TYPE == SCMacintoshThreads
