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
// OVERVIEW: Vector4.h
// ========
// Class definition for 4D vector.
//
// Author: Paulo Pagliosa
// Last revision: 18/10/2014

#ifndef __Vector4_h
#define __Vector4_h

#include "Math/Vector3.h"

DS_BEGIN_NAMESPACE


/////////////////////////////////////////////////////////////////////
//
// Vector4: 4D vector class
// =======
template <typename real>
class Vector4
{
public:
  typedef Vector3<real> vec3;
  typedef Vector4<real> vec4;

  real x;
  real y;
  real z;
  real w;

  /// Default constructor.
  __host__ __device__
  Vector4()
  {
    // do nothing
  }

  /// Constructs a Vector4 object from (x, y, z, w).
  __host__ __device__
  Vector4(real x, real y, real z, real w = 0)
  {
    set(x, y, z, w);
  }

  /// Constructs a Vector4 object from v[4].
  __host__ __device__
  explicit Vector4(const real v[])
  {
    set(v);
  }

  /// Constructs a Vector4 object with (s, s, s, s).
  __host__ __device__
  explicit Vector4(real s)
  {
    set(s);
  }

  /// Constructs a Vector4 object from (v, w).
  __host__ __device__
  explicit Vector4(const vec3& v, real w = 0)
  {
    set(v, w);
  }

  /// Sets this object to v.
  __host__ __device__
  void set(const vec4& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
  }

  /// Sets the coordinates of this object to (x, y, z, w).
  __host__ __device__
  void set(real x, real y, real z, real w = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to v[4].
  __host__ __device__
  void set(const real v[])
  {
    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
  }

  /// Sets the coordinates of this object to (s, s, s, s).
  __host__ __device__
  void set(real s)
  {
    x = y = z = s;
  }

  /// Sets the coordinates of this object to (v, w).
  __host__ __device__
  void set(const vec3& v, real w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  __host__ __device__
  vec4& operator =(const vec3& v)
  {
    set(v);
    return *this;
  }

  /// Returns a null vector.
  __host__ __device__
  static vec4 null()
  {
    return vec4(real(0));
  }

  /// Returns true if this object is equal to v.
  __host__ __device__
  bool equals(const vec4& v, real eps = FloatInfo<real>::eps()) const
  {
    return Math::isNull<real>(x - v.x, y - v.y, z - v.z, w - v.w, eps);
  }

  bool operator ==(const vec4& v) const
  {
    return equals(v);
  }

  /// Returns true if this object is not equal to v.
  bool operator !=(const vec4& v) const
  {
    return !operator ==(v);
  }

  /// Returns a reference to this object += b.
  __host__ __device__
  vec4& operator +=(const vec4& b)
  {
    x += b.x;
    y += b.y;
    z += b.z;
    w += b.w;
    return *this;
  }

  /// Returns a reference to this object -= b.
  __host__ __device__
  vec4& operator -=(const vec4& b)
  {
    x -= b.x;
    y -= b.y;
    z -= b.z;
    w -= b.w;
    return *this;
  }

  /// Returns a reference to this object *= s.
  __host__ __device__
  vec4& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /// Returns a reference to this object *= b.
  __host__ __device__
  vec4& operator *=(const vec4& b)
  {
    x *= b.x;
    y *= b.y;
    z *= b.z;
    w *= b.w;
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
  vec4 operator +(const vec4& b) const
  {
    return vec4(x + b.x, y + b.y, z + b.z, w + b.w);
  }

  /// Returns this object - b.
  __host__ __device__
  vec4 operator -(const vec4& b) const
  {
    return vec4(x - b.x, y - b.y, z - b.z, w - b.w);
  }

  /// Returns a vector in the direction opposite to this object.
  __host__ __device__
  vec4 operator -() const
  {
    return vec4(-x, -y, -z, -w);
  }

  /// Returns the scalar multiplication of this object and s.
  __host__ __device__
  vec4 operator *(real s) const
  {
    return vec4(x * s, y * s, z * s, w * s);
  }

  /// Returns the multiplication of this object and b.
  __host__ __device__
  vec4 operator *(const vec4& b) const
  {
    return vec4(x * b.x, y * b.y, z * b.z, w * b.w);
  }

  /// Returns true if this object is null.
  __host__ __device__
  bool isNull(real eps = FloatInfo<real>::eps()) const
  {
    return Math::isNull<real>(x, y, z, w, eps);
  }

  /// Returns the squared norm of this object.
  __host__ __device__
  real normSquared() const
  {
    return sqr(x) + sqr(y) + sqr(z) + sqr(w);
  }

  /// Returns the length of this object.
  __host__ __device__
  real length() const
  {
    return sqrt(normSquared());
  }

  /// Returns the inverse of this object.
  __host__ __device__
  vec4 inverse() const
  {
    return vec4(1 / x, 1 / y, 1 / z, 1 / z);
  }

  /// Negates and returns a reference to this object.
  __host__ __device__
  vec4& negate()
  {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    return *this;
  }

  /// Normalizes and returns a reference to this object.
  __host__ __device__
  vec4& normalize(real eps = FloatInfo<real>::eps())
  {
    const real len = length();

    if (!Math::isZero<real>(len, eps))
      operator *=(Math::inverse<real>(len));
    return *this;
  }

  /// Returns the unit vector of this this object.
  __host__ __device__
  vec4 versor(real eps = FloatInfo<real>::eps()) const
  {
    return vec4(*this).normalize(eps);
  }

  /// Returns the unit vector of v.
  __host__ __device__
  static vec4 versor(const vec4& v)
  {
    return v.versor();
  }

  /// Returns the dot product of this object and b.
  __host__ __device__
  real dot(const vec4& b) const
  {
    return x * b.x + y * b.y + z * b.z + w * b.w;
  }

  /// Returns the dot product of this object and (x, y, z, w).
  __host__ __device__
  real dot(real x, real y, real z, real w) const
  {
    return dot(vec4(x, y, z, w));
  }

  /// Returns the dot product of v and w.
  __host__ __device__
  static real dot(const vec4& v, const vec4& w)
  {
    return v.dot(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%f,%f,%f,%f>\n", s, x, y, z, w);
  }

}; // Vector4

/// Returns the scalar multiplication of s and v.
template <typename real>
__host__ __device__ inline Vector4<real>
operator *(double s, const Vector4<real>& v)
{
  return v * (real)s;
}

/// Sets the coordinates of this object from v.
template <typename real>
__host__ __device__ inline void
Vector3<real>::set(const Vector4<real>& v) // declared in Vector3.h
{
  x = v.x;
  y = v.y;
  z = v.z;
}

DS_END_NAMESPACE

/// Default vec4 type.
typedef DS_NAMESPACE::Vector4<REAL> vec4;

#endif // __Vector4_h
