#ifndef __TriangleMesh_h
#define __TriangleMesh_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2010-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: TriangleMesh.h
//  ========
//  Class definition for simple triangle mesh.

#include "Geometry/Bounds3.h"
#include "Graphics/Color.h"
#include "Object.h"

using namespace Ds;

namespace Graphics
{ // begin namespace Graphics

using namespace System;

//
// Auxiliary functions
//
__host__ __device__ inline vec3
triangleNormal(const vec3& v0, const vec3& v1, const vec3& v2)
{
  return ((v1 - v0).cross(v2 - v0)).versor();
}

__host__ __device__ inline vec3
triangleNormal(const vec3* v)
{
  return triangleNormal(v[0], v[1], v[2]);
}

__host__ __device__ inline vec3
triangleNormal(const vec3* v, int i, int j, int k)
{
  return triangleNormal(v[i], v[j], v[k]);
}

__host__ __device__ inline vec3
triangleNormal(const vec3* v, const int i[3])
{
  return triangleNormal(v[i[0]], v[i[1]], v[i[2]]);
}

__host__ __device__ inline vec3
triangleCenter(const vec3& v0, const vec3& v1, const vec3& v2)
{
  return (v0 + v1 + v2) * Math::inverse<REAL>(3);
}

__host__ __device__ inline vec3
triangleCenter(const vec3* v)
{
  return triangleCenter(v[0], v[1], v[2]);
}

__host__ __device__ inline vec3
triangleCenter(const vec3* v, int i, int j, int k)
{
  return triangleCenter(v[i], v[j], v[k]);
}

__host__ __device__ inline vec3
triangleCenter(const vec3* v, const int i[3])
{
  return triangleCenter(v[i[0]], v[i[1]], v[i[2]]);
}

template <typename T>
__host__ __device__ inline T
triangleInterpolate(const vec3& p, const T& v0, const T& v1, const T& v2)
{
  return v0 * p.x + v1 * p.y + v2 * p.z;
}

template <typename T>
__host__ __device__ inline T
triangleInterpolate(const vec3& p, const T v[3])
{
  return triangleInterpolate<T>(p, v[0], v[1], v[2]);
}


//////////////////////////////////////////////////////////
//
// TriangleMesh: simple triangle mesh class
// ============
class TriangleMesh: public Object
{
public:
  struct Triangle
  {
    int v[3];

    void setVertices(int v0, int v1, int v2)
    {
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;
    }

  }; // Triangle

  struct Data
  {
    vec3* vertices;
    vec3* normals;
    Color* vertexColors;
    Triangle* triangles;

  }; // Data

  struct Arrays: public Data
  {
    int numberOfVertices;
    int numberOfNormals;
    int numberOfVertexColors;
    int numberOfTriangles;

    // Constructor
    Arrays():
      numberOfVertices(0),
      numberOfNormals(0),
      numberOfVertexColors(0),
      numberOfTriangles(0)
    {
      vertices = 0;
      normals = 0;
      vertexColors = 0;
      triangles = 0;
    }

    Arrays copy() const;
    void print(FILE*) const;

  }; // Arrays

  const uint id;
  ObjectPtr<Object> userData;

  // Constructor
  TriangleMesh(const Arrays& a):
    id(++nextId),
    data(a)
  {
    // do nothing
  }

  // Destructor
  ~TriangleMesh()
  {
    delete data.vertices;
    delete data.normals;
    delete data.vertexColors;
    delete data.triangles;
  }

  Object* clone() const;
  Bounds3 boundingBox() const;

  void computeNormals();
  void transform(const mat4&);

  const Arrays& getData() const
  {
    return data;
  }

  bool hasNormals() const
  {
    return data.normals != 0;
  }

  bool hasVertexColors() const
  {
    return data.vertexColors != 0;
  }

protected:
  Arrays data;

private:
  static uint nextId;

}; // TriangleMesh

} // end namespace Graphics

#endif // __TriangleMesh_h
