/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+----------------------+---------------+----------------+-------+
|   Module      |   File               |   Created     |   Project      |       |
+---------------+----------------------+---------------+----------------+-------+
|  SCScheduler  |  SCSystemScheduler.h | 12. Jul 1994  |   SCL          |       |
+---------------+----------------------+---------------+----------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     05.07.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCSYSTEMSCHEDULER__

#define __SCSYSTEMSCHEDULER__

/* This should automagically include the correct header */

#include "SCThreadTypes.h"

class SCSystemScheduler: public SCSystemThread
{
  public:
       SCSystemScheduler (void);
      ~SCSystemScheduler (void);

	void Suspend (void);
	void Resume (void);
};


#endif // __SCSYSTEMSCHEDULER__
