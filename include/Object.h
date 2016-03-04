#ifndef __Object_h
#define __Object_h

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
//  OVERVIEW: Object.h
//  ========
//  Class definition for generic object.

namespace System
{ // begin namespace System


//////////////////////////////////////////////////////////
//
// Object: generic object class
// ======
class Object
{
public:
  // Destructor
  virtual ~Object()
  {
    // do nothing
  }

  // Make a deep-copy of this object
  virtual Object* clone() const
  {
    return 0;
  }

  // Get number of uses of this object
  int getNumberOfUses() const
  {
    return counter;
  }

  template <typename T> friend T* makeUse(T*);

  // Release this object
  void release()
  {
    if (--counter <= 0)
      delete this;
  }

protected:
  // Protected default constructor
  Object():
    counter(0)
  {
    // do nothing
  }

private:
  int counter; // reference counter

}; // Object

template <typename T>
inline T*
clone(T* object)
{
  return object != 0 ? dynamic_cast<T*>(object->clone()) : 0;
}

template <typename T>
inline T*
makeUse(T* object)
{
  if (object != 0)
    ++object->counter;
  return object;
}

template <typename T>
inline void
release(T* object)
{
  if (object != 0)
    object->release();
}


//////////////////////////////////////////////////////////
//
// ObjectPtr: object pointer class
// =========
template <typename T>
class ObjectPtr
{
public:
  // Constructors
  ObjectPtr():
    object(0)
  {
    // do nothing
  }

  ObjectPtr(const ObjectPtr<T>& ptr)
  {
    this->object = makeUse(ptr.object);
  }

  ObjectPtr(T* object)
  {
    this->object = makeUse(object);
  }

  // Destructor
  ~ObjectPtr()
  {
    release(this->object);
  }

  ObjectPtr<T>& operator =(T* object)
  {
    release(this->object);
    this->object = makeUse(object);
    return *this;
  }

  ObjectPtr<T>& operator =(const ObjectPtr<T>& ptr)
  {
    release(this->object);
    this->object = makeUse(ptr.object);
    return *this;
  }

  bool operator ==(T* object) const
  {
    return this->object == object;
  }

  bool operator ==(const ObjectPtr<T>& ptr) const
  {
    return this->object == ptr.object;
  }

  bool operator !=(T* object) const
  {
    return !operator ==(object);
  }

  bool operator !=(const ObjectPtr<T>& ptr) const
  {
    return !operator ==(ptr);
  }

  operator T*() const
  {
    return this->object;
  }

  T* operator ->() const
  {
    return object;
  }

private:
  T* object; // this is the object

}; // ObjectPtr

} // end namespace System

#endif // __Object_h
