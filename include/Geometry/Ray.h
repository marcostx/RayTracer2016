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
//  OVERVIEW: Ray.h
//  ========
//  Class definition for ray.
//
// Author: Paulo Pagliosa
// Last revision: 31/10/2014

#ifndef __Ray_h
#define __Ray_h

#include "Math/Matrix4x4.h"

DS_BEGIN_NAMESPACE


//////////////////////////////////////////////////////////
//
// Ray: ray class
// ===
struct Ray
{
  vec3 origin;
  vec3 direction;
  REAL minD;
  REAL maxD;

  /// Constructs an empty Ray object.
  __host__ __device__
  Ray()
  {
    // do nothing
  }

  __host__ __device__
  Ray(
    const vec3& o,
    const vec3& d,
    REAL t0 = 0,
    REAL t1 = FloatInfo<REAL>::inf()):
    maxD(t1),
    minD(t0)
  {
    set(o, d);
  }

  __host__ __device__
  Ray(const Ray& ray, const mat4& m):
    maxD(ray.maxD),
    minD(ray.minD)
  {
    set(m.transform(ray.origin), m.transformVector(ray.direction));
  }

  __host__ __device__
  void set(const vec3& o, const vec3& d)
  {
    origin = o;
    direction = d;
  }

  __host__ __device__
  void transform(const mat4& m)
  {
    origin = m.transform(origin);
    direction = m.transformVector(direction);
  }

  __host__ __device__
  vec3 operator ()(REAL t) const
  {
    return origin + direction * t;
  }

}; // Ray

DS_END_NAMESPACE

#endif // __Ray_h
