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
| SCType        |  SCType.cc        | 9. Aug 1994   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     09.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCStream.h"
#include "SCType.h"

#include "SCSignalType.h"
#include "SCRequestType.h"
#include "SCProcessType.h"
#include "SCProcedureType.h"
#include "SCStateType.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCType.inl.h"
#endif

SCType::SCType (SCNatural pID,
                const char *pName,
                const SCObject* father) :
  SCObject(SC_TYPE, father),
  typeID(pID),
  typeName(pName)
{
}

SCType::~SCType (void)
{
}

SCStream& operator<< (SCStream& pStream, const SCType& pData)
{
  if (pData.GetName() != NULL)
  {
    pStream << "<" << pData.GetName() << ">(#";
    pStream << pData.GetID() << ")";
  }
  else
    pStream << "#" << pData.GetID();

  return (pStream);
}


const char * SCType::GetObjectName(SCObjectType objectType, SCNatural id)
{
  SCSignalType * signalType;
  SCRequestType * requestType;
  SCProcessType * processType;
  SCProcedureType * procedureType;
  SCStateType * stateType;
  
  static char name[80] = "";
  
  switch (objectType)
  {
    case SC_SIGNAL:
      signalType = SCSignalType::GetSignalTypeTable()->Find(id);
      if (signalType)
      {
        return signalType->GetName();
      }
      break;
    case SC_REQUEST:
      requestType = SCRequestType::GetRequestTypeTable()->Find(id);
      if (requestType)
      {
        return requestType->GetName();
      }
      break;
    case SC_PROCESS:
      processType = SCProcessType::GetProcessTypeTable()->Find(id);
      if (processType)
      {
        return processType->GetName();
      }
      break;
    case SC_PROCEDURE:
      procedureType = SCProcedureType::GetProcedureTypeTable()->Find(id);
      if (procedureType)
      {
        return procedureType->GetName();
      }
      break;
    case SC_STATE:
      stateType = SCStateType::GetStateTypeTable()->Find(id);
      if (stateType)
      {
        return stateType->GetName();
      }
      break;
    case SC_NONE:
      sprintf(name, "%ld", id);
      return name;
    default:
      break;
  }
  return NULL;
}

