#ifndef __TriangleMeshShape_h
#define __TriangleMeshShape_h

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
//  OVERVIEW: TriangleMeshShape.h
//  ========
//  Class definition for triangle mesh shape.

#include "Model.h"
#include "TriangleMesh.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// TriangleMeshShape: triangle mesh shape class
// =================
class TriangleMeshShape: public Primitive
{
public:
  // Constructor
  TriangleMeshShape(TriangleMesh* m):
    mesh(m)
  {
    if (m != 0)
      bounds = m->boundingBox();
  }

  const TriangleMesh* getMesh() const
  {
    return mesh;
  }

  Object* clone() const;

  bool canIntersect() const;
  Array<ModelPtr> refine() const;
  const TriangleMesh* triangleMesh() const;
  bool intersect(const Ray&, Intersection&) const;
  vec3 normal(const Intersection&) const;
  Bounds3 boundingBox() const;

private:
  ObjectPtr<TriangleMesh> mesh;
  Bounds3 bounds;

}; // TriangleMeshShape


//////////////////////////////////////////////////////////
//
// TriangleShape: triangle shape class
// =============
class TriangleShape: public Model
{
public:
  // Constructor
  TriangleShape(TriangleMesh* m, int t):
    mesh(m)
  {
    v = m->getData().triangles[t].v;
  }

  bool intersect(const Ray&, Intersection&) const;
  vec3 normal(const Intersection&) const;
  const Material* getMaterial() const;
  Bounds3 boundingBox() const;

private:
  ObjectPtr<TriangleMesh> mesh;
  const int* v;

}; // TriangleShape

} // end namespace Graphics

#endif // __TriangleMeshShape_h
