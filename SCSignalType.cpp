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
| SCSignalType  |  SCSignalType.cc  | 2. Apr 1995   |   SCL             |      |
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
#include "SCSignalType.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSignalTypeTable *SCSignalType::signalTypeTable = NULL;


SCSignalType::SCSignalType (SCNatural sigID,
                            const char *pName,
                            const SCObject* father) :
  SCType (sigID, pName, father)
{
  assert(signalTypeTable);
  signalTypeTable->Insert (sigID, this);
}


SCSignalType::~SCSignalType (void)
{
  assert(signalTypeTable);
  signalTypeTable->Remove (GetID());
}


SCStream& operator<< (SCStream& pStream, const SCSignalType& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}


SCBoolean SCSignalType::Initialize (SCNatural tableSize)
{
  // allocate table for all signal types

  signalTypeTable = new SCSignalTypeTable (tableSize, false);
  assert (signalTypeTable);

  return true;
}


SCBoolean SCSignalType::Finish (void) // static!
{
  // delete table of signal types

  if (signalTypeTable)
  {
    delete signalTypeTable;
    signalTypeTable = NULL;
  }

  return true;
}


SCBoolean SCSignalType::operator==(const SCSignalType &second) const
{
  return (GetID() == second.GetID());
}


SCBoolean SCSignalType::operator!=(const SCSignalType &second) const
{
  return (GetID() != second.GetID());
}


SCSignalType * SCSignalType::GetSignalTypeFromName(const char * name)
{
  SCNatural numOfElems = signalTypeTable->GetTableSize();
  SCSignalType * signalType;
  SCNatural i;
  
  if (name == NULL)
    return NULL;
    
  for (i = 0; i < numOfElems; i++)
  {
    signalType = (*signalTypeTable)[i];
    if (signalType)
    {
      if (!strcmp(signalType->GetName(), name))
      {
        return signalType;
      }
    }
  }
  
  return NULL;
}

