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
| SCRequestType | SCRequestType.cc  | 2. Apr 1995   |   SCL             |      |
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

#include <string.h>
#include "SCRequestType.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCRequestTypeTable *SCRequestType::requestTypeTable = NULL;


SCRequestType::SCRequestType (SCNatural rtID,
                              const char *pName,
                              const SCObject* father) :
  SCType (rtID, pName, father)
{
  assert (requestTypeTable);
  requestTypeTable->Insert (rtID, this);
}


SCRequestType::~SCRequestType (void)
{
  assert (requestTypeTable);
  requestTypeTable->Remove (GetID());
}


SCStream& operator<< (SCStream& pStream, const SCRequestType& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCBoolean SCRequestType::Initialize (SCNatural tableSize)
{
  // allocate table for all request types

  requestTypeTable = new SCRequestTypeTable (tableSize, false);
  assert (requestTypeTable);

  return true;
}


SCBoolean SCRequestType::Finish (void) // static!
{
  // delete table of request types

  if (requestTypeTable)
  {
    delete requestTypeTable;
    requestTypeTable = NULL;
  }

  return true;
}


SCBoolean SCRequestType::operator==(const SCRequestType &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCRequestType::operator!=(const SCRequestType &second) const
{
  return (GetID() != second.GetID());
}


SCRequestType * SCRequestType::GetRequestTypeFromName(const char * name)
{
  SCNatural numOfElems = requestTypeTable->GetTableSize();
  SCRequestType * requestType;
  SCNatural i;
  
  if (name == NULL)
    return NULL;
    
  for (i = 0; i < numOfElems; i++)
  {
    requestType = (*requestTypeTable)[i];
    if (requestType)
    {
      if (!strcmp(requestType->GetName(), name))
      {
        return requestType;
      }
    }
  }
  
  return NULL;
}

