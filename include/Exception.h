#ifndef __Exception_h
#define __Exception_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        GVSG Foundation Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Exception.h
//  ========
//  Class definitions for exceptions.

#include <stdio.h>
#include <stdarg.h>
#include <string>

using namespace std;

namespace System
{ // begin namespace System


//////////////////////////////////////////////////////////
//
// Exception: generic exception class
// =========
class Exception
{
public:
  // Constructors
  Exception()
  {
    // do nothing
  }

  Exception(const string& msg):
    message(msg)
  {
    // do nothing
  }

  const char* getMessage() const
  {
    return message.c_str();
  }

protected:
  string message;

private:
  Exception& operator =(const Exception&);

}; // Exception


//////////////////////////////////////////////////////////
//
// IndexOutOfBoundsException: index out of bounds exception class
// =========================
class IndexOutOfBoundsException: public Exception
{
public:
  // Costructors
  IndexOutOfBoundsException():
    Exception("Index out of bounds exception")
  {
    // do nothing
  }

  IndexOutOfBoundsException(const string& msg):
    Exception(msg)
  {
    // do nothing
  }

}; // IndexOutOfBoundsException


//////////////////////////////////////////////////////////
//
// Precondition: precondition class
// ============
class Precondition: public Exception
{
public:
  // Constructor
  Precondition(const char* expr, const char* file, int line):
    Exception(makeMessage(expr, file, line))
  {
    // do nothing
  }

private:
  static string makeMessage(const char*, const char*, int);

}; // Precondition


//////////////////////////////////////////////////////////
//
// Precondition inline implementtaion
// ============
#define PF_MAXLEN 1024
#define PF_FORMAT "Precondition %s %d: %s"

inline string
Precondition::makeMessage(const char* expr, const char* file, int line)
{
  char msg[PF_MAXLEN];
  int len = sprintf(msg, PF_FORMAT, file, line, expr);

  return string(msg, len);
}

#undef PF_MAXLEN
#undef PF_FORMAT

//
// PRECONDITION macro
//
#define PRECONDITION(expr) \
if (!(expr)) throw Precondition(#expr, __FILE__ , __LINE__)

inline void
warning(const string& msg)
{
  throw Exception(string("**Warning: ") + msg);
}

} // end namespace System

#endif // __Exception_h
