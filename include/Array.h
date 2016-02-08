#ifndef __Array_h
#define __Array_h

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
// OVERVIEW: Array.h
// ========
// Class definition for generic array.

#include <memory.h>
#include <utility>

#include "Exception.h"

#ifndef D_DEBUG
#define D_PRECONDITION(e)
#else
#define D_PRECONDITION(e) PRECONDITION(e)
#endif

namespace System
{ // begin namespace System

namespace Collections
{ // begin namespace Collections

#define DFL_ARRAY_SIZE 10

template <typename T>
inline void
constructArray(T* dst, int count)
{
  for (; count > 0; count--, dst++)
    new(dst) T(); // placement default constructor
}

template <typename T>
inline void
constructArray(T* dst, const T* src, int count)
{
  for (; count > 0; count--, dst++, src++)
    new(dst) T(*src); // placement copy constructor
}

template <typename T>
inline void
destructArray(T* dst, int count)
{
  for (; count > 0; count--, dst++)
    dst->~T();
}

//
// Forward definitions
//
template <typename T> class Array;
template <typename T> class ArrayIterator;
template <typename T> class PointerArray;
template <typename T> class PointerArrayIterator;


//////////////////////////////////////////////////////////
//
// Array: generic array class
// =====
template <typename T>
class Array
{
public:
  using value_type = T;

  // Constructor
  Array(int = DFL_ARRAY_SIZE, int = 0);

  // Copy constructor
  Array(const Array<T>&) = delete;

  // Move constructor
  Array(Array<T>&& other):
    data(nullptr)
  {
    *this = std::move(other);
  }

  // Destructor
  ~Array()
  {
    clear();
  }

  void add(const T&);
  void addAt(const T&, int);
  bool removeAt(int);
  void clear();

  bool remove(const T& t)
  {
    int i = findIndex(t);
    return i >= 0 ? removeAt(i) : false;
  }

  Array<T>& operator =(const Array<T>&) = delete;

  Array<T>& operator =(Array<T>&&);

  T& operator [](int i)
  {
    D_PRECONDITION(i >= 0 && i < count);
    return data[i];
  }

  const T& operator [](int i) const
  {
    D_PRECONDITION(i >= 0 && i < count);
    return data[i];
  }

  const T* getData() const
  {
    return data;
  }

  int findIndex(const T&) const;

  int size() const
  {
    return count;
  }

  bool isEmpty() const
  {
    return count == 0;
  }

protected:
  int capacity;
  int delta;
  int count;
  value_type* data;

  void resize();

private:
  static T* allocateData(int n)
  {
    T* data = static_cast<T*>(::malloc(n * sizeof(T)));

    if (data == 0)
      throw Exception("Array::allocateData(): out of memory");
    return data;
  }

  static void freeData(T* data)
  {
    ::free(data);
  }

  friend class ArrayIterator<T>;

}; // Array


//////////////////////////////////////////////////////////
//
// Array implementation
// =====
template <typename T>
Array<T>::Array(int initSize, int delta)
{
  PRECONDITION(initSize > 0 && delta >= 0);
  data = allocateData(capacity = initSize);
  this->delta = delta;
  count = 0;
}

template <typename T>
Array<T>&
Array<T>::operator =(Array<T>&& other)
{
  clear();
  data = other.data;
  capacity = other.capacity;
  delta = other.delta;
  count = other.count;
  other.data = nullptr;
  return *this;
}

template <typename T>
void
Array<T>::clear()
{
  if (data != nullptr)
  {
    destructArray<T>(data, count);
    freeData(data);
  }
}

template <typename T>
void
Array<T>::resize()
{
  int newSize = capacity + (delta == 0 ? capacity : delta);
  T* temp = allocateData(newSize);

  memcpy(temp, data, capacity * sizeof(T));
  freeData(data);
  capacity = newSize;
  data = temp;
}

template <typename T>
void
Array<T>::add(const T& t)
{
  if (count >= capacity)
    resize();
  constructArray<T>(data + count++, &t, 1);
}

template <typename T>
void
Array<T>::addAt(const T& t, int i)
{
  PRECONDITION(i >= 0 && i < count);
  if (count >= capacity)
    resize();

  T* src = data + i;

  memmove(src + 1, src, (count - i) * sizeof(T));
  constructArray<T>(data + i, &t, 1);
  count++;
}

template <typename T>
bool
Array<T>::removeAt(int i)
{
  PRECONDITION(i >= 0);
  if (i >= count)
    return false;
  count--;

  T* dst = data + i;

  destructArray<T>(dst, 1);
  memmove(dst, dst + 1, (count - i) * sizeof(T));
  return true;
}

template <typename T>
int
Array<T>::findIndex(const T& t) const
{
  for (int cur = 0; cur < count; cur++)
    if (data[cur] == t)
      return cur;
   return -1;
}


//////////////////////////////////////////////////////////
//
// ArrayIterator: generic array iterator class
// =============
template <typename T>
class ArrayIterator
{
public:
  // Constructor
  ArrayIterator(const Array<T>& array)
  {
    this->array = &array;
    cur = 0;
  }

  // Testing if objects remain in the iterator
  operator int() const
  {
    return cur < array->count;
  }

  // Get the current object
  T& current() const
  {
    return array->data[cur];
  }

  // Restart the iterator
  void restart()
  {
    cur = 0;
  }

  // Next/previous object
  T& operator ++(int)
  {
    return array->data[cur++];
  }

  T& operator ++()
  {
    return array->data[++cur];
  }

  T& operator --(int)
  {
    return array->data[cur--];
  }

  T& operator --()
  {
    return array->data[--cur];
  }

protected:
  const Array<T>* array;
  int cur;

}; // ArrayIterator


//////////////////////////////////////////////////////////
//
// PointerArray: generic pointer array class
// ============
template <typename T>
class PointerArray
{
public:
  bool shouldDelete;

  // Constructor
  PointerArray(int = DFL_ARRAY_SIZE, int = 0);

  // Destructor
  ~PointerArray()
  {
    if (shouldDelete)
      clear();
    delete []data;
  }

  void add(T*);
  void addAt(T*, int);
  bool removeAt(int, bool = false);

  bool remove(T* t, bool forceDelete = false)
  {
    int i = findIndex(t);
    return i >= 0 ? removeAt(i, forceDelete) : false;
  }

  void clear(bool = false);

  T*& operator [](int i)
  {
    D_PRECONDITION(i >= 0 && i < count);
    return data[i];
  }

  const T* operator [](int i) const
  {
    D_PRECONDITION(i >= 0 && i < count);
    return data[i];
  }

  const T** getData() const
  {
    return data;
  }

  int findIndex(T*) const;

  int size() const
  {
    return count;
  }

  bool isEmpty() const
  {
    return count == 0;
  }

protected:
  int capacity;
  int delta;
  int count;
  T** data;

  void resize();

private:
  PointerArray(const PointerArray<T>&);
  PointerArray<T>& operator =(const PointerArray<T>&);

  friend class PointerArrayIterator<T>;

}; // PointerArray


//////////////////////////////////////////////////////////
//
// PointerArray implementation
// ============
template <typename T>
PointerArray<T>::PointerArray(int initSize, int delta):
  shouldDelete(false)
{
  PRECONDITION(initSize >= 0 && delta >= 0);
  data = new T*[capacity = initSize];
  this->delta = delta;
  count = 0;
}

template <typename T>
void
PointerArray<T>::resize()
{
  int newSize = capacity + (delta == 0 ? capacity : delta);
  T** temp = new T*[newSize];

  memcpy(temp, data, capacity * sizeof(T*));
  delete []data;
  capacity = newSize;
  data = temp;
}

template <typename T>
void
PointerArray<T>::add(T* t)
{
  if (count >= capacity)
    resize();
  data[count++] = t;
}

template <typename T>
void
PointerArray<T>::addAt(T* t, int i)
{
  PRECONDITION(i >= 0 && i < count);
  if (count >= capacity)
    resize();

  T** src = data + i;

  memmove(src + 1, src, (count - i) * sizeof(T*));
  data[i] = t;
  count++;
}

template <typename T>
bool
PointerArray<T>::removeAt(int i, bool forceDelete)
{
  PRECONDITION(i >= 0);
  if (i >= count)
    return false;
  if (forceDelete || shouldDelete)
    delete data[i];
  count--;

  T** dst = data + i;

  memmove(dst, dst + 1, (count - i) * sizeof(T*));
  return true;
}

template <typename T>
void
PointerArray<T>::clear(bool forceDelete)
{
  forceDelete |= shouldDelete;
  for (int i = 0, n = size(); i < n; i++)
    if (data[i] != 0)
    {
      if (forceDelete)
        delete data[i];
      data[i] = 0;
    }
  count = 0;
}

template <typename T>
int
PointerArray<T>::findIndex(T* t) const
{
  for (int cur = 0; cur < count; cur++)
    if (data[cur] == t)
      return cur;
   return -1;
}


//////////////////////////////////////////////////////////
//
// PointerArrayIterator: generic pointer array iterator class
// ====================
template <typename T>
class PointerArrayIterator
{
public:
  // Constructor
  PointerArrayIterator(const PointerArray<T>& array)
  {
    this->array = &array;
    cur = 0;
  }

  // Testing if objects remain in the iterator
  operator int() const
  {
    return cur < array->count;
  }

  // Get the current object
  T* current() const
  {
    return array->data[cur];
  }

  // Restart the iterator
  void restart()
  {
    cur = 0;
  }

  // Next/previous object
  T* operator ++(int)
  {
    return array->data[cur++];
  }

  T* operator ++()
  {
    return array->data[++cur];
  }

  T* operator --(int)
  {
    return array->data[cur--];
  }

  T* operator --()
  {
    return array->data[--cur];
  }

protected:
  const PointerArray<T>* array;
  int cur;

}; // PointerArrayIterator

} // end namespace Collections

} // end namespace System

#endif // __Array_h
