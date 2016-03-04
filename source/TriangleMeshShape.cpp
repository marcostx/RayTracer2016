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
//  OVERVIEW: TriangleMeshShape.cpp
//  ========
//  Source file for triangle mesh shape.

#include "TriangleMeshShape.h"

using namespace Graphics;


//////////////////////////////////////////////////////////
//
// TriangleShape implementation
// =============
bool
TriangleShape::intersect(const Ray& ray, Intersection& hit) const
//[]---------------------------------------------------[]
//|  Intersect                                          |
//[]---------------------------------------------------[]
{
  const vec3* vertices = mesh->getData().vertices;
  const vec3& p0 = vertices[v[0]];
  const vec3& p1 = vertices[v[1]];
  const vec3& p2 = vertices[v[2]];
  vec3 e1 = p1 - p0;
  vec3 e2 = p2 - p0;
  vec3 s1 = ray.direction.cross(e2);
  REAL invDet = s1.dot(e1);

  if (Math::isZero(invDet))
    return false;
  invDet = Math::inverse(invDet);

  // Compute first barycentric coordinate
  vec3 s = ray.origin - p0;
  REAL b1 = s.dot(s1) * invDet;

  if (b1 < 0 || b1 > 1)
    return false;

  // Compute second barycentric coordinate
  vec3 s2 = s.cross(e1);
  REAL b2 = ray.direction.dot(s2) * invDet;

  if (b2 < 0 || b1 + b2 > 1)
    return false;

  // Compute distance to the intersection point
  REAL t = e2.dot(s2) * invDet;

  if (t < ray.minD || t > ray.maxD)
    return false;
  hit.distance = t;
  hit.triangle = this;
  hit.p.set(1 - b1 - b2, b1, b2);
  return true;
}

vec3
TriangleShape::normal(const Intersection& hit) const
//[]---------------------------------------------------[]
//|  Normal                                             |
//[]---------------------------------------------------[]
{
  const vec3* normals = mesh->getData().normals;

  if (normals == 0)
    return triangleNormal(mesh->getData().vertices, v);

  const vec3& N0 = normals[v[0]];
  const vec3& N1 = normals[v[1]];
  const vec3& N2 = normals[v[2]];

  return triangleInterpolate<vec3>(hit.p, N0, N1, N2).versor();
}

const Material*
TriangleShape::getMaterial() const
//[]---------------------------------------------------[]
//|  Get material                                       |
//[]---------------------------------------------------[]
{
  System::warning("TriangleShape::normal() invoked");
  return 0;
}

Bounds3
TriangleShape::boundingBox() const
//[]---------------------------------------------------[]
//|  Bounding box                                       |
//[]---------------------------------------------------[]
{
  const vec3* vertices = mesh->getData().vertices;
  const vec3& p0 = vertices[v[0]];
  const vec3& p1 = vertices[v[1]];
  const vec3& p2 = vertices[v[2]];
  Bounds3 b;

  b.inflate(p0);
  b.inflate(p1);
  b.inflate(p2);
  return b;
}


//////////////////////////////////////////////////////////
//
// TriangleMeshShape implementation
// =================
Object*
TriangleMeshShape::clone() const
//[]---------------------------------------------------[]
//|  Make copy                                          |
//[]---------------------------------------------------[]
{
  return new TriangleMeshShape((TriangleMesh*)mesh->clone());
}

bool
TriangleMeshShape::canIntersect() const
//[]---------------------------------------------------[]
//|  Can intersect                                      |
//[]---------------------------------------------------[]
{
  return false;
}

Array<ModelPtr>
TriangleMeshShape::refine() const
//[]---------------------------------------------------[]
//|  Refine                                             |
//[]---------------------------------------------------[]
{
  int nt = mesh->getData().numberOfTriangles;
  Array<ModelPtr> a(nt);

  for (int t = 0; t < nt; t++)
    a.add(new TriangleShape(mesh, t));
  return a;
}

bool
TriangleMeshShape::intersect(const Ray&, Intersection&) const
//[]---------------------------------------------------[]
//|  Normal                                             |
//[]---------------------------------------------------[]
{
  System::warning("TriangleMeshShape::intersection() invoked");
  return false;
}

vec3
TriangleMeshShape::normal(const Intersection&) const
//[]---------------------------------------------------[]
//|  Normal                                             |
//[]---------------------------------------------------[]
{
  System::warning("TriangleMeshShape::normal() invoked");
  return vec3::null();
}

const TriangleMesh*
TriangleMeshShape::triangleMesh() const
//[]---------------------------------------------------[]
//|  Triangle mesh                                      |
//[]---------------------------------------------------[]
{
  return mesh;
}

Bounds3
TriangleMeshShape::boundingBox() const
//[]---------------------------------------------------[]
//|  Bounding box                                       |
//[]---------------------------------------------------[]
{
  return Bounds3(bounds, localToWorld);
}
