/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (C) 1994-96 Marc Diefenbruch, Axel Hirche                                  |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCPosixMutex  |   SCPosixMutex.cc | 26. Jun 1996  |   SCL             |      |
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

#if (_SC_THREAD_TYPE == SCPosixThreads)

#include "SCPosixMutex.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCMutex::SCMutex(void)
{
  pthread_mutex_init (&mutex, NULL);
}


SCMutex::~SCMutex(void)
{
  pthread_mutex_destroy(&mutex);
}


void SCMutex::Lock (void) const
{
  pthread_mutex_lock((pthread_mutex_t *)&mutex);
}


void SCMutex::Unlock (void) const
{
  pthread_mutex_unlock((pthread_mutex_t *)&mutex);
}


SCBoolean SCMutex::Trylock (void) const
{
  return (pthread_mutex_trylock((pthread_mutex_t *)&mutex) == 0);
}


#endif  // _SC_THREAD_TYPE == SCPosixThreads
