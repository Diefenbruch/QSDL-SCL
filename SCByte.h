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
|   SCByte      |   SCByte.h        | 17. Aug 1995  |   SCL             |       |
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


#ifndef __SCBYTE__

#define __SCBYTE__

#include "SCBasicTypes.h"

typedef unsigned short SCByte;
typedef SCByte byte;


_SCINLINEC_ SCByte band (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bor (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bxor (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bnot (const SCByte v);
_SCINLINEC_ SCByte bshl (const SCByte v, const SCInteger shift);
_SCINLINEC_ SCByte bshr (const SCByte v, const SCInteger shift);

_SCINLINEC_ SCByte bplus (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bsub (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bmul (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bdiv (const SCByte v1, const SCByte v2);
_SCINLINEC_ SCByte bmod (const SCByte v1, const SCByte v2);

extern SCByte bhex (const class SCCharstring &cs);

_SCINLINEC_ SCByte i2b (const SCInteger v);
_SCINLINEC_ SCInteger b2i (const SCByte v);


#if !_SC_NOINLINES
  #include "SCByte.inl.h"
#endif


#endif  // __SCBYTE__
