/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Axel Hirche                                   |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|   SCPosixMutex|   SCPosixMutex.h  | 26. Jun 1996  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     26.06.96    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCMUTEX__

#define __SCMUTEX__

#include "SCEnvironment.h"
#include "SCBasicTypes.h"

extern "C" {
#include <pthread.h>
}

class SCMutex
{
  public:
    SCMutex(void);
   ~SCMutex(void);
    
    void      Lock (void) const;
    void      Unlock (void) const;
    SCBoolean Trylock (void) const;
      
  private:
    mutable pthread_mutex_t mutex;
};

#endif // __SCMUTEX__
