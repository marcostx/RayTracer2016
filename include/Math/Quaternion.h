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
// OVERVIEW: Quaternion.h
// ========
// Class definition for quaternion.
//
// Author: Paulo Pagliosa
// Last revision: 18/10/2014

#ifndef __Quaternion_h
#define __Quaternion_h

#include "Math/Vector3.h"

DS_BEGIN_NAMESPACE

// Forward definition
template <typename real> class Matrix3x3;

template <typename real>
inline Vector3<real>
toRadians3(const Vector3<real>& v)
{
  return Vector3<real>(toRadians(v.x), toRadians(v.y), toRadians(v.z));
}

template <typename real>
inline Vector3<real>
toDegrees3(const Vector3<real>& v)
{
  Vector3<real> angles(toDegrees(v.x), toDegrees(v.y), toDegrees(v.z));

  if (angles.x < 0)
    angles.x = (real)360 + angles.x;
  if (angles.y < 0)
    angles.y = (real)360 + angles.y;
  if (angles.z < 0)
    angles.z = (real)360 + angles.z;
  return angles;
}

template <typename real>
inline Vector3<real>
cos3(const Vector3<real>& v)
{
  return Vector3<real>(cos(v.x), cos(v.y), cos(v.z));
}

template <typename real>
inline Vector3<real>
sin3(const Vector3<real>& v)
{
  return Vector3<real>(sin(v.x), sin(v.y), sin(v.z));
}


/////////////////////////////////////////////////////////////////////
//
// Quaternion: quaternion class
// ==========
template <typename real>
class Quaternion
{
public:
  typedef Quaternion<real> quat;
  typedef Vector3<real> vec3;
  typedef Matrix3x3<real> mat3;

  real x;
  real y;
  real z;
  real w;

  /// Default constructor.
  __host__ __device__
  Quaternion()
  {
    // do nothing
  }

  /// Constructs a Quaternion object from [(x, y, z), w].
  __host__ __device__
  Quaternion(real x, real y, real z, real w)
  {
    set(x, y, z, w);
  }

  /// Constructs a Quaternion object from q[4].
  __host__ __device__
  explicit Quaternion(const real q[])
  {
    set(q);
  }

  /// Constructs a Quaternion object from [(0, 0, 0), w].
  __host__ __device__
  explicit Quaternion(real w)
  {
    set(w);
  }

  /// Constructs a Quaternion object from [v, w].
  __host__ __device__
  explicit Quaternion(const vec3& v, real w = 0)
  {
    set(v, w);
  }

  /// Constructs a Quaternion object from angle (in degrees) and axis.
  __host__ __device__
  Quaternion(real angle, const vec3& axis)
  {
    set(angle, axis);
  }

  /// Constructs a Quaternion object from m.
  __host__ __device__
  explicit Quaternion(const mat3& m)
  {
    set(m);
  }

  /// Sets this object to q.
  __host__ __device__
  void set(const quat& q)
  {
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
  }

  /// Sets the coordinates of this object to [(x, y, z), w].
  __host__ __device__
  void set(real x, real y, real z, real w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to q[4].
  __host__ __device__
  void set(const real q[])
  {
    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }

  /// Sets the coordinates of this object to [(0, 0, 0), w].
  __host__ __device__
  void set(real w)
  {
    x = y = z = 0;
    this->w = w;
  }

  /// Sets the coordinates of this object to [v, w].
  __host__ __device__
  void set(const vec3& v, real w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  /// Sets the coordinates of this object from angle (in degress) and axis.
  __host__ __device__
  void set(real angle, const vec3& axis)
  {
    const real a = toRadians(angle) * real(0.5);
    const vec3 v = axis.versor() * sin(a);

    set(v, cos(a));
  }

  /// Sets the coordinates of this object from m.
  __host__ __device__
  void set(const mat3& m); // implemented in Matrix3x3.h

  /// Returns an identity quaternion.
  __host__ __device__
  static quat identity()
  {
    return quat((real)1);
  }

  /// \brief Returns a quaternion that rotates z radians around the
  /// z axis, x radians around the x axis, and y radians around the
  /// y axis (IN THAT ORDER).
  __host__ __device__
  static quat eulerAngles(real x, real y, real z)
  {
    return eulerAngles(vec3(x, y, z));
  }

  /// Returns a quaternion from Euler angles.
  __host__ __device__
  static quat eulerAngles(const vec3& angles)
  {
    const vec3 a = toRadians3(angles) * real(0.5);
    const vec3 c = cos3(a);
    const vec3 s = sin3(a);
    const real x = c.y * s.x * c.z + s.y * c.x * s.z;
    const real y = s.y * c.x * c.z - c.y * s.x * s.z;
    const real z = c.y * c.x * s.z - s.y * s.x * c.z;
    const real w = c.y * c.x * c.z + s.y * s.x * s.z;

    return quat(x, y, z, w);
  }

  /// Returns the Euler angles (in degress) from this object.
  __host__ __device__
  vec3 eulerAngles() const
  {
    const real sqx = sqr(x);
    const real sqy = sqr(y);
    const real sqz = sqr(z);
    const real sqw = sqr(w);
    const real one = sqx + sqy + sqz + sqw;
    const real eps = real(0.4995) * one;
    const real tol = x * w - y * z;
    vec3 angles;

    if (tol > eps)
    {
      angles.y = real(+2 * atan2(y, x));
      angles.x = real(+M_PI_2);
      angles.z = 0;
    }
    else if (tol < -eps)
    {
      angles.y = real(-2 * atan2(y, x));
      angles.x = real(-M_PI_2);
      angles.z = 0;
    }
    else
    {
      angles.y = real(atan2(2 * (y * w + x * z), sqw - sqx - sqy + sqz));
      angles.x = real(asin(2 * tol / one));
      angles.z = real(atan2(2 * (z * w + x * y), sqw - sqx + sqy - sqz));
    }
    return toDegrees3(angles);
  }

  /// Returns a quaternion from forward and up.
  __host__ __device__
  static quat lookAt(const vec3& forward, const vec3& up = vec3::up())
  {
    mat3 m;

    m[2] = forward.versor();
    m[0] = up.cross(forward).versor();
    m[1] = m[2].cross(m[0]);
    return quat(m);
  }

  /// Returns true if this object is equals to q.
  __host__ __device__
  bool equals(const quat& q, real eps = FloatInfo<real>::eps()) const
  {
    return Math::isNull<real>(x - q.x, y - q.y, z - q.z, w - q.w, eps);
  }

  bool operator ==(const quat& q) const
  {
    return equals(q);
  }

  bool operator !=(const quat& q) const
  {
    return !operator ==(q);
  }

  /// Returns a reference to this object += b.
  __host__ __device__
  quat& operator +=(const quat& b)
  {
    x += b.x;
    y += b.y;
    z += b.z;
    w += b.w;
    return *this;
  }

  /// Returns a reference to this object -= b.
  __host__ __device__
  quat& operator -=(const quat& b)
  {
    x -= b.x;
    y -= b.y;
    z -= b.z;
    w -= b.w;
    return *this;
  }

  /// Returns a reference to this object *= s.
  __host__ __device__
  quat& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /// Returns a reference to this object *= b.
  __host__ __device__
  quat& operator *=(const quat& b)
  {
    return *this = operator *(b);
  }

  /// Returns this object + b.
  __host__ __device__
  quat operator +(const quat& b) const
  {
    return quat(x + b.x, y + b.y, z + b.z, w + b.w);
  }

  /// Returns this object + b.
  __host__ __device__
  quat operator -(const quat& b) const
  {
    return quat(x - b.x, y - b.y, z - b.z, w - b.w);
  }

  /// Returns this object * s.
  __host__ __device__
  quat operator *(real s) const
  {
    return quat(x * s, y * s, z * s, w * s);
  }

  /// Returns this object * b.
  __host__ __device__
  quat operator *(const quat& b) const
  {
    const real cx = w * b.x + b.w * x + y * b.z - b.y * z;
    const real cy = w * b.y + b.w * y + z * b.x - b.z * x;
    const real cz = w * b.z + b.w * z + x * b.y - b.x * y;
    const real cw = w * b.w - b.x * x - y * b.y - b.z * z;

    return quat(cx, cy, cz, cw);
  }

  /// Returns this object * -1.
  quat operator -() const
  {
    return quat(-x, -y, -z, -w);
  }

  /// Returns the conjugate of this object.
  __host__ __device__
  quat operator ~() const
  {
    return quat(-x, -y, -z, +w);
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
    return real(sqrt(normSquared()));
  }

  /// Returns true if length of this object is close to unit.
  __host__ __device__
  bool isUnit(real eps = FloatInfo<real>::eps()) const
  {
    return Math::isEqual<real>(normSquared(), 1, eps);
  }

  /// Normalizes and returns a reference to this object.
  __host__ __device__
  quat& normalize(real eps = FloatInfo<real>::eps())
  {
    const real len = length();

    if (!Math::isZero<real>(len, eps))
      operator *=(Math::inverse<real>(len));
    return *this;
  }

  /// Negates and returns a reference to this object.
  __host__ __device__
  quat& negate()
  {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    return *this;
  }

  /// Inverts and returns a reference to this object.
  __host__ __device__
  quat& invert()
  {
    x = -x;
    y = -y;
    z = -z;
    return normalize();
  }

  /// Returns the conjugate of this object.
  __host__ __device__
  quat conjugate() const
  {
    return operator ~();
  }

  /// Returns the inverse of this object.
  __host__ __device__
  quat inverse() const
  {
    return conjugate().normalize();
  }

  /// Returns the point p rotated by this object.
  __host__ __device__
  vec3 rotate(const vec3& p) const
  {
    const real vx = real(2) * p.x;
    const real vy = real(2) * p.y;
    const real vz = real(2) * p.z;
    const real w2 = w * w - real(0.5);
    const real d2 = x * vx + y * vy + z * vz;
    const real px = x * d2 + w * (y * vz - z * vy) + vx * w2;
    const real py = y * d2 + w * (z * vx - x * vz) + vy * w2;
    const real pz = z * d2 + w * (x * vy - y * vx) + vz * w2;

    return vec3(px, py, pz);
  }

  /// Returns the point p rotated by the inverse of this object.
  __host__ __device__
  vec3 inverseRotate(const vec3& p) const
  {
    const real vx = real(2) * p.x;
    const real vy = real(2) * p.y;
    const real vz = real(2) * p.z;
    const real w2 = w * w - real(0.5);
    const real d2 = x * vx + y * vy + z * vz;
    const real px = x * d2 - w * (y * vz - z * vy) + vx * w2;
    const real py = y * d2 - w * (z * vx - x * vz) + vy * w2;
    const real pz = z * d2 - w * (x * vy - y * vx) + vz * w2;

    return vec3(px, py, pz);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s[<%f,%f,%f>,%f]\n", s, x, y, z, w);
  }

}; // Quaternion

/// Returns the scalar multiplication of s and q.
template <typename real>
__host__ __device__ inline Quaternion<real>
operator *(double s, const Quaternion<real>& q)
{
  return q * (real)s;
}

DS_END_NAMESPACE

/// Default quat type.
typedef DS_NAMESPACE::Quaternion<REAL> quat;

#endif // __Quaternion_h
