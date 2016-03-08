#ifndef __Light_h
#define __Light_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Light.h
//  ========
//  Class definition for light.

#include "Core/Flags.h"
#include "Graphics/Color.h"
#include "Math/Vector3.h"
#include "List.h"
#include "SceneComponent.h"

using namespace Ds;
using namespace System;
using namespace System::Collections;

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Light: light class
// =====
class Light: public SceneComponent
{
public:
  enum
  {
    Linear = 1,
    Squared = 2,
    Directional = 4,
    TurnedOn = 8
  };

  vec3 position;
  Color color;
  Flags flags;

  // Constructor
  Light(const vec3& p, const Color& c = Color::white):
    position(p),
    color(c),
    flags(TurnedOn)
  {
    // do nothing
  }

  bool isDirectional() const
  {
    return flags.isSet(Directional);
  }

  void setDirectional(bool state)
  {
    flags.enable(Directional, state);
  }

  bool isTurnedOn() const
  {
    return flags.isSet(TurnedOn);
  }

  void setSwitch(bool state)
  {
    flags.enable(TurnedOn, state);
  }

  Color getScaledColor(REAL) const;
  void lightVector(const vec3&, vec3&, REAL&) const;

private:
  DECLARE_LIST_ELEMENT(Light);

  friend class Scene;

}; // Light

typedef ListImp<Light> Lights;
typedef ListIteratorImp<Light> LightIterator;

__host__ __device__ inline Color
getLightScaledColor(const Color& color, Flags flags, REAL distance)
{
  if (flags.isSet(Light::Directional))
    return color;
  if (!flags.test(Light::Linear | Light::Squared))
    return color;

  REAL f = Math::inverse<REAL>(distance);

  if (flags.isSet(Light::Squared))
    f *= f;
  return color * f;
}

__host__ __device__ inline void
lightVector(
  const vec3& position,
  Flags flags,
  const vec3& P,
  vec3& L,
  REAL& distance)
{
  if (flags.isSet(Light::Directional))
  {
    L = -position.versor();
    distance = FloatInfo<REAL>::inf();
  }
  else if (!Math::isZero<REAL>(distance = (L = P - position).length()))
    L *= Math::inverse<REAL>(distance);
}


//////////////////////////////////////////////////////////
//
// Light inline implementation
// =====
inline Color
Light::getScaledColor(REAL distance) const
{
  return getLightScaledColor(color, flags, distance);
}

inline void
Light::lightVector(const vec3& P, vec3& L, REAL& distance) const
{
  return Graphics::lightVector(position, flags, P, L, distance);
}

} // end namespace Graphics

#endif // __Light_h
