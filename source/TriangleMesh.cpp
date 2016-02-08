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
//  OVERVIEW: TriangleMesh.cpp
//  ========
//  Source file for simple triangle mesh.

#include <memory.h>
#include "TriangleMesh.h"

//
// Auxiliary functions
//
template <typename T>
inline void
copyArray(T* dst, const T* src, int n)
{
  memcpy(dst, src, n * sizeof(T));
}

template <typename T>
inline void
copyNewArray(T*& dst, const T* src, int n)
{
  copyArray<T>(dst = new T[n], src, n);
}

using namespace Graphics;

//
// Auxiliary function
//
inline void
printVec3(FILE*f, const char* s, const vec3& p)
{
  fprintf(f, "%s<%g, %g, %g>\n", s, p.x, p.y, p.z);
}


//////////////////////////////////////////////////////////
//
// TriangleMesh implementation
// ============
uint TriangleMesh::nextId;

TriangleMesh::Arrays
TriangleMesh::Arrays::copy() const
//[]---------------------------------------------------[]
//|  Copy data                                          |
//[]---------------------------------------------------[]
{
  Arrays c;

  if (vertices != 0)
  {
    c.numberOfVertices = numberOfVertices;
    ::copyNewArray(c.vertices, vertices, numberOfVertices);
  }
  if (normals != 0)
  {
    c.numberOfNormals = numberOfNormals;
    ::copyNewArray(c.normals, normals, numberOfNormals);
  }
  if (vertexColors != 0)
  {
    c.numberOfVertexColors = numberOfVertexColors;
    ::copyNewArray(c.vertexColors, vertexColors, numberOfVertexColors);
  }
  if (triangles != 0)
  {
    c.numberOfTriangles = numberOfTriangles;
    ::copyNewArray(c.triangles, triangles, numberOfTriangles);
  }
  return c;
}

Object*
TriangleMesh::clone() const
{
  return new TriangleMesh(data.copy());
}

Bounds3
TriangleMesh::boundingBox() const
{
  Bounds3 box;

  for (int i = 0; i < data.numberOfVertices; i++)
    box.inflate(data.vertices[i]);
  return box;
}

void
TriangleMesh::computeNormals()
{
  int nv = data.numberOfVertices;

  if (data.normals == 0)
    data.normals = new vec3[nv];
  else if (data.numberOfNormals != nv)
  {
    delete []data.normals;
    data.normals = new vec3[nv];
  }
  data.numberOfNormals = nv;

  vec3* normals = data.normals;
  Triangle* t = data.triangles;

  memset(normals, 0, nv * sizeof(vec3));
  for (int i = 0; i < data.numberOfTriangles; i++, t++)
  {
    int v0 = t->v[0];
    int v1 = t->v[1];
    int v2 = t->v[2];
    vec3 N = triangleNormal(data.vertices, v0, v1, v2);

    normals[v0] += N;
    normals[v1] += N;
    normals[v2] += N;
  }
  for (int i = 0; i < nv; i++)
    normals[i].normalize();
}

void
TriangleMesh::transform(const mat4& m)
{
  for (int i = 0; i < data.numberOfVertices; i++)
    data.vertices[i] = m.transform3x4(data.vertices[i]);
  for (int i = 0; i < data.numberOfNormals; i++)
    data.normals[i] = m.transformVector(data.normals[i]).versor();
}

void
TriangleMesh::Arrays::print(FILE* f) const
//[]---------------------------------------------------[]
//|  Print                                              |
//[]---------------------------------------------------[]
{
  fprintf(f, "mesh\n{\n\tvertices\n\t{\n\t\t%d\n", numberOfVertices);
  for (int i = 0; i < numberOfVertices; i++)
    printVec3(f, "\t\t", vertices[i]);
  fprintf(f, "\t}\n");
  if (normals != 0)
  {
    fprintf(f, "\tnormals\n\t{\n\t\t%d\n", numberOfNormals);
    for (int i = 0; i < numberOfNormals; i++)
      printVec3(f, "\t\t", normals[i]);
    fprintf(f, "\t}\n");
  }
  fprintf(f, "\ttriangles\n\t{\n\t\t%d\n", numberOfTriangles);

  Triangle* t = triangles;

  for (int i = 0; i < numberOfTriangles; i++, t++)
    fprintf(f, "\t\t<%d, %d, %d>\n", t->v[0], t->v[1], t->v[2]);
  fprintf(f, "\t}\n}\n");
}
