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
|   SCRandom    |   SCRandom.cc     | 23. Mai 1994  |   SCL             |      |
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

#include <math.h>

#include "SCStream.h"
#include "SCRandom.h"
#include "SCMem.h"

#if _SC_USEDRAND48
  #include <stdlib.h>
  #include "SCRand48Seed.h"
#else
  #include "SCRandMGenSeed.h"
  #include "SCRandLGenSeed.h"
#endif

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCInteger SCRandom::nextID = 0;

#if _SC_USEDRAND48

enum
{
  kSCSeedSize = 8192L  // Anzahl Zufallsstroeme
};

#else

enum
{
  kSCSeedSize = 3072L  // Anzahl Zufallsstroeme
};

#endif // _SC_USEDRAND48


SCRandom::SCRandom (const SCObject* pParent):
  SCDataType(pParent),
  id(nextID++)
{
  SCNatural index = ID2SeedIndex (nextID % kSCSeedSize);

#if _SC_USEDRAND48
  curSeed48[0] = seed48Init[0][index];
  curSeed48[1] = seed48Init[1][index];
  curSeed48[2] = seed48Init[2][index];
#else
  SCNatural z_i;

  mSeed = mSeedInit[index];
  lSeed = lSeedInit[index];
  for (z_i = 0;
       z_i < (sizeof (series) / sizeof (SCReal));
       z_i++)
  {
    series[z_i] = MGen();
  }
#endif
}


/* RNG-ID in Index in das Seed-Array konvertieren.
 * Erzeugt folgende Zuordnung:
 *  id   | seedIndex
 * ------+-------------------------------------
 *    0  | 0 * kSCSeedSize / 1
 *    1  | 1 * kSCSeedSize / 2
 *    2  | 1 * kSCSeedSize / 4
 *    3  | 3 * kSCSeedSize / 4
 *    4  | 1 * kSCSeedSize / 8
 *    5  | 3 * kSCSeedSize / 8
 *    6  | 5 * kSCSeedSize / 8
 *    7  | 7 * kSCSeedSize / 8
 *  ...
 */

SCNatural SCRandom::ID2SeedIndex (SCInteger iD) const
{
  int       exp;
  double    base;
  SCNatural baseN;

  if (iD)
  {
    frexp ((double)iD, &exp);
    base = ldexp (1.0, exp);
    baseN = (SCNatural)base;
    return ((iD << 1) - baseN + 1) * kSCSeedSize / baseN;
  }
  else
    return 0;
}

#if _SC_USEDRAND48

SCReal SCRandom::Uniform (void)
{
  return erand48(curSeed48);
}

#else

/* Implementation of our own random number generator */

SCReal SCRandom::MGen (void)
{
  // A multiplicative generator, courtesy I. Mitrani 1992,
  // private correspondence
  // Y[z_i+1] = Y[z_i] * 5^5 mod 2^26
  // period is 2^24, initial seed must be odd

  const SCInteger two2the26th = 67108864; // 2**26

  mSeed = (mSeed * 25) % two2the26th;
  mSeed = (mSeed * 25) % two2the26th;
  mSeed = (mSeed * 5) % two2the26th;

  return ((SCReal)mSeed / (SCReal)two2the26th);
}

SCReal SCRandom::Uniform (void) 
{

  // A linear congruential generator based on the algorithm from
  // "Algorithms", R. Sedgewick, Addison-Wesley, Reading MA, 1983.
  // pp. 36-38.
  const SCInteger m = 100000000;
  const SCInteger b = 31415821;
  const SCInteger m1 = 10000;

  // Do the multiplication in pieces to avoid overflow
  SCInteger p0 = lSeed % m1,
            p1 = lSeed / m1,
            q0 = b % m1,
            q1 = b / m1;

  lSeed = (((((p0*q1+p1*q0)%m1)*m1+p0*q0)%m) + 1) % m;

  // The results of the LC generator are shuffled with
  // the multiplicative generator as suggested by
  // Maclaren and Marsaglia (See Knuth Vol2, Seminumerical Algorithms)

  SCInteger choose = lSeed % (sizeof(series)/sizeof(SCReal));

  SCReal result = series[choose];
  series[choose] = MGen();

  return (result);
}
#endif


SCBoolean SCRandom::Save (SCMem& saveArea) const
{
#if _SC_USEDRAND48
  saveArea.HistoryStore (&curSeed48, 3 * sizeof (unsigned short));
#else
  saveArea.HistoryStore(&mSeed, sizeof(SCInteger));
  saveArea.HistoryStore(&lSeed, sizeof(SCInteger));
#endif

  return true;
}


SCBoolean SCRandom::HistorySave (SCMem& saveArea) const
{
  return Save(saveArea); // random vars are always
                         // stored in hist area!
}


SCBoolean SCRandom::Restore (SCMem &saveArea)
{
#if _SC_USEDRAND48
  saveArea.HistoryRestore (&curSeed48, 3 * sizeof (unsigned short));
  seed48(curSeed48);
#else
  saveArea.HistoryRestore(&mSeed, sizeof(SCInteger));
  saveArea.HistoryRestore(&lSeed, sizeof(SCInteger));
#endif

  return true;
}


SCBoolean SCRandom::HistoryRestore (SCMem& saveArea)
{
  return Restore(saveArea); // random vars are always
                            // stored in hist area!
}


void SCRandom::Size(SCSize *curSize) const
{
#if _SC_USEDRAND48
  curSize->historySize += (3 * sizeof (unsigned short));
#else
  curSize->historySize += (sizeof(SCInteger) +  // Member mSeed
                           sizeof(SCInteger));  // Member lSeed
#endif
}


SCBoolean SCRandom::StaticSave(SCMem &saveArea)
{
  saveArea.HistoryStore(&nextID, sizeof(SCRandID));

  return true;
}


SCBoolean SCRandom::StaticRestore(SCMem &saveArea)
{
  saveArea.HistoryRestore(&nextID, sizeof(SCRandID));

  return true;
}

void SCRandom::StaticSize(SCSize *curSize)
{
  curSize->historySize += sizeof(SCRandID);         // Member nextID
}


SCStream &operator<< (SCStream &pStream, const SCRandom &pData)
{
  (void)pStream;
  (void)pData;

  pStream << "(" << pData.id << ", ";
  pData.Display (pStream);
  pStream << ")";

  return (pStream);
}


SCStream& SCRandom::Display (SCStream& pStream) const
{
  return (pStream << *this);
}


SCDataType* SCRandom::Clone (void) const
{
  assert(false);

  // SCRandom is a pure virtual base class. The only way to implement a clone
  // method is to declare clone in SCRandom as pure virtual.
  // Implementation takes place in the leaf classes of the inheritance tree.

  return NULL;
}


SCBoolean SCRandom::Equal (const SCDataType &second) const
{
  SCRandom *tmp = (SCRandom*) &second;
  SCBoolean test = false;
  int i;

#if _SC_USEDRAND48

  for (i = 0; i < 3; i++)
  {
    test = test && (curSeed48[i] == tmp->curSeed48[i]);
  }
  return test;

#else

  for (i = 0; i < (sizeof (series) / sizeof (SCReal)); i++)
  {
    test = test && (series[i] == tmp->series[i]);
  }
  return mSeed == tmp->mSeed &&
         lSeed == tmp->lSeed &&
         test;
    
#endif
}

SCBoolean SCRandom::operator==(const SCRandom &second) const
{
  return Equal(second);
}


SCBoolean SCRandom::operator!=(const SCRandom &second) const
{
  return !Equal(second);
}
