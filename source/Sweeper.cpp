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
//  OVERVIEW: Sweeper.cpp
//  ========
//  Source file for generic sweeper.

#include "Sweeper.h"

using namespace Graphics;


//////////////////////////////////////////////////////////
//
// Sweeper::Polyline implementation
// =================
void
Sweeper::Polyline::transform(const mat4& m)
//[]----------------------------------------------------[]
//|  Transform                                           |
//[]----------------------------------------------------[]
{
  for (VertexIterator vit(getVertexIterator()); vit;)
    (vit++).transform(m);
}

vec3
Sweeper::Polyline::normal() const
//[]----------------------------------------------------[]
//|  Normal                                              |
//[]----------------------------------------------------[]
{
  vec3 N(0, 0, 0);
  VertexIterator vit(getVertexIterator());

  if (vit)
  {
    vec3* first = &(vit++).position;
    vec3* p = first;

    for (vec3* q = 0; q != first;)
    {
      q = vit ? &(vit++).position : first;
      N.x += (p->y - q->y) * (p->z + q->z);
      N.y += (p->z - q->z) * (p->x + q->x);
      N.z += (p->x - q->x) * (p->y + q->y);
      p = q;
    }
  }
  return N;
}

//
// Auxiliary function
//
vec3
getFirstPoint(const vec3& center, REAL radius, const vec3& normal)
{
  vec3 p;

  if (fabs(normal.z) > M_SQRT2 * 0.5)
  {
      // Choose p in y-z plane
    REAL s = normal.y * normal.y + normal.z * normal.z;

    s = Math::inverse<REAL>(sqrt(s));
    p.set(0, -normal.z * s, normal.y * s);
  }
  else
  {
    // Choose p in x-y plane
    REAL s = normal.x * normal.x + normal.y * normal.y;

    s = Math::inverse<REAL>(sqrt(s));
    p.set(-normal.y * s, normal.x * s, 0);
  }
  return center + radius * p;
}


//////////////////////////////////////////////////////////
//
// Sweeper implementation
// =======
Sweeper::Polyline
Sweeper::makeArc(
  const vec3& center,
  REAL radius,
  const vec3& normal,
  REAL angle,
  int segments)
//[]----------------------------------------------------[]
//|  Make arc                                            |
//[]----------------------------------------------------[]
{
  Sweeper::Polyline poly;
  mat4 m = mat4::rotation(normal, REAL(angle / segments), center);
  vec3 p = getFirstPoint(center, radius, normal);

  poly.mv(p);
  for (int i = 1; i <= segments; i++)
  {
    p = m.transform3x4(p);
    poly.mv(p);
  }
  return poly;
}

Sweeper::Polyline
Sweeper::makeCircle(
  const vec3& center,
  REAL radius,
  const vec3& normal,
  int points)
//[]----------------------------------------------------[]
//|  Make circle                                         |
//[]----------------------------------------------------[]
{
  Sweeper::Polyline poly;
  mat4 m = mat4::rotation(normal, REAL(360) / points, center);
  vec3 p = getFirstPoint(center, radius, normal);

  poly.mv(p);
  for (int i = 1; i < points; i++)
  {
    p = m.transform3x4(p);
    poly.mv(p);
  }
  poly.close();
  return poly;
}
