#ifndef __List_h
#define __List_h

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
// OVERVIEW: List.h
// ========
// Class definition for generic doubly linked list.

namespace System
{ // begin namespace System

namespace Collections
{ // begin namespace Collections

//
// Doubly list procedures
//
template <typename T>
void
insertNode(T* node, T*& head)
{
  node->next = head;
  node->prev = 0;
  if (head != 0)
    head->prev = node;
  head = node;
}

template <typename T>
void
removeNode(T* node, T*& head)
{
  if (node->next != 0)
    node->next->prev = node->prev;
  if (node->prev != 0)
    node->prev->next = node->next;
  if (node == head)
    head = node->next;
}

//
// Forward definitions
//
template <typename T> class ListImp;
template <typename T> class ListIteratorImp;
template <typename T> class List;
template <typename T> class ListIterator;


//////////////////////////////////////////////////////////
//
// ListImp: generic doubly list class (imp)
// =======
template <typename T>
class ListImp
{
public:
  // Constructor
  ListImp():
    head(0),
    tail(0),
    numberOfElements(0)
  {
    // do nothing
  }

  // Destructor
  ~ListImp()
  {
    clear();
  }

  void addAtHead(T*);
  void addAtTail(T*);

  void add(T* e)
  {
    addAtTail(e);
  }

  void remove(T&);
  void clear();

  T* peekHead() const
  {
    return head;
  }

  T* peekTail() const
  {
    return tail;
  }

  int size() const
  {
    return numberOfElements;
  }

  bool isEmpty() const
  {
    return head == 0;
  }

  ListIteratorImp<T> iterator() const;

protected:
  T* head;
  T* tail;
  int numberOfElements;

private:
  ListImp(const ListImp<T>&);
  ListImp<T>& operator =(const ListImp<T>&);

  friend class ListIteratorImp<T>;
  friend class List<T>;

}; // ListImp


//////////////////////////////////////////////////////////
//
// ListIteratorImp: doubly list iterator class (imp)
// ===============
template <typename T>
class ListIteratorImp
{
public:
  // Constructor
  ListIteratorImp(const ListImp<T>& list)
  {
    this->list = &list;
    cur = list.head;
  }

  // Testing if objects remain in the iterator
  operator bool() const
  {
    return cur != 0;
  }

  // Get the current object
  T* current() const
  {
    return cur;
  }

  // Restart the iterator
  void restart()
  {
    cur = list->head;
  }

  // Remove the current element
  T* remove()
  {
    T* temp = cur;

    if (temp != 0)
    {
      cur = temp->next;
      list->remove(temp);
    }
    return temp;
  }

  // Next/previous object
  T* operator ++(int)
  {
    T*  temp = cur;

    cur = cur->next;
    return temp;
  }

  T* operator ++()
  {
    return cur = cur->next;
  }

  T* operator --(int)
  {
    T*  temp = cur;

    cur = cur->prev;
    return temp;
  }

  T* operator --()
  {
    return cur = cur->prev;
  }

private:
  const ListImp<T>* list;
  T* cur;

}; // ListIteratorImp


//////////////////////////////////////////////////////////
//
// ListImp implementation
// =======
template <typename T>
void
ListImp<T>::addAtHead(T* e)
{
  e->next = head;
  e->prev = 0;
  if (head != 0)
    head->prev = e;
  else
    tail = e;
  head = e;
  numberOfElements++;
}

template <typename T>
void
ListImp<T>::addAtTail(T* e)
{
  e->next = 0;
  e->prev = tail;
  if (tail != 0)
    tail->next = e;
  else
    head = e;
  tail = e;
  numberOfElements++;
}

template <typename T>
void
ListImp<T>::remove(T& e)
{
  if (e.next != 0)
    e.next->prev = e.prev;
  if (e.prev != 0)
    e.prev->next = e.next;
  if (&e == head)
    head = e.next;
  if (&e == tail)
    tail = e.prev;
  numberOfElements--;
}

template <typename T>
void
ListImp<T>::clear()
{
  while (head != 0)
  {
    T* temp = head;

    head = head->next;
    delete temp;
  }
  tail = 0;
  numberOfElements = 0;
}

template <typename T>
inline ListIteratorImp<T>
ListImp<T>::iterator() const
{
  return ListIteratorImp<T>(*this);
}

//
// Macro to declare a doubly list element
//
#define DECLARE_LIST_ELEMENT(cls) \
  friend void System::Collections::insertNode<>(cls*, cls*&); \
  friend void System::Collections::removeNode<>(cls*, cls*&); \
public: \
  cls* getNext() \
  { \
    return next; \
  } \
  cls* getPrev() \
  { \
    return prev; \
  } \
  friend class System::Collections::ListImp< cls >; \
  friend class System::Collections::ListIteratorImp< cls >; \
private: \
  cls* next; \
  cls* prev


//////////////////////////////////////////////////////////
//
// ListElement: list element class
// ===========
template <typename T>
class ListElement
{
public:
  // Constructor
  ListElement(const T& v):
    value(v)
  {
    // do nothing
  }

  bool operator ==(const ListElement& e) const
  {
    return value == e.value;
  }

  T value;
  ListElement<T>* next;
  ListElement<T>* prev;

}; // ListElement


//////////////////////////////////////////////////////////
//
// List: generic list class
// ====
template <typename T>
class List
{
public:
  // Constructor
  List()
  {
    // do nothing
  }

  void addAtHead(const T& t)
  {
    imp.addAtHead(new ListElement<T>(t));
  }

  void addAtTail(const T& t)
  {
    imp.addAtTail(new ListElement<T>(t));
  }

  void add(const T& t)
  {
    addAtTail(t);
  }

  bool remove(const T&);
  bool removeHead();

  void clear()
  {
    imp.clear();
  }

  bool contains(const T& t) const
  {
    return find(t) != 0;
  }

  int size() const
  {
    return imp.size();
  }

  bool isEmpty() const
  {
    return imp.isEmpty();
  }

  ListIterator<T> iterator() const;

protected:
  ListImp<ListElement<T> > imp;

private:
  List(const List<T>&);
  List<T>& operator =(const List<T>&);

  ListElement<T>* find(const T&) const;

  void remove(ListElement<T>* e)
  {
    imp.remove(*e);
    delete e;
  }

  friend class ListIterator<T>;

}; // List


//////////////////////////////////////////////////////////
//
// ListIterator: generic list iterator class
// ============
template <typename T>
class ListIterator
{
public:
  // Constructor
  ListIterator(const List<T>& list):
    imp(list.imp)
  {
    // do nothing
  }

  // Testing if objects remain in the iterator
  operator bool() const
  {
    return imp;
  }

  // Restart the iterator
  void restart()
  {
    imp.restart();
  }

  // Get the current object
  T& current() const
  {
    return imp.current()->value;
  }

  // Next/previous object
  T& operator ++(int)
  {
    return (imp++)->value;
  }

  T& operator ++()
  {
    return (++imp)->value;
  }

  T& operator --(int)
  {
    return (imp--)->value;
  }

  T& operator --()
  {
    return (--imp)->value;
  }

private:
  ListIteratorImp<ListElement<T> > imp;

}; // ListIterator


//////////////////////////////////////////////////////////
//
// List implementation
// ====
template <typename T>
bool
List<T>::remove(const T& t)
{
  ListElement<T>* temp = find(t);

  if (temp == 0)
    return false;
  remove(temp);
  return true;
}

template <typename T>
bool
List<T>::removeHead()
{
  if (imp.head == 0)
    return false;
  remove(imp.head);
  return true;
}

template <typename T>
ListElement<T>*
List<T>::find(const T& t) const
{
  for (ListElement<T>* e = imp.head; e; e = e->next)
    if (e->value == t)
      return e;
  return 0;
}

template <typename T>
inline ListIterator<T>
List<T>::iterator() const
{
  return ListIterator<T>(*this);
}

} // end namespace Collection

} // end namespace System

#endif // __List_h
