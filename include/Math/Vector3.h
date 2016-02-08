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
// OVERVIEW: Vector3.h
// ========
// Class definition for 3D vector.
//
// Author: Paulo Pagliosa
// Last revision: 18/10/2014

#ifndef __Vector3_h
#define __Vector3_h

#include <stdio.h>
#include "Math/Real.h"

DS_BEGIN_NAMESPACE

using namespace Math;

// Forward definition
template <typename real> class Vector4;


/////////////////////////////////////////////////////////////////////
//
// Vector3: 3D vector class
// =======
template <typename real>
class Vector3
{
public:
  typedef Vector3<real> vec3;
  typedef Vector4<real> vec4;

  real x;
  real y;
  real z;

  /// Default constructor.
  __host__ __device__
  Vector3()
  {
    // do nothing
  }

  /// Constructs a Vector3 object from (x, y, z).
  __host__ __device__
  Vector3(real x, real y, real z = 0)
  {
    set(x, y, z);
  }

  /// Constructs a Vector3 object from v[3].
  __host__ __device__
  explicit Vector3(const real v[])
  {
    set(v);
  }

  /// Constructs a Vector3 object with (s, s, s).
  __host__ __device__
  explicit Vector3(real s)
  {
    set(s);
  }

  /// Constructs a Vector3 object from v.
  __host__ __device__
  explicit Vector3(const vec4& v)
  {
    set(v);
  }

  __host__ __device__
  template <typename T>
  explicit Vector3(const Vector3<T>& v):
    x { real(v.x) },
    y { real(v.y) },
    z { real(v.z) }
  {
    // do nothing
  }

  /// Sets this object to v.
  __host__ __device__
  void set(const vec3& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  /// Sets the coordinates of this object to (x, y, z).
  __host__ __device__
  void set(real x, real y, real z = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  /// Sets the coordinates of this object to v[3].
  __host__ __device__
  void set(const real v[])
  {
    x = v[0];
    y = v[1];
    z = v[2];
  }

  /// Sets the coordinates of this object to (s, s, s).
  __host__ __device__
  void set(real s)
  {
    x = y = z = s;
  }

  /// Sets the coordinates of this object from v.
  __host__ __device__
  void set(const vec4& v); // implemented in Vector4.h

  __host__ __device__
  vec3& operator =(const vec4& v)
  {
    set(v);
    return *this;
  }

  __host__ __device__
  template <typename T>
  vec3& operator =(const Vector3<T>& v)
  {
    set(real(v.x), real(v.y), real(v.z));
    return *this;
  }

  /// Returns a null vector.
  __host__ __device__
  static vec3 null()
  {
    return vec3(real(0));
  }

  /// Returns the up vector.
  __host__ __device__
  static vec3 up()
  {
    return vec3(real(0), real(1), real(0));
  }

  /// Returns true if this object is equal to v.
  __host__ __device__
  bool equals(const vec3& v, real eps = FloatInfo<real>::eps()) const
  {
    return Math::isNull<real>(x - v.x, y - v.y, z - v.z, eps);
  }

  bool operator ==(const vec3& v) const
  {
    return equals(v);
  }

  /// Returns true if this object is not equal to v.
  bool operator !=(const vec3& v) const
  {
    return !operator ==(v);
  }

  /// Returns a reference to this object += b.
  __host__ __device__
  vec3& operator +=(const vec3& b)
  {
    x += b.x;
    y += b.y;
    z += b.z;
    return *this;
  }

  /// Returns a reference to this object -= b.
  __host__ __device__
  vec3& operator -=(const vec3& b)
  {
    x -= b.x;
    y -= b.y;
    z -= b.z;
    return *this;
  }

  /// Returns a reference to this object *= s.
  __host__ __device__
  vec3& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  /// Returns a reference to this object *= b.
  __host__ __device__
  vec3& operator *=(const vec3& b)
  {
    x *= b.x;
    y *= b.y;
    z *= b.z;
    return *this;
  }

  /// Returns a reference to the i-th coordinate of this object.
  __host__ __device__
  real& operator [](int i)
  {
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  __host__ __device__
  const real& operator [](int i) const
  {
    return (&x)[i];
  }

  /// Returns this object + b.
  __host__ __device__
  vec3 operator +(const vec3& b) const
  {
    return vec3(x + b.x, y + b.y, z + b.z);
  }

  /// Returns this object - b.
  __host__ __device__
  vec3 operator -(const vec3& b) const
  {
    return vec3(x - b.x, y - b.y, z - b.z);
  }

  /// Returns a vector in the direction opposite to this object.
  __host__ __device__
  vec3 operator -() const
  {
    return vec3(-x, -y, -z);
  }

  /// Returns the scalar multiplication of this object and s.
  __host__ __device__
  vec3 operator *(real s) const
  {
    return vec3(x * s, y * s, z * s);
  }

  /// Returns the multiplication of this object and b.
  __host__ __device__
  vec3 operator *(const vec3& b) const
  {
    return vec3(x * b.x, y * b.y, z * b.z);
  }

  /// Returns true if this object is null.
  __host__ __device__
  bool isNull(real eps = FloatInfo<real>::eps()) const
  {
    return Math::isNull<real>(x, y, z, eps);
  }

  /// Returns the squared norm of this object.
  __host__ __device__
  real normSquared() const
  {
    return sqr(x) + sqr(y) + sqr(z);
  }

  /// Returns the length of this object.
  __host__ __device__
  real length() const
  {
    return real(sqrt(normSquared()));
  }

  /// Returns the maximum coordinate of this object.
  __host__ __device__
  real max() const
  {
    return dMax<real>(x, dMax<real>(y, z));
  }

  /// Returns the mainimum coordinate of this object.
  __host__ __device__
  real min() const
  {
    return dMin<real>(x, dMin<real>(y, z));
  }

  /// Returns the inverse of this object.
  __host__ __device__
  vec3 inverse() const
  {
    return vec3(1 / x, 1 / y, 1 / z);
  }

  /// Negates and returns a reference to this object.
  __host__ __device__
  vec3& negate()
  {
    x = -x;
    y = -y;
    z = -z;
    return *this;
  }

  /// Normalizes and returns a reference to this object.
  __host__ __device__
  vec3& normalize(real eps = FloatInfo<real>::eps())
  {
    const real len = length();

    if (!Math::isZero<real>(len, eps))
      operator *=(Math::inverse<real>(len));
    return *this;
  }

  /// Returns the unit vector of this this object.
  __host__ __device__
  vec3 versor(real eps = FloatInfo<real>::eps()) const
  {
    return vec3(*this).normalize(eps);
  }

  /// Returns the unit vector of v.
  __host__ __device__
  static vec3 versor(const vec3& v)
  {
    return v.versor();
  }

  /// Returns the dot product of this object and b.
  __host__ __device__
  real dot(const vec3& b) const
  {
    return x * b.x + y * b.y + z * b.z;
  }

  /// Returns the dot product of this object and (x, y, z).
  __host__ __device__
  real dot(real x, real y, real z) const
  {
    return dot(vec3(x, y, z));
  }

  /// Returns the dot product of v and w.
  __host__ __device__
  static real dot(const vec3& v, const vec3& w)
  {
    return v.dot(w);
  }

  /// Returns the cross product of this object and b.
  __host__ __device__
  vec3 cross(const vec3& b) const
  {
    const real cx = y * b.z - z * b.y;
    const real cy = z * b.x - x * b.z;
    const real cz = x * b.y - y * b.x;

    return vec3(cx, cy, cz);
  }

  /// Returns the cross product of this object and (x, y, z).
  __host__ __device__
  vec3 cross(real x, real y, real z) const
  {
    return cross(vec3(x, y, z));
  }

  /// Returns the cross product of v and w.
  __host__ __device__
  static vec3 cross(const vec3& v, const vec3& w)
  {
    return v.cross(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%f,%f,%f>\n", s, x, y, z);
  }

}; // Vector3

/// Returns the scalar multiplication of s and v.
template <typename real>
__host__ __device__ inline Vector3<real>
operator *(double s, const Vector3<real>& v)
{
  return v * (real)s;
}

DS_END_NAMESPACE

/// Default vec3 type.
typedef DS_NAMESPACE::Vector3<REAL> vec3;

#endif // __Vector3_h
