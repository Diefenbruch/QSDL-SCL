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
| SCCharstring  | SCCharstring.cc   | 23. Mai 1994  |   SCL             |      |
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

#include <string.h>

#include "SCCharstring.h"
#include "SCMem.h"
#include "SCStream.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

#if _SC_NOINLINES
  #include "SCCharstring.inl.h"
#endif


SCCharstring::SCCharstring (const SCNatural l,
                            const SCObject* father):
  SCDataType(father),
  maxLength(l),
  curLength(0)
{
  if (l > 0)
  {
    s = new SCCharacter[l];
    assert(s != NULL);
  }
  else
   s = (SCCharacter *)NULL;
}


SCCharstring::SCCharstring (const SCCharacter c,
                            const SCObject* father):
  SCDataType(father),
  maxLength(1),
  curLength(1)
{
  s = new SCCharacter[curLength + 1];
  assert(s != NULL);

  s[0] = c;
  s[1] = '\0';
}


SCCharstring::SCCharstring (const SCCharacter *const string,
                            const SCObject* father):
  SCDataType(father)
{
  if (string)
  {
    curLength = maxLength = strlen (string);

    if (curLength > 0)
    {
      s = new SCCharacter[curLength + 1];
      assert(s != NULL);

      strcpy (s, string);
    }
    else
      s = (SCCharacter *)NULL;
  }
  else
  {
    curLength = maxLength = 0;
    s = (SCCharacter *)NULL;
  }
}


SCCharstring::SCCharstring (const SCCharstring &orig,
                            const SCObject* father):
  SCDataType(father)
{
  if (orig.s)
  {
    maxLength = orig.maxLength;
    curLength = orig.curLength;

    s = new SCCharacter[curLength + 1];
    assert (s != NULL);

    strcpy (s, orig.s);
  }
  else
  {
    maxLength = curLength = 0;
    s = (SCCharacter *)NULL;
  }
}


SCCharstring::~SCCharstring (void)
{
  if (s)
    delete[] s;
}


SCCharstring& SCCharstring::operator= (const SCCharstring &orig)
{
  if (this != &orig)
  {
    delete s;

    curLength = orig.curLength;
    maxLength = orig.maxLength;

    if (curLength > 0)
    {
      s = new SCCharacter[curLength + 1];
      assert (s != NULL);

      strcpy (s, orig.s);
    }
    else
    {
      s = (SCCharacter *)NULL;
    }
  }
  return *this;
}


SCBoolean SCCharstring::operator== (const SCCharstring &second) const
{
  if (this == &second)
    return (true);

  else if (curLength == second.curLength)
  {
    if (s && second.s)
      return (strncmp (s, second.s, curLength) == 0);
    else
      return (false);
  }
  else
    return (false);
}


SCCharstring SCCharstring::operator|| (const SCCharstring &second)
{
  SCCharstring newS;

  newS.curLength = curLength + second.curLength;

  newS.s = new SCCharacter[curLength + second.curLength];
  assert (newS.s != NULL);

  strncpy (newS.s, s, curLength);
  strncat (newS.s + curLength, second.s, second.curLength);

  return (newS);
}


SCCharstring SCCharstring::SubString (const SCNatural start,
                                      const SCNatural length) const
{
  SCCharstring newS;

  newS.curLength = length;
  newS.s = new SCCharacter[length];
  assert (newS.s != NULL);

  strncpy (newS.s, s + start, length);

  return (newS);
}


SCStream &operator<< (SCStream &pStream, const SCCharstring &pData)
{
  pStream << "char string" << std::endl;
  pStream << "Max. length " << pData.maxLength << std::endl;
  pStream << "Cur. length " << pData.curLength << std::endl;
  pStream << "String = '" << pData.s << '\'' << std::endl;

  return (pStream);
}


SCStream &SCCharstring::Display (SCStream &pStream) const
{
  return (pStream << *this);
}


SCBoolean SCCharstring::Save(SCMem& saveArea) const
{
  saveArea.HistoryStore(&maxLength, sizeof(SCNatural));
  saveArea.HistoryStore(&curLength, sizeof(SCNatural));
  saveArea.Store(s, sizeof(SCCharacter) * (curLength + 1));
  
  return true;
}

// Zusaetzliche Methode fuer History-Variablen:

SCBoolean SCCharstring::HistorySave(SCMem& saveArea) const
{
  saveArea.HistoryStore(&maxLength, sizeof(SCNatural));
  saveArea.HistoryStore(&curLength, sizeof(SCNatural));
  saveArea.HistoryStore(s, sizeof(SCCharacter) * (curLength + 1));
  
  return true;
}

SCBoolean SCCharstring::Restore(SCMem& saveArea)
{
  saveArea.HistoryRestore(&maxLength, sizeof(SCNatural));
  saveArea.HistoryRestore(&curLength, sizeof(SCNatural));
  
  if (s)
  {
    delete[] s;
  }
  
  s = new SCCharacter[curLength + 1];
  assert(s);

  saveArea.Restore(s, sizeof(SCCharacter) * (curLength + 1));
  
  return true;
}


// Zusaetzliche Methode fuer History-Variablen:
SCBoolean SCCharstring::HistoryRestore(SCMem& saveArea)
{
  saveArea.HistoryRestore(&maxLength, sizeof(SCNatural));
  saveArea.HistoryRestore(&curLength, sizeof(SCNatural));
  
  if (s)
  {
    delete[] s;
  }
  
  s = new SCCharacter[curLength + 1];
  assert(s);

  saveArea.HistoryRestore(s, sizeof(SCCharacter) * (curLength + 1));
  
  return true;
}


void SCCharstring::Size(SCSize *curSize) const
{
  curSize->size += (curLength + 1);               // Provide for \0!
  curSize->historySize += (sizeof(SCNatural) +    // Member MaxLength
                           sizeof(SCNatural));    // Member CurLength
}


void SCCharstring::HistorySize(SCSize *curSize) const
{
  curSize->historySize += (curLength + 1);        // Provide for \0!
  curSize->historySize += (sizeof(SCNatural) +    // Member MaxLength
                           sizeof(SCNatural));    // Member CurLength
}
