//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Bounds3.h
// ========
// Class definition for axis-aligned bounding box.
//
// Author: Paulo Pagliosa
// Last revision: 31/10/2014

#ifndef __Bounds3_h
#define __Bounds3_h

#include "Geometry/Ray.h"

DS_BEGIN_NAMESPACE

__host__ __device__ inline void
inflateBounds3(vec3& p1, vec3& p2, const vec3& p)
{
  if (p.x < p1.x)
    p1.x = p.x;
  if (p.x > p2.x)
    p2.x = p.x;
  if (p.y < p1.y)
    p1.y = p.y;
  if (p.y > p2.y)
    p2.y = p.y;
  if (p.z < p1.z)
    p1.z = p.z;
  if (p.z > p2.z)
    p2.z = p.z;
}


/////////////////////////////////////////////////////////////////////
//
// Bounds3: axis-aligned bounding box class
// =======
class Bounds3
{
public:
  class PreparedRay: public Ray
  {
  public:
    PreparedRay(const Ray& r):
      Ray(r)
    {
      invDir = r.direction.inverse();
      isNegDir[0] = r.direction.x < 0;
      isNegDir[1] = r.direction.y < 0;
      isNegDir[2] = r.direction.z < 0;
    }

  private:
    vec3 invDir;
    uint isNegDir[3];

    friend class Bounds3;

  }; // PreparedRay

  /// Constructs an empty Bounds3 object.
  __host__ __device__
  Bounds3()
  {
    setEmpty();
  }

  Bounds3(const vec3& min, const vec3& max)
  {
    set(min, max);
  }

  Bounds3(const Bounds3& b, const mat4& m = mat4::identity()):
    p1(b.p1),
    p2(b.p2)
  {
    transform(m);
  }

  __host__ __device__
  vec3 center() const
  {
    return (p1 + p2) * 0.5;
  }

  __host__ __device__
  REAL diagonalLength() const
  {
    return (p2 - p1).length();
  }

  __host__ __device__
  vec3 size() const
  {
    return p2 - p1;
  }

  __host__ __device__
  REAL maxSize() const
  {
    return size().max();
  }

  __host__ __device__
  REAL area() const
  {
    vec3 s = size();
    REAL a = s.x * s.y + s.y * s.z + s.z * s.x;

    return a + a;
  }

  __host__ __device__
  bool isEmpty() const
  {
    return p1.x >= p2.x || p1.y >= p2.y || p1.z >= p2.z;
  }

  __host__ __device__
  const vec3& getMin() const
  {
    return p1;
  }

  __host__ __device__
  const vec3& getMax() const
  {
    return p2;
  }

  __host__ __device__
  const vec3& operator [](int i) const
  {
    return (&p1)[i];
  }

  __host__ __device__
  void setEmpty()
  {
    p1.x = p1.y = p1.z = +FloatInfo<REAL>::inf();
    p2.x = p2.y = p2.z = -FloatInfo<REAL>::inf();
  }

  __host__ __device__
  void set(const vec3& min, const vec3& max)
  {
    p1 = min;
    p2 = max;
    if (max.x < min.x)
      dSwap<REAL>(p1.x, p2.x);
    if (max.y < min.y)
      dSwap<REAL>(p1.y, p2.y);
    if (max.z < min.z)
      dSwap<REAL>(p1.z, p2.z);
  }

  __host__ __device__
  void inflate(const vec3& p)
  {
    inflateBounds3(p1, p2, p);
  }

  __host__ __device__
  void inflate(REAL x, REAL y, REAL z = 0)
  {
    inflate(vec3(x, y, z));
  }

  __host__ __device__
  void inflate(REAL s)
  {
    if (Math::isPositive<REAL>(s))
    {
      vec3 c = center() * (1 - s);

      p1 = p1 * s + c;
      p2 = p2 * s + c;
    }
  }

  __host__ __device__
  void inflate(const Bounds3& b)
  {
    inflate(b.p1);
    inflate(b.p2);
  }

  __host__ __device__
  void transform(const mat4& m)
  {
    vec3 min = p1;
    vec3 max = p2;

    setEmpty();
    for (int i = 0; i < 8; i++)
    {
      vec3 p = min;

      if (i & 1)
        p[0] = max[0];
      if (i & 2)
        p[1] = max[1];
      if (i & 4)
        p[2] = max[2];
      inflate(m.transform3x4(p));
    }
  }

  __host__ __device__
  bool contains(const vec3& p) const
  {
    if (p.x < p1.x || p.x > p2.x)
      return false;
    if (p.y < p1.y || p.y > p2.y)
      return false;
    if (p.z < p1.z || p.z > p2.z)
      return false;
    return true;
  }

  __host__ __device__
  bool intersect(const Ray& ray, REAL& d) const
  {
    return intersect(*this, PreparedRay(ray), d);
  }

protected:
  vec3 p1;
  vec3 p2;

  __host__ __device__
  static bool intersect(const Bounds3& b, const PreparedRay& r, REAL& d)
  {
    REAL tmin, tmax;
    REAL amin, amax;

    tmin = (b[r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
    tmax = (b[1 - r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
    amin = (b[r.isNegDir[1]].y - r.origin.y) * r.invDir.y;
    amax = (b[1 - r.isNegDir[1]].y - r.origin.y) * r.invDir.y;
    if (tmin > amax || amin > tmax)
      return false;
    if (amin > tmin)
      tmin = amin;
    if (amax < tmax)
      tmax = amax;
    amin = (b[r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
    amax = (b[1 - r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
    if (tmin > amax || amin > tmax)
      return false;
    if (amin > tmin)
      tmin = amin;
    if (tmin > r.minD)
      return (d = tmin) < r.maxD;
    if (amax < tmax)
      tmax = amax;
    if (tmax > r.minD)
      return (d = tmax) < r.maxD;
    return false;
  }

}; // Bounds3

DS_END_NAMESPACE

#endif // __Bounds3_h
