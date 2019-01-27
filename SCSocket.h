/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCSocket      |  SCSocket.h       |  20. Apr 1997 |   SCL             |       |
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

#ifndef __SCSOCKET__
#define __SCSOCKET__

#include "SCStream.h"

#if _SC_PLATFORM_MAC
  #include <GUSI.h>
#else
  #include <netinet/in.h>
#endif

#include "SCObject.h"

class SCSocket : public SCStream
{
  public:
                       // Konstruktor
                       SCSocket (const char * pHostName,
                                 const SCNatural pPortNumber);
                      ~SCSocket (void);

    const char *       GetHostName(void) const { return hostname; }
    const SCNatural    GetPortNumber (void) const { return portnumber; }

    SCBoolean          CreateSocket (void);
    SCBoolean          ResolveHostName (void);
    SCBoolean          Open (void);
    SCBoolean          Close (void);

    SCBoolean          IsOpen (void) const { return isOpen; }

    SCSocket &         operator<< (const SCInteger value) { Display(value); return *this; }
    SCSocket &         operator<< (const SCNatural value) { Display(value); return *this; }
    SCSocket &         operator<< (const SCReal value) { Display(value); return *this; }
    SCSocket &         operator<< (const char *value) { Display(value); return *this; }
        
    void               Display (const SCInteger value);
    void               Display (const SCNatural value);
    void               Display (const SCReal value);
    void               Display (const char *value);

    friend SCStream &  operator<< (SCStream& pStream,
                                   const SCSocket& pData);
    virtual SCStream & Display (SCStream &pStream) const;

  private:
    const char *       hostname;
    const SCNatural    portnumber;
    int                sock;
    struct sockaddr_in server;
    struct hostent *   hp;
    SCBoolean          isOpen;
};


#endif  //  __SCSOCKET__
