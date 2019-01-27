/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 University of Essen, Germany                                   |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
|   SCObject    |   SCObject.cc     | 24. Feb 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     05.03.96   Neu angelegt                                            |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCObject.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCObject::SCObject(const SCObjectType newType,
                   const SCObject *father):
  parent((SCObject *)father),
  type(newType)
{
}


SCObject::~SCObject(void)
{
}


SCObject* SCObject::GetParent(void) const
{
  return parent;
}


void SCObject::SetParent(SCObject *father)
{
  parent = father;
}


SCBoolean SCObject::Save(SCMem& saveArea) const
{
  (void)saveArea;

  assert (false);

  return true;
}


SCBoolean SCObject::Restore(SCMem& saveArea)
{
  (void)saveArea;

  assert (false);

  return true;
}


void SCObject::Size(SCSize *size) const
{
  (void)size;

  assert(false);
}


SCStream& operator<< (SCStream& pStream, const SCObject& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}
