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
|  SCPowerset   |   SCPowerset.cc   | 7. Jun 1994   |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     --.--.--    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include <string.h>

#include "SCPowerset.h"
#include "SCMem.h"
#include "SCStream.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
    #include "SCPowerset.inl.h"
#endif


static inline SCInteger max (SCInteger a, SCInteger b)
{
    return (a > b ? a : b);
}


static inline SCInteger min (SCInteger a, SCInteger b)
{
    return (a < b ? a : b);
}


SCPowerset::SCPowerset (const SCInteger low,
                        const SCInteger high,
                        const SCObject* father):
  SCDataType(father)
{
  SCNatural   size;

  lowBound = low;
  highBound = high;
  assert ((highBound > 0) && (lowBound > 0));  // This should always be true -
                                               // I think ;-)
  if (low <= high) {
      size = (highBound - lowBound + 1) / sizeof (unsigned char);
      set = new unsigned char [size];
      assert (set != NULL);
      memset (set, 0, size);
  }
  else
    set = NULL;
}


SCPowerset::SCPowerset (const SCPowerset& orig,
                        const SCObject* father):
  SCDataType(father)
{
  SCNatural   size;

  lowBound = orig.lowBound;
  highBound = orig.highBound;
  if (orig.set)
  {
    size = (highBound - lowBound + 1) / sizeof (unsigned char);
    set = new unsigned char [size];
    assert (set != NULL);
#if _SC_DMALLOC
    memcpy ((char *)set, (const char *)orig.set, size);
#else
    memcpy (set, orig.set, size);
#endif
  }
  else
    set = NULL;
}


SCPowerset::~SCPowerset (void)
{
  if (set)
    delete[] set;
}


SCPowerset& SCPowerset::operator= (const SCPowerset& orig)
{
  SCNatural   size;

  if (this != &orig)
  {
    if (set)
        delete[] set;
    lowBound = orig.lowBound;
    highBound = orig.highBound;
    if (orig.set) {
        size = (highBound - lowBound + 1) / sizeof (unsigned char);
        set = new unsigned char [size];
        #if _SC_DMALLOC
          memcpy ((char *)set, (const char *)orig.set, size);
        #else
          memcpy (set, orig.set, size);
        #endif
    }
    else
      set = NULL;
  }
  return (*this);
}


SCBoolean SCPowerset::In (const SCInteger item) const
{
  SCNatural   bytePos;
  SCNatural   pos;

  if (item >= lowBound && item <= highBound)
  {
      pos = item - lowBound;
      bytePos = pos / sizeof (unsigned char);
      return ((set[bytePos] & (1L<<(pos & 7))) != 0);
  }
  else
    return (false);
}


void SCPowerset::Incl (const SCInteger item)
{
  SCNatural   bytePos;
  SCNatural   pos;

  if (item >= lowBound && item <= highBound)
  {
    pos = item - lowBound;
    bytePos = pos / sizeof (unsigned char);
    set[bytePos] |= 1L << (pos & 7);
  }
}


void SCPowerset::Del (const SCInteger item)
{
  SCNatural   bytePos;
  SCNatural   pos;

  if (item >= lowBound && item <= highBound)
  {
    pos = item - lowBound;
    bytePos = pos / sizeof (unsigned char);
    set[bytePos] &= ~(1L << (pos & 7));
  }
}


SCBoolean SCPowerset::operator== (const SCPowerset& second) const
{
  SCInteger   size = (highBound - lowBound + 1) / sizeof (unsigned char);

  return (memcmp (set, second.set, size) == 0);
}


SCBoolean SCPowerset::operator< (const SCPowerset& superSet) const
{
  SCInteger   z_i;
  SCInteger   size = (highBound - lowBound + 1) / sizeof (unsigned char);

  for (z_i = 0; z_i < size; z_i++)
  {
    if ((set[z_i] & superSet.set[z_i]) != set[z_i])
      return (false);
  }
  return (*this != superSet);
}


SCBoolean SCPowerset::operator<= (const SCPowerset& superSet) const
{
    SCInteger   z_i;
    SCInteger   size = (highBound - lowBound + 1) / sizeof (unsigned char);

    for (z_i = 0; z_i < size; z_i++) {
        if ((set[z_i] & superSet.set[z_i]) != set[z_i])
            return (false);
    }
    return (true);
}


SCPowerset SCPowerset::operator& (const SCPowerset& second) const
{
  SCInteger   z_i;
  SCInteger   size;
  SCInteger   newLow,
              newHigh;

  if (this != &second)
  {
      if (lowBound == second.lowBound &&
          highBound == second.highBound)
      {
          SCPowerset newSet (*this);

          if (set) {
              size = (highBound - lowBound + 1) / sizeof (unsigned char);
              for (z_i = 0; z_i < size; z_i++) {
                  newSet.set[z_i] &= second.set[z_i];
              }
          }
          return (newSet);
      } else {
          newLow = min (lowBound, second.lowBound);
          newHigh = max (highBound, second.highBound);
          {
              SCPowerset  newSet (newLow, newHigh);
              for (z_i = newLow; z_i <= newHigh; z_i++)
                  if (In (z_i) && second.In (z_i))
                      newSet.Incl (z_i);
              return (newSet);
          }
      }
  } else
      return (*this);
}


SCPowerset SCPowerset::operator| (const SCPowerset& second) const
{
  SCInteger   z_i;
  SCInteger   size;
  SCInteger   newLow,
              newHigh;

  if (this != &second)
  {
      if (lowBound == second.lowBound &&
          highBound == second.highBound)
      {
          SCPowerset newSet (*this);

          size = (highBound - lowBound + 1) / sizeof (unsigned char);
          for (z_i = 0; z_i < size; z_i++) {
              newSet.set[z_i] &= second.set[z_i];
          }
          return (newSet);
      }
      else
      {
          newLow = min (lowBound, second.lowBound);
          newHigh = max (highBound, second.highBound);
          {
              SCPowerset  newSet (newLow, newHigh);
              for (z_i = newLow; z_i <= newHigh; z_i++)
                  if (In (z_i) || second.In (z_i))
                      newSet.Incl (z_i);
              return (newSet);
          }
      }
  }
  else
    return (*this);
}


SCStream &operator<< (SCStream &pStream, const SCPowerset &pData)
{
  SCInteger z_i;
  pStream << "power set <" << std::endl;
  pStream << pData.lowBound << ", " << std::endl;
  pStream << pData.highBound << "> = " << std::endl;
  pStream << "[";
  for (z_i = pData.lowBound; z_i < pData.highBound; z_i++)
    pStream << (pData.In (z_i) ? "1, " : "0, ");
  pStream << (pData.In (pData.highBound) ? "1]" : "0]");

  return (pStream);
}


SCStream &SCPowerset::Display (SCStream &pStream) const
{
  return (pStream << *this);
}


void SCPowerset::Size(SCSize *curSize) const
{
  curSize->size += ((highBound - lowBound + 1) / sizeof (unsigned char));
  curSize->historySize += (sizeof(SCInteger) +
                           sizeof(SCInteger));
}

void SCPowerset::HistorySize(SCSize *curSize) const
{
  curSize->historySize += ((highBound - lowBound + 1) / sizeof (unsigned char));
  curSize->historySize += (sizeof(SCInteger) +
                           sizeof(SCInteger));
}


SCBoolean SCPowerset::Save(SCMem& saveArea) const
{
  SCInteger tmp;
    
  saveArea.HistoryStore(&lowBound, sizeof(SCInteger));
  saveArea.HistoryStore(&highBound, sizeof(SCInteger));
  
  tmp = (highBound - lowBound + 1) / sizeof (unsigned char);
  assert(tmp > 0);
  
  saveArea.Store(set, tmp);
  
  return true;
}


SCBoolean SCPowerset::HistorySave(SCMem& saveArea) const
{
  SCInteger tmp;
    
  saveArea.HistoryStore(&lowBound, sizeof(SCInteger));
  saveArea.HistoryStore(&highBound, sizeof(SCInteger));
  
  tmp = (highBound - lowBound + 1) / sizeof (unsigned char);
  assert(tmp > 0);
  
  saveArea.HistoryStore(set, tmp);
  
  return true;
}


SCBoolean SCPowerset::Restore(SCMem& saveArea)
{
  SCInteger tmp;
    
  saveArea.HistoryRestore(&lowBound, sizeof(SCInteger));
  saveArea.HistoryRestore(&highBound, sizeof(SCInteger));
  
  tmp = (highBound - lowBound + 1) / sizeof (unsigned char);
  assert(tmp > 0);
  
  saveArea.Restore(set, tmp);
  
  return true;
}


SCBoolean SCPowerset::HistoryRestore(SCMem& saveArea)
{
  SCInteger tmp;
    
  saveArea.HistoryRestore(&lowBound, sizeof(SCInteger));
  saveArea.HistoryRestore(&highBound, sizeof(SCInteger));
  
  tmp = (highBound - lowBound + 1) / sizeof (unsigned char);
  assert(tmp > 0);
  
  saveArea.HistoryRestore(set, tmp);
  
  return true;
}
