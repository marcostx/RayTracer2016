#ifndef __NameableObject_h
#define __NameableObject_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        GVSG Foundation Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2010-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
// OVERVIEW: NameableObject.h
// ========
// Class definition for nameable object.

#include <string>
#include "Object.h"

using namespace std;

namespace System
{ // begin namespace System


//////////////////////////////////////////////////////////
//
// NameableObject: nameable object
// ==============
class NameableObject: public virtual Object
{
public:
  // Constructors
  NameableObject()
  {
    // do nothing
  }

  NameableObject(const string& s):
    name(s)
  {
    // do nothing
  }

  // Get the object name
  string getName() const
  {
    return name;
  }

  // Set the object name
  void setName(const string& name)
  {
    this->name = name;
  }

private:
  string name;

}; // NameableObject

} // end namespace System

#endif // __NameableObject_h
