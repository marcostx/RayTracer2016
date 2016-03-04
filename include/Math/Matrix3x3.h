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
// OVERVIEW: Matrix3x3.h
// ========
// Class definition for 3x3 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 18/10/2014

#ifndef __Matrix3x3_h
#define __Matrix3x3_h

#include "Math/Quaternion.h"

DS_BEGIN_NAMESPACE

// Forward definition
template <typename real> class Matrix4x4;


/////////////////////////////////////////////////////////////////////
//
// Matrix3x3: 3x3 matrix class (column-major format)
// =========
template <typename real>
class Matrix3x3
{
public:
  typedef Quaternion<real> quat;
  typedef Vector3<real> vec3;
  typedef Matrix3x3<real> mat3;
  typedef Matrix4x4<real> mat4;

  /// Default constructor.
  __host__ __device__
  Matrix3x3()
  {
    // do nothing
  }

  /// Constructs a Matrix3x3 object from [v0; v1; v2].
  __host__ __device__
  Matrix3x3(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    set(v0, v1, v2);
  }

  /// Constructs a Matrix3x3 object from v[9].
  __host__ __device__
  explicit Matrix3x3(const real v[])
  {
    set(v);
  }

  /// Constructs a Matrix3x3 object as a multiple s of the identity matrix.
  __host__ __device__
  explicit Matrix3x3(real s)
  {
    set(s);
  }

  /// Constructs a Matrix3x3 object from the diagonal d.
  __host__ __device__
  explicit Matrix3x3(const vec3& d)
  {
    set(d);
  }

  /// Constructs a Matrix3x3 object from q.
  __host__ __device__
  explicit Matrix3x3(const quat& q)
  {
    set(q);
  }

  /// Constructs a Matrix3x3 object from m.
  __host__ __device__
  explicit Matrix3x3(const mat4& m)
  {
    set(m);
  }

  /// Sets this object to m.
  __host__ __device__
  void set(const mat3& m)
  {
    v0 = m.v0;
    v1 = m.v1;
    v2 = m.v2;
  }

  /// Sets the columns of this object to [v0; v1; v2].
  __host__ __device__
  void set(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
  }

  /// Sets the elements of this object from v[9].
  __host__ __device__
  void set(const real v[])
  {
    v0.set(&v[0]);
    v1.set(&v[3]);
    v2.set(&v[6]);
  }
    
  /// Sets this object to a multiple s of the identity matrix.
  __host__ __device__
  void set(real s)
  {
    v0.set(s, 0, 0);
    v1.set(0, s, 0);
    v2.set(0, 0, s);
  }

  /// Sets this object to a diagonal matrix d.
  __host__ __device__
  void set(const vec3& d)
  {
    v0.set(d.x, 0, 0);
    v1.set(0, d.y, 0);
    v2.set(0, 0, d.z);
  }

  /// Sets the elements of this object from q.
  __host__ __device__
  void set(const quat& q)
  {
    const real qx = q.x;
    const real qy = q.y;
    const real qz = q.z;
    const real qw = q.w;
    const real x2 = qx + qx;
    const real y2 = qy + qy;
    const real z2 = qz + qz;
    const real xx = qx * x2;
    const real yy = qy * y2;
    const real zz = qz * z2;
    const real xy = qy * x2;
    const real xz = qz * x2;
    const real xw = qw * x2;
    const real yz = qz * y2;
    const real yw = qw * y2;
    const real zw = qw * z2;

    v0.set((real)1 - (yy + zz), xy + zw, xz - yw);
    v1.set(xy - zw, (real)1 - (xx + zz), yz + xw);
    v2.set(xz + yw, yz - xw, (real)1 - (xx + yy));
  }

  /// Sets the elements of this object from m.
  __host__ __device__
  void set(const mat4& m); // implemented in Matrix4x4.h
  
  __host__ __device__
  mat3& operator =(const mat4& m)
  {
    set(m);
    return *this;
  }

  /// Returns a zero matrix.
  __host__ __device__
  static mat3 zero()
  {
    return mat3((real)0);
  }

  /// Returns an identity matrix.
  __host__ __device__
  static mat3 identity()
  {
    return mat3((real)1);
  }

  /// Returns a diagonal matrix d.
  __host__ __device__
  static mat3 diagonal(const vec3& d)
  {
    return mat3(d);
  }

  /// Returns the diagonal of this object.
  __host__ __device__
  vec3 diagonal() const
  {
    return vec3(v0.x, v1.y, v2.z);
  }

  /// Returns the trace of this object.
  __host__ __device__
  real trace() const
  {
    return v0.x + v1.y + v2.z;
  }

  /// Returns a reference to the j-th column of this object.
  __host__ __device__
  vec3& operator [](int j)
  {
    return (&v0)[j];
  }

  /// Returns the j-th column of this object.
  __host__ __device__
  const vec3& operator [](int j) const
  {
    return (&v0)[j];
  }

  /// Returns a reference to the element (i, j) of this object.
  __host__ __device__
  real& operator ()(int i, int j)
  {
    return (*this)[j][i];
  }

  /// Returns the element (i, j) of this object.
  __host__ __device__
  const real& operator ()(int i, int j) const
  {
    return (*this)[j][i];
  }

  /// Returns this object * s.
  __host__ __device__
  mat3 operator *(real s) const
  {
    return mat3(v0 * s, v1 * s, v2 * s);
  }

  /// Returns a reference to this object *= s.
  __host__ __device__
  mat3& operator *=(real s)
  {
    v0 *= s;
    v1 *= s;
    v2 *= s;
    return *this;
  }

  /// Returns this object * m.
  __host__ __device__
  mat3 operator *(const mat3& m) const
  {
    const vec3 b0 = transform(m.v0);
    const vec3 b1 = transform(m.v1);
    const vec3 b2 = transform(m.v2);

    return mat3(b0, b1, b2);
  }

  /// Returns a reference to this object *= m.
  __host__ __device__
  mat3& operator *=(const mat3& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  __host__ __device__
  vec3 operator *(const vec3& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  __host__ __device__
  mat3 transposed() const
  {
    const vec3 b0(v0.x, v1.x, v2.x);
    const vec3 b1(v0.y, v1.y, v2.y);
    const vec3 b2(v0.z, v1.z, v2.z);

    return mat3(b0, b1, b2);
  }

  /// Transposes and returns a reference to this object.
  __host__ __device__
  mat3& transpose()
  {
    return *this = transposed();
  }

  /// \brief Tries to invert this object and returns true on success;
  /// otherwise, leaves this object unchanged and returns false.
  __host__ __device__
  bool invert(real eps = FloatInfo<real>::eps())
  {
    const real b00 = v1[1] * v2[2] - v1[2] * v2[1];
    const real b01 = v0[2] * v2[1] - v0[1] * v2[2];
    const real b02 = v0[1] * v1[2] - v0[2] * v1[1];
    const real b10 = v1[2] * v2[0] - v1[0] * v2[2];
    const real b11 = v0[0] * v2[2] - v0[2] * v2[0];
    const real b12 = v0[2] * v1[0] - v0[0] * v1[2];
    const real b20 = v1[0] * v2[1] - v1[1] * v2[0];
    const real b21 = v0[1] * v2[0] - v0[0] * v2[1];
    const real b22 = v0[0] * v1[1] - v0[1] * v1[0];
    real d = v0[0] * b00 + v1[0] * b01 + v2[0] * b02;

    if (Math::isZero<real>(d, eps))
      return false;
    d = (real)1 / d;
    v0.set(d * b00, d * b01, d * b02);
    v1.set(d * b10, d * b11, d * b12);
    v2.set(d * b20, d * b21, d * b22);
    return true;
  }

  /// Assigns this object to m and tries to invert m.
  __host__ __device__
  bool inverse(mat3& m, real eps = FloatInfo<real>::eps()) const
  {
    return (m = *this).invert(eps);
  }

  /// Returns v transformed by this object.
  __host__ __device__
  vec3 transform(const vec3& v) const
  {
    return v0 * v.x + v1 * v.y + v2 * v.z;
  }

  /// Returns v transformed by the transposed of this object.
  __host__ __device__
  vec3 transposeTransform(const vec3& v) const
  {
    return vec3(v0.dot(v), v1.dot(v), v2.dot(v));
  }

  /// Returns a pointer to the elements of this object.
  __host__ __device__
  const real* data() const
  {
    return &v0.x;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    fprintf(f, "[%9.4f %9.4f %9.4f]\n", v0.x, v1.x, v2.x);
    fprintf(f, "[%9.4f %9.4f %9.4f]\n", v0.y, v1.y, v2.y);
    fprintf(f, "[%9.4f %9.4f %9.4f]\n", v0.z, v1.z, v2.z);
  }

private:
  vec3 v0; // column 0
  vec3 v1; // column 1
  vec3 v2; // column 2

}; // Matrix3x3

/// Sets the coordinates of this object from m.
template <typename real> 
__host__ __device__ inline void
Quaternion<real>::set(const Matrix3x3<real>& m) // declared in Quaternion.h
{
  real t = m.trace();

  if (t >= 0)
  {
    real s = sqrt(t + 1);

    w = real(0.5) * s;
    s = real(0.5) / s;
    x = (m(2, 1) - m(1, 2)) * s;
    y = (m(0, 2) - m(2, 0)) * s;
    z = (m(1, 0) - m(0, 1)) * s;
    return;
  }

  int i = 0;

  if (m(1, 1) > m(0, 0))
    i = 1;
  if (m(2, 2) > m(i, i))
    i = 2;
  if (i == 1)
  {
    real s = sqrt(m(1, 1) - (m(2, 2) + m(0, 0)) + 1);

    y = real(0.5) * s;
    s = real(0.5) / s;
    z = (m(1, 2) + m(2, 1)) * s;
    x = (m(0, 1) + m(1, 0)) * s;
    w = (m(0, 2) - m(2, 0)) * s;
  }
  else if (i == 2)
  {
    real s = sqrt(m(2, 2) - (m(0, 0) + m(1, 1)) + 1);

    z = real(0.5) * s;
    s = real(0.5) / s;
    x = (m(2, 0) + m(0, 2)) * s;
    y = (m(1, 2) + m(2, 1)) * s;
    w = (m(1, 0) - m(0, 1)) * s;
  }
  else
  {
    real s = sqrt(m(0, 0) - (m(1, 1) + m(2, 2)) + 1);

    x = real(0.5) * s;
    s = real(0.5) / s;
    y = (m(0, 1) + m(1, 0)) * s;
    z = (m(2, 0) + m(0, 2)) * s;
    w = (m(0, 1) - m(1, 2)) * s;
  }
}

/// Returns s * m.
template <typename real>
__host__ __device__ inline Matrix3x3<real>
operator *(double s, const Matrix3x3<real>& m)
{
  return m * (real)s;
}

DS_END_NAMESPACE

/// Default mat3 type.
typedef DS_NAMESPACE::Matrix3x3<REAL> mat3;

#endif // __Matrix3x3_h
