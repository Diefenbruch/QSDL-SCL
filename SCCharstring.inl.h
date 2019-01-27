/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCCharstring  | SCCharstring.inl.h| 26. Jul 1995  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     23.05.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __SCCHARSTRING_INL__

#define __SCCHARSTRING_INL__


_SCINLINE_ SCCharstring & SCCharstring::MkString(const SCCharacter stringChar)
{
  return *(new SCCharstring(stringChar));
}


_SCINLINE_ SCCharacter SCCharstring::operator[] (const SCNatural index) const
{
#if _SC_RANGECHECK
  if (index > curLength)
  {
    fprintf (stderr, "ERROR: SCCharstring::operator[](index): index[%ld] > curLength[%ld]\n",
             index, curLength);
    exit (1);
  }
  if (index < 1)
  {
    fprintf (stderr, "ERROR: SCCharstring::operator[](index): index[%ld] < 1\n", index);
    exit (1);
  }
#endif

  return (s[index-1]);
}


_SCINLINE_ SCCharacter SCCharstring::First (void) const
{
  return ((*this)[1]);
}


_SCINLINE_ SCCharacter SCCharstring::Last (void) const
{
  return ((*this)[curLength]);
}


_SCINLINE_ SCInteger SCCharstring::Length (void) const
{
  return ((SCInteger)curLength);
}


_SCINLINE_ SCCharacter *SCCharstring::GetString (void) const
{
  return (s);
}


#endif  // __SCCHARSTRING_INL__
