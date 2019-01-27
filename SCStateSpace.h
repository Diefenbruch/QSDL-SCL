/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch                                                |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
|  SCStateSpace |  SCStateSpace.h   | 19. Nov 1997  |   SCL             |       |
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


#ifndef __SCSTATESPACE__

#define __SCSTATESPACE__

#include "SCBasicTypes.h"
#include "SCObject.h"


typedef enum
{
  scBitStateSpace,
  scFullStateSpace
} SCStateSpaceType;


class SCStateSpace: public SCObject
{
  public:
                     SCStateSpace (const SCStateSpaceType pType,
                                   const SCObject * father = NULL);
    virtual         ~SCStateSpace (void);

    virtual          SCBoolean IsStateAnalyzed(class SCMem *state) = 0;
    virtual          SCNatural GetNumOfAnalyzedStates(void) const = 0;
    virtual          SCNatural GetStateSpaceSize(void) const = 0;
    virtual          SCInteger GetCurrentKey(void) const = 0;
    SCStateSpaceType GetType(void) { return type; }
    
  private:
    SCStateSpaceType type;    
};


#endif  // __SCSTATESPACE__
