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
//  OVERVIEW: Model.cpp
//  ========
//  Source file for generic model.

#include "TriangleMeshShape.h"

using namespace Graphics;

static const mat4*
identity()
{
  static mat4 I(mat4::identity());
  return &I;
}


//////////////////////////////////////////////////////////
//
// Model implementation
// =====
Model::~Model()
//[]---------------------------------------------------[]
//|  Destructor                                         |
//[]---------------------------------------------------[]
{
  // do nothing
}

bool
Model::canIntersect() const
//[]---------------------------------------------------[]
//|  Can intersect                                      |
//[]---------------------------------------------------[]
{
  return true;
}

Array<ModelPtr>
Model::refine() const
//[]---------------------------------------------------[]
//|  Refine                                             |
//[]---------------------------------------------------[]
{
  Array<ModelPtr> a(1);

  System::warning("Model::refine() invoked");
  a.add((Model*)this);
  return a;
}

const TriangleMesh*
Model::triangleMesh() const
//[]---------------------------------------------------[]
//|  Triangle mesh                                      |
//[]---------------------------------------------------[]
{
  return 0;
}

const mat4&
Model::getLocalToWorldMatrix() const
//[]---------------------------------------------------[]
//|  Get local to world matrix                          |
//[]---------------------------------------------------[]
{
  return *::identity();
}

const mat4&
Model::getWorldToLocalMatrix() const
//[]---------------------------------------------------[]
//|  Get world to local matrix                          |
//[]---------------------------------------------------[]
{
  return *::identity();
}


//////////////////////////////////////////////////////////
//
// ModelInstance implementation
// =============
const TriangleMesh*
ModelInstance::triangleMesh() const
//[]---------------------------------------------------[]
//|  Triangle mesh                                      |
//[]---------------------------------------------------[]
{
  return model->triangleMesh();
}

bool
ModelInstance::intersect(const Ray& ray, Intersection& hit) const
{
  Ray localRay(ray, worldToLocal);
  REAL d = Math::inverse(localRay.direction.length());

  localRay.direction *= d;
  if (!model->intersect(localRay, hit))
    return false;
  hit.distance *= d;
  hit.object = this;
  return true;
}

vec3
ModelInstance::normal(const Intersection& hit) const
//[]---------------------------------------------------[]
//|  Normal                                             |
//[]---------------------------------------------------[]
{
  vec3 N = hit.triangle->normal(hit);
  return localToWorld.transformVector(N).versor();
}

Bounds3
ModelInstance::boundingBox() const
//[]---------------------------------------------------[]
//|  Bounding box                                       |
//[]---------------------------------------------------[]
{
  return Bounds3(model->boundingBox(), localToWorld);
}


//////////////////////////////////////////////////////////
//
// Aggregate implementation
// =========
vec3
Aggregate::normal(const Intersection&) const
//[]---------------------------------------------------[]
//|  Normal                                             |
//[]---------------------------------------------------[]
{
  System::warning("Aggregate::normal() invoked");
  return vec3::null();
}

const Material*
Aggregate::getMaterial() const
//[]---------------------------------------------------[]
//|  Get Material                                       |
//[]---------------------------------------------------[]
{
  System::warning("Aggregate::getMaterial() invoked");
  return 0;
}


//////////////////////////////////////////////////////////
//
// Primitive implementation
// =========
const Material*
Primitive::getMaterial() const
//[]---------------------------------------------------[]
//|  Get material                                       |
//[]---------------------------------------------------[]
{
  return material;
}

const mat4&
Primitive::getLocalToWorldMatrix() const
//[]---------------------------------------------------[]
//|  Get local to world matrix                          |
//[]---------------------------------------------------[]
{
  return localToWorld;
}

const mat4&
Primitive::getWorldToLocalMatrix() const
//[]---------------------------------------------------[]
//|  Get world to local matrix                          |
//[]---------------------------------------------------[]
{
  return worldToLocal;
}

void
Primitive::setMaterial(Material* m)
//[]---------------------------------------------------[]
//|  Set material                                       |
//[]---------------------------------------------------[]
{
  material = m == 0 ? Material::getDefault() : m;
}

inline mat4
inverseTRS(const mat4& trs)
{
  vec3 u(trs[0]);
  vec3 v(trs[1]);
  vec3 w(trs[2]);

  u *= inverse(u.normSquared());
  v *= inverse(v.normSquared());
  w *= inverse(w.normSquared());

  vec3 p(trs[3]);
  mat4 inv;

  inv[0].set(u.x, v.x, w.x);
  inv[1].set(u.y, v.y, w.y);
  inv[2].set(u.z, v.z, w.z);
  inv[3].set(-(u.dot(p)), -(v.dot(p)), -(w.dot(p)), 1);
  return inv;
}

void
Primitive::setTransform(const vec3& p, const quat& q, const vec3& s)
//[]---------------------------------------------------[]
//|  Set transform                                      |
//[]---------------------------------------------------[]
{
  localToWorld = mat4::TRS(p, q, s);
  worldToLocal = inverseTRS(localToWorld);
}
