#ifndef __Sweeper_h
#define __Sweeper_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Sweeper.h
//  ========
//  Class definition for generic sweeper.

#include "Core/Flags.h"
#include "DsMath"
#include "Array.h"
#include "List.h"
#include "Object.h"

using namespace Ds;
using namespace System;
using namespace System::Collections;

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Sweeper: generic sweeper class
// =======
class Sweeper
{
public:
  class Polyline
  {
  public:
    enum
    {
      Closed = 1
    };

    class Vertex
    {
    public:
      vec3 position;

      // Constructors
      Vertex()
      {
        // do nothing
      }

      Vertex(const vec3& p):
        position(p)
      {
        // do nothing
      }

      void transform(const mat4& m)
      {
        position = m.transform3x4(position);
      }

      bool operator ==(const Vertex& vertex) const
      {
        return position == vertex.position;
      }

    }; // Sweeper::Polyline::Vertex

    typedef List<Vertex> Vertices;
    typedef ListIterator<Vertex> VertexIterator;

    // Constructors
    Polyline():
      data(new Data())
    {
      // do nothing
    }

    void mv(const vec3& position)
    {
      data->vertices.add(Vertex(position));
    }

    void transform(const mat4&);

    void open()
    {
      data->flags.reset(Closed);
    }

    void close()
    {
      data->flags.set(Closed);
    }

    int getNumberOfVertices() const
    {
      return data->vertices.size();
    }

    VertexIterator getVertexIterator() const
    {
      return VertexIterator(data->vertices);
    }

    Flags getFlags() const
    {
      return data->flags;
    }

    bool isClosed() const
    {
      return data->flags.isSet(Closed);
    }

    vec3 normal() const;

    bool operator ==(const Polyline& polyline) const
    {
      return data == polyline.data;
    }

  private:
    class Data: public Object
    {
    private:
      Polyline::Vertices vertices;
      Flags flags;

      friend class Polyline;

    }; // Sweeper::Polyline::tData

    ObjectPtr<Data> data;

  }; // Sweeper::Polyline

  typedef Array<Polyline> PolylineArray;

  static Polyline makeArc(const vec3&, REAL, const vec3&, REAL, int = 16);
  static Polyline makeCircle(const vec3&, REAL, const vec3&, int = 16);

}; // Sweeper

} // end namespace Graphics

#endif // __Sweeper_h
