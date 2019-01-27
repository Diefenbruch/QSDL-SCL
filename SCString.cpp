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
|   SCString    |   SCString.cc     | 7. Jun 1994   |   SCL             |      |
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


#ifndef TEST_TEMPLATE
//#define   TEST_TEMPLATE     // Define this to test the implementation
#endif


#ifdef  TEST_TEMPLATE
  #include "SCString.tmpl.h"
  #include "SCSyntype.tmpl.h"


class bit : public SCSyntype<SCInteger>
{
  public:
    bit(void) : SCSyntype<SCInteger>(0,1,0, NULL) {};
};

class byte : public SCString<bit>
{
  public:
    byte(void) : SCString<bit>(0,7) {};
};


template class SCSyntype<SCInteger>;
template class SCString<bit>;
template class SCString<byte>;

//template class SCString<SCInteger>;

#endif  // TEST_TEMPLATE
