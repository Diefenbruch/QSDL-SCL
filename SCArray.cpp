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
|   SCArray     |   SCArray.cc      | 7. Jun 1994   |   SCL             |      |
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
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef TEST_TEMPLATE
//#define   TEST_TEMPLATE   // Define this to test the implementation
#endif


#ifdef  TEST_TEMPLATE
  #include "SCArray.tmpl.h"
  #include "SCSyntype.tmpl.h"


class bit : public SCSyntype<SCInteger>
{
  public:
    bit(void) : SCSyntype<SCInteger>(0,1,0, NULL) {};
};

class byte : public SCArray<bit>
{
  public:
    byte(void) : SCArray<bit>(0,7) {};
};


template class SCSyntype<SCInteger>;
template class SCArray<bit>;
template class SCArray<byte>;

//template class SCArray<SCInteger>;

#endif  // TEST_TEMPLATE
