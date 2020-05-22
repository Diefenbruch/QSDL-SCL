#ifndef _SCSTREAM_H_
#define _SCSTREAM_H_

#include <iostream>
#include <fstream>

#include "SCObject.h"

typedef std::ostream& (*sc_omanip)(std::ostream&);

class SCStream: public SCObject
{
  public:
    SCStream(std::ostream &the_stream) : SCObject(SC_STREAM), scStream(&the_stream), deleteStream(false) {}
    SCStream(const char *fileName) : SCObject(SC_STREAM), scStream(new std::ofstream(fileName)), deleteStream(true) {}
    virtual ~SCStream(void) { if (deleteStream) delete scStream; }

    SCStream& operator<<(char c) { Display(c); return *this;}
    SCStream& operator<<(unsigned char c) { Display(c); return *this;}
    SCStream& operator<<(signed char c) { Display(c); return *this;}
    SCStream& operator<<(const char *s) { Display(s); return *this;}
    SCStream& operator<<(const unsigned char *s) { Display(s); return *this;}
    SCStream& operator<<(const signed char *s) { Display(s); return *this;}
    SCStream& operator<<(const void *p) { Display(p); return *this;}
    SCStream& operator<<(int n) { Display(n); return *this;}
    SCStream& operator<<(unsigned int n) { Display(n); return *this;}
    SCStream& operator<<(long n) { Display(n); return *this;}
    SCStream& operator<<(unsigned long n) { Display(n); return *this;}
    SCStream& operator<<(short n) { Display(n); return *this;}
    SCStream& operator<<(unsigned short n) { Display(n); return *this;}
    SCStream& operator<<(double n) { Display(n); return *this;}
    SCStream& operator<<(float n) { Display(n); return *this;}
    SCStream& operator<<(long double n) { Display(n); return *this;}
    SCStream& operator<<(sc_omanip func) { Display(func); return *this;}

    std::ostream&  GetStream(void) const { return *scStream; }

  protected:
    virtual void Display(char c) { *scStream << c;}
    virtual void Display(unsigned char c) { *scStream << c;}
    virtual void Display(signed char c) { *scStream << c;}
    virtual void Display(const char *s) { *scStream << s;}
    virtual void Display(const unsigned char *s) { *scStream << s;}
    virtual void Display(const signed char *s) { *scStream << s;}
    virtual void Display(const void *p) { *scStream << p;}
    virtual void Display(int n) { *scStream << n;}
    virtual void Display(unsigned int n) { *scStream << n;}
    virtual void Display(long n) { *scStream << n;}
    virtual void Display(unsigned long n) { *scStream << n;}
    virtual void Display(short n) { *scStream << n;}
    virtual void Display(unsigned short n) { *scStream << n;}
    virtual void Display(double n) { *scStream << n;}
    virtual void Display(float n) { *scStream << n;}
    virtual void Display(long double n) { *scStream << n;}
    virtual void Display(sc_omanip func) { *scStream << func;}

    virtual SCStream & Display (SCStream &pStream) const { return pStream; }

  private:
    std::ostream *scStream;
    SCBoolean deleteStream;
};

#endif

