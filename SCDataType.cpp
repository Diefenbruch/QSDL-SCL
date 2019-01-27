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
|   SCDataType  |   SCDataType.cc   | 23. Mai 1994  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     23.05.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCDataType.h"
#include "SCStream.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCDataType::SCDataType (const SCDataType &orig,
                        const SCObject* father):
  SCObject(SC_DATATYPE, father)
{
  (void)orig;
}


SCDataType::SCDataType (const SCObject* father):
  SCObject(SC_DATATYPE, father)
{
}


SCDataType::~SCDataType (void)
{
}


SCStream& operator<< (SCStream& pStream, const SCDataType& pData)
{
  return pData.Display(pStream); // virtuelle Funktion aufrufen
}
