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
// OVERVIEW: Color.h
// ========
// Class definition RGB color.
//
// Author: Paulo Pagliosa
// Last revision: 18/10/2014

#ifndef __Color_h
#define __Color_h

#include "Math/Vector4.h"

DS_BEGIN_NAMESPACE


/////////////////////////////////////////////////////////////////////
//
// Color: RGB color class
// =====
class Color
{
public:
  float r;
  float g;
  float b;
  float a;

  /// Default constructor.
  __host__ __device__
  Color()
  {
    // do nothing
  }

  /// Constructs a Color object from (r, g, b, a).
  __host__ __device__
  explicit Color(float r, float g, float b, float a = 0)
  {
    setRGB(r, g, b, a);
  }

  /// Constructs a Color ibject from c[4].
  __host__ __device__
  explicit Color(float* c)
  {
    setRGB(c);
  }

  /// Constructs a Color object from (r, g, b).
  __host__ __device__
  explicit Color(int r, int g, int b)
  {
    setRGB(r, g, b);
  }

  /// Constructs a Color object from v.
  __host__ __device__
  explicit Color(const vec3& v)
  {
    setRGB(v);
  }

  __host__ __device__
  explicit Color(const vec4& v)
  {
    setRGB(v);
  }

  /// Sets this object from (r, g, b).
  __host__ __device__
  void setRGB(float r, float g, float b, float a = 0)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
  }

  /// Sets this object from c[4].
  __host__ __device__
  void setRGB(float* c)
  {
    r = c[0];
    g = c[1];
    b = c[2];
    a = c[3];
  }

  /// Sets this object from (r, g, b).
  __host__ __device__
  void setRGB(int r, int g, int b)
  {
    this->r = r * Math::inverse<float>(255);
    this->g = g * Math::inverse<float>(255);
    this->b = b * Math::inverse<float>(255);
    this->a = 0;
  }

  /// Sets this object from v.
  __host__ __device__
  void setRGB(const vec3& v)
  {
    setRGB(vec4(v));
  }

  __host__ __device__
  void setRGB(const vec4& v)
  {
    r = (float)v.x;
    g = (float)v.y;
    b = (float)v.z;
    a = (float)v.w;
  }

  __host__ __device__
  Color& operator =(const vec3& v)
  {
    return operator =(vec4(v));
  }

  __host__ __device__
  Color& operator =(const vec4& v)
  {
    setRGB(v);
    return *this;
  }

  /// Returns this object + c.
  __host__ __device__
  Color operator +(const Color& c) const
  {
    return Color(r + c.r, g + c.g, b + c.b);
  }

  /// Returns this object - c.
  __host__ __device__
  Color operator -(const Color& c) const
  {
    return Color(r - c.r, g - c.g, b - c.b);
  }

  /// Returns this object * c.
  __host__ __device__
  Color operator *(const Color& c) const
  {
    return Color(r * c.r, g * c.g, b * c.b);
  }

  /// Returns this object * s.
  __host__ __device__
  Color operator *(float s) const
  {
    return Color(r * s, g * s, b * s);
  }

  /// Returns the i-th component of this object.
  __host__ __device__
  const float& operator [](int i) const
  {
    return (&r)[i];
  }

  /// Returns a reference to the i-th component of this object.
  __host__ __device__
  float& operator [](int i)
  {
    return (&r)[i];
  }

  /// Returns a reference to this object += c.
  __host__ __device__
  Color& operator +=(const Color& c)
  {
    r += c.r;
    g += c.g;
    b += c.b;
    return *this;
  }

  /// Returns a reference to this object -= c.
  __host__ __device__
  Color& operator -=(const Color& c)
  {
    r -= c.r;
    g -= c.g;
    b -= c.b;
    return *this;
  }

  /// Returns a reference to this object *= c.
  __host__ __device__
  Color& operator *=(const Color& c)
  {
    r *= c.r;
    g *= c.g;
    b *= c.b;
    return *this;
  }

  /// Returns a reference to this object *= s.
  __host__ __device__
  Color& operator *=(float s)
  {
    r *= s;
    g *= s;
    b *= s;
    return *this;
  }

  /// Returns true if this object is equals to c.
  __host__ __device__
  bool operator ==(const Color& c) const
  {
    return Math::isNull<float>(r - c.r, g - c.g, b - c.b);
  }

  /// Returns true if this object is not equals to c.
  __host__ __device__
  bool operator !=(const Color& c) const
  {
    return !operator ==(c);
  }

  void print(const char* s) const
  {
    printf("%srgb(%f,%f,%f)\n", s, r, g, b);
  }

  static Color black;
  static Color red;
  static Color green;
  static Color blue;
  static Color cyan;
  static Color magenta;
  static Color yellow;
  static Color white;
  static Color darkGray;
  static Color gray;

  static Color HSV2RGB(float, float, float);

}; // Color

/// Returns the color s * c.
__host__ __device__ inline Color
operator *(double s, const Color& c)
{
  return c * (float)s;
}

DS_END_NAMESPACE

#endif // __Color_h
