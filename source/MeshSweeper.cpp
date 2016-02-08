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
//  OVERVIEW: MeshSweeper.cpp
//  ========
//  Source file for mesh sweeper.

#include <stdio.h>
#include "MeshSweeper.h"

using namespace Graphics;


//////////////////////////////////////////////////////////
//
// MeshSweeper implementation
// ===========
TriangleMesh*
MeshSweeper::makeCylinder(
  const vec3& center,
  REAL radius,
  const vec3& height,
  int segments)
//[]----------------------------------------------------[]
//|  Make cylinder                                       |
//[]----------------------------------------------------[]
{
  const vec3 baseCenter = center - height * REAL(0.5);
  Polyline base = makeCircle(baseCenter, radius, height, segments);
  const int nt = segments << 2; // number of triangles
  const int nv = nt + 2; // number of vertices
  TriangleMesh::Arrays data;
  const int b = segments << 1;

  data.vertices = new vec3[data.numberOfVertices = nv];
  data.normals = new vec3[data.numberOfNormals = nv];
  data.triangles = new TriangleMesh::Triangle[data.numberOfTriangles = nt];
  if (true)
  {
    Polyline::VertexIterator vit(base.getVertexIterator());
    const vec3 baseNormal = -height.versor();
    const REAL invR = Math::inverse<REAL>(radius);
    int k = b;
    int l = k + segments + 1;

    for (int i = 0; i < segments; i++, k++, l++)
    {
      const vec3& p = vit++.position;
      int j = i + segments;

      data.vertices[i] = data.vertices[k] = p + height;
      data.vertices[j] = data.vertices[l] = p;
      data.normals[i] = data.normals[j] = invR * (p - baseCenter);
      data.normals[k] = -(data.normals[l] = baseNormal);
    }
    data.vertices[k] = baseCenter + height;
    data.vertices[l] = baseCenter;
    data.normals[k] = -(data.normals[l] = baseNormal);
  }

  TriangleMesh::Triangle* triangle = data.triangles;
  const int s = b + segments;

  for (int t = b + 1, i = 0; i < segments; i++, triangle++)
  {
    int j = i + segments;
    int k = (i + 1) % segments;
    int l = k + segments;

    triangle->setVertices(i, j, k);
    triangle[segments].setVertices(j, l, k);
    triangle[b].setVertices(s, i + b, k + b);
    triangle[s].setVertices(nv - 1, l + t, j + t);
  }
  return new TriangleMesh(data);
}

inline void
setBoxVertices(vec3* v)
{
  const vec3 p1(-1, -1, -1);
  const vec3 p2(+1, -1, -1);
  const vec3 p3(+1, +1, -1);
  const vec3 p4(-1, +1, -1);
  const vec3 p5(-1, -1, +1);
  const vec3 p6(+1, -1, +1);
  const vec3 p7(+1, +1, +1);
  const vec3 p8(-1, +1, +1);

  v[ 0] = p1; v[ 1] = p5; v[ 2] = p8; v[ 3] = p4; // x = -1
  v[ 4] = p2; v[ 5] = p3; v[ 6] = p7; v[ 7] = p6; // x = +1
  v[ 8] = p1; v[ 9] = p2; v[10] = p6; v[11] = p5; // y = -1
  v[12] = p4; v[13] = p8; v[14] = p7; v[15] = p3; // y = +1
  v[16] = p1; v[17] = p4; v[18] = p3; v[19] = p2; // z = -1
  v[20] = p5; v[21] = p6; v[22] = p7; v[23] = p8; // z = +1
}

inline void
setBoxNormals(vec3* n)
{
  const vec3 n1(-1, 0, 0);
  const vec3 n2(+1, 0, 0);
  const vec3 n3(0, -1, 0);
  const vec3 n4(0, +1, 0);
  const vec3 n5(0, 0, -1);
  const vec3 n6(0, 0, +1);

  n[ 0] = n[ 1] = n[ 2] = n[ 3] = n1; // x = -1
  n[ 4] = n[ 5] = n[ 6] = n[ 7] = n2; // x = +1
  n[ 8] = n[ 9] = n[10] = n[11] = n3; // y = -1
  n[12] = n[13] = n[14] = n[15] = n4; // y = +1
  n[16] = n[17] = n[18] = n[19] = n5; // z = -1
  n[20] = n[21] = n[22] = n[23] = n6; // z = +1
}

inline void
setBoxTriangles(TriangleMesh::Triangle* t)
{
  t[ 0].setVertices( 0,  1,  2); t[ 1].setVertices( 2,  3,  0);
  t[ 2].setVertices( 4,  5,  7); t[ 3].setVertices( 5,  6,  7);
  t[ 4].setVertices( 8,  9, 11); t[ 5].setVertices( 9, 10, 11);
  t[ 6].setVertices(12, 13, 14); t[ 7].setVertices(14, 15, 12);
  t[ 8].setVertices(16, 17, 19); t[ 9].setVertices(17, 18, 19);
  t[10].setVertices(20, 21, 22); t[11].setVertices(22, 23, 20);
}

TriangleMesh*
MeshSweeper::makeCube()
//[]----------------------------------------------------[]
//|  Make cube                                           |
//[]----------------------------------------------------[]
{
  TriangleMesh::Arrays data;

  data.vertices = new vec3[data.numberOfVertices = 24];
  data.normals = new vec3[data.numberOfNormals = 24];
  data.triangles = new TriangleMesh::Triangle[data.numberOfTriangles = 12];
  setBoxVertices(data.vertices);
  setBoxNormals(data.normals);
  setBoxTriangles(data.triangles);
  return new TriangleMesh(data);
}

TriangleMesh*
MeshSweeper::makeBox(
  const vec3& center,
  const quat& orientation,
  const vec3& scale)
//[]----------------------------------------------------[]
//|  Make box                                            |
//[]----------------------------------------------------[]
{
  TriangleMesh* mesh = makeCube();

  mesh->transform(mat4::TRS(center, orientation, scale));
  return mesh;
}

TriangleMesh*
MeshSweeper::makeSphere(const vec3& center, REAL radius, int mers)
//[]----------------------------------------------------[]
//|  Make sphere                                         |
//[]----------------------------------------------------[]
{
  if (mers < 6)
    mers = 6;

  const int sections = mers;
  const int nv = sections * mers + 2; // number of vertices (and normals)
  const int nt = 2 * mers * sections; // number of triangles
  TriangleMesh::Arrays data;

  data.vertices = new vec3[data.numberOfVertices = nv];
  data.normals = new vec3[data.numberOfNormals = nv];
  data.triangles = new TriangleMesh::Triangle[data.numberOfTriangles = nt];
  if (true)
  {
    Polyline arc = makeArc(center, radius, vec3(0, 0, 1), 180, sections + 1);
    Polyline::VertexIterator vit = arc.getVertexIterator();
    vec3* vertex = data.vertices;
    vec3* normal = data.normals;
    REAL invRadius = Math::inverse<REAL>(radius);

    *normal = ((*vertex = (vit++).position) - center) * invRadius;

    mat4 rot = mat4::rotation(*normal, REAL(360) / mers, center);

    vertex++;
    normal++;
    for (int s = 0; s < sections; s++)
    {
      vec3 p = *vertex = (vit++).position;

      *normal = (p - center) * invRadius;
      vertex++;
      normal++;
      for (int m = 1; m < mers; m++)
      {
        *vertex = p = rot.transform3x4(p);
        *normal = (p - center) * invRadius;
        vertex++;
        normal++;
      }
    }
    *normal = ((*vertex = (vit++).position) - center) * invRadius;
  }

  TriangleMesh::Triangle* triangle = data.triangles;

  for (int i = 1; i <= mers; i++)
  {
    int j = i % mers + 1;

    triangle->setVertices(0, i, j);
    triangle++;
  }
  for (int s = 1; s < sections; s++)
    for (int m = 0, b = (s - 1) * mers + 1; m < mers;)
    {
      int i = b + m;
      int k = b + ++m % mers;
      int j = i + mers;
      int l = k + mers;

      triangle->setVertices(i, j, k);
      triangle[1].setVertices(k, j, l);
      triangle += 2;
    }
  for (int m = 0, b = (sections - 1) * mers + 1, j = nv - 1; m < mers;)
  {
    int i = b + m;
    int k = b + ++m % mers;

    triangle->setVertices(i, j, k);
    triangle++;
  }
  return new TriangleMesh(data);
}

TriangleMesh*
MeshSweeper::makeCone(
  const vec3& baseCenter,
  REAL radius,
  const vec3& height,
  int segments)
//[]----------------------------------------------------[]
//|  Make cone                                           |
//[]----------------------------------------------------[]
{
  Polyline base = makeCircle(baseCenter, radius, height, segments);
  const int nt = segments << 1; // number of triangles
  const int nv = nt + 2; // number of vertices
  TriangleMesh::Arrays data;

  data.vertices = new vec3[data.numberOfVertices = nv];
  data.normals = new vec3[data.numberOfNormals = nv];
  data.triangles = new TriangleMesh::Triangle[data.numberOfTriangles = nt];
  if (true)
  {
    Polyline::VertexIterator vit(base.getVertexIterator());
    const vec3 baseNormal = -height.versor();
    const REAL invR = Math::inverse<REAL>(radius);
    int i = 0;
    int j = segments + 1;

    for (; i < segments; i++, j++)
    {
      const vec3& p = vit++.position;

      data.vertices[i] = data.vertices[j] = p;
      data.normals[i] = invR * (p - baseCenter);
      data.normals[j] = baseNormal;
    }
    data.vertices[i] = baseCenter + height;
    data.vertices[j] = baseCenter;
    data.normals[i] = -(data.normals[j] = baseNormal);
  }

  TriangleMesh::Triangle* triangle = data.triangles;

  for (int t = segments + 1, i = 0; i < segments; i++, triangle++)
  {
    int j = (i + 1) % segments;

    triangle->setVertices(segments, i, j);
    triangle[segments].setVertices(nv - 1, j + t, i + t);
  }
  return new TriangleMesh(data);
}
