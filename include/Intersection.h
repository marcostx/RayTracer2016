#ifndef __Intersection_h
#define __Intersection_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        GVSG Foundation Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|                Copyright® 2007-2016, Paulo Aristarco Pagliosa            |
//|                All Rights Reserved.                                      |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Intersection.h
//  ========
//  Class definition for intersection ray/object.

#include "Core/Flags.h"
#include "Geometry/Ray.h"

using namespace Ds;

namespace Graphics
{ // begin namespace Graphics

class Model;
class TriangleShape;


//////////////////////////////////////////////////////////
//
// Intersection: intersection ray/object class
// ============
struct Intersection
{
  const Model* object; // object intercepted by the ray
  const TriangleShape* triangle; // triangle intercepted by the ray
  REAL distance; // distance from the ray's origin to the intersection point
  vec3 p;  // barycentric coordinates of the intersection point
  Flags flags; // flags
  void* userData;   // any user data

}; // Intersection

} // end namespace Graphics

#endif // __Intersection_h
