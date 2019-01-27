/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1995-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCSocket      | SCSocket.cc       |  3. Sep 1996  |   SCL             |      |
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
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

#include "SCStream.h"
#include "SCDebug.h"
#include "SCSocket.h"
#include "SCScheduler.h"

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif


SCSocket::SCSocket (const char *pHostName,
                    const SCNatural pPortNumber) :
  SCStream(SCScheduler::outputStream), // only a dummy not really used
  hostname(pHostName),
  portnumber(pPortNumber),
  hp(NULL),
  isOpen(false)
{
  assert(pHostName);

  if (!CreateSocket())
    exit(1);
}


SCSocket::~SCSocket (void)
{
  if (isOpen)
    Close();

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << *this << " deleted" << std::endl;
#endif
}


SCBoolean SCSocket::CreateSocket (void)
{
  sock = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sock == -1)
  {
    std::cerr << "Can't create socket!" << std::endl;

    return false;
  }

  server.sin_family = AF_INET;

  server.sin_port = htons((int)portnumber);

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << *this << " created" << std::endl;
#endif

  return true;
}


SCBoolean SCSocket::ResolveHostName (void)
{
  assert(hostname);

  if (hostname[0] == '\0')
  {
    std::cerr << "No hostname given!" << std::endl;
    
    return false;
  }

  hp = gethostbyname((char *)hostname);

  if (hp == NULL)
  {
    std::cerr << "Unknown host " << hostname << std::endl;

    return false;
  }

  memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << *this << " host name resolved" << std::endl;
#endif

  return true;
}


SCBoolean SCSocket::Open (void)
{
  SCBoolean firstTry = true;

  assert(hostname);

  if (!hp)
  {
    if (!ResolveHostName())
      return false;
  }

retry:
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
     if (firstTry)
     {
       std::cerr << "Can't connect socket stream!" << std::endl;
     }

     switch (errno)
     {
       case EACCES:
         std::cerr << "Search permission denied!" << std::endl;
         break;
       case EADDRINUSE:
         std::cerr << "Address already in use!" << std::endl;
         break;
       case EADDRNOTAVAIL:
         std::cerr << "Address not available on remote host!" << std::endl;
         break;
       case EAFNOSUPPORT:
         std::cerr << "Address not supported in this address family!" << std::endl;
         break;
       case EALREADY:
         std::cerr << "Socket in use by previous connection attempt!" << std::endl;
         break;
       case EBADF:
         std::cerr << "Socket parameter is not a valid descriptor!" << std::endl;
         break;
       case ECONNREFUSED: // see man-page!!
         if (firstTry)
         {
           std::cerr << "Connection attempt rejected!" << std::endl;
           std::cerr << "Waiting for visualization server..." << std::endl;

           firstTry = false;
         }

         Close();
         if (!CreateSocket())
           exit(1);
         goto retry;
         break;
       case EINPROGRESS:
         std::cerr << "Connection cannot completed immediately!" << std::endl;
         break;
       case EINTR:
         std::cerr << "Connection interrupted!" << std::endl;
         break;
       case EINVAL:
         std::cerr << "Size parameter invalid!" << std::endl;
         break;
       case EIO:
         std::cerr << "I/O error!" << std::endl;
         break;
       case EISCONN:
         std::cerr << "Socket is already connected!" << std::endl;
         break;
       case ENETUNREACH:
         std::cerr << "Network is not reachable!" << std::endl;
         break;
       case ENXIO:
         std::cerr << "Server exited before connection was complete!" << std::endl;
         break;
       case ETIMEDOUT:
         std::cerr << "Connect establishment timed out!" << std::endl;
         break;
       default:
         std::cerr << "Unknown error!" << std::endl;
     }
     return false;
  }
  
  isOpen = true;
  
#if _SC_SOCKET_DEBUG
  scSocketDebugLog << *this << " opened" << std::endl;
#endif

  return true;
}


SCBoolean SCSocket::Close (void)
{
  close(sock);
  
#if _SC_SOCKET_DEBUG
  scSocketDebugLog << *this << " closed" << std::endl;
#endif

  return true;
}


void SCSocket::Display (const SCInteger value)
{
  if (!isOpen)
    return;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSocket::Display (const SCInteger value): ";
  scSocketDebugLog << value << std::endl;
#endif

  if (::write(sock, (const void *)&value, sizeof(SCInteger)) == -1)
    assert(false);
}


void SCSocket::Display (const SCNatural value)
{
  if (!isOpen)
    return;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSocket::Display (const SCNatural value): ";
  scSocketDebugLog << value << std::endl;
#endif

  if (::write(sock, (const void *)&value, sizeof(SCNatural)) == -1)
    assert(false);
}


void SCSocket::Display (const SCReal value)
{
  if (!isOpen)
    return;

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSocket::Display (const SCReal value): ";
  scSocketDebugLog << value << std::endl;
#endif

  if (::write(sock, (const void *)&value, sizeof(SCReal)) == -1)
    assert(false);
}


void SCSocket::Display (const char *value)
{
  SCNatural len;
  char      end = '\n';

  assert(value);
  
  if (!isOpen)
    return;

  len = strlen(value);

#if _SC_SOCKET_DEBUG
  scSocketDebugLog << "SCSocket::Write (const char * value): ";
  scSocketDebugLog << value << " (len = " << len << ")" << std::endl;
#endif

  if (::write(sock, (const void *)&len, sizeof(SCNatural)) == -1)
    assert(false);

  if (::write(sock, (const void *)value, len) == -1)
    assert(false);

  if (::write(sock, (const void *)&end, sizeof(char)) == -1)
    assert(false);
}


SCStream& operator<< (SCStream& pStream, const SCSocket& pData)
{
  assert(pData.hostname);

  pStream << "socket <" << pData.hostname << ">(#";
  pStream << pData.portnumber << ")";

  return pStream;
}


SCStream& SCSocket::Display(SCStream& pStream) const
{
  return (pStream << *this);
}
