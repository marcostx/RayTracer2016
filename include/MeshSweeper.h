#ifndef __MeshSweeper_h
#define __MeshSweeper_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2010-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: MeshSweeper.h
//  ========
//  Class definition for mesh sweeper.

#include "Sweeper.h"
#include "TriangleMesh.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// MeshSweeper: mesh sweeper class
// ===========
class MeshSweeper: public Sweeper
{
public:
  // Make box
  static TriangleMesh* makeBox(
    const vec3& center,
    const quat& orientation,
    const vec3& scale);

  // Make cube
  static TriangleMesh* makeCube(
    const vec3& center,
    const quat& orientation,
    REAL scale)
  {
    return makeBox(center, orientation, vec3(scale, scale, scale));
  }

  static TriangleMesh* makeCube();

  // Make sphere
  static TriangleMesh* makeSphere(
    const vec3& center,
    REAL radius,
    int meridians = 16);

  static TriangleMesh* makeSphere()
  {
    return makeSphere(vec3::null(), 1);
  }

  // Make cylinder
  static TriangleMesh* makeCylinder(
    const vec3& center,
    REAL radius,
    const vec3& height,
    int segments = 16);

  static TriangleMesh* makeCylinder()
  {
    return makeCylinder(vec3::null(), 1, vec3::up());
  }

  // Make cone
  static TriangleMesh* makeCone(
    const vec3& baseCenter,
    REAL radius,
    const vec3& height,
    int segments = 16);

  static TriangleMesh* makeCone()
  {
    return makeCone(vec3::null(), 1, vec3::up());
  }

}; // MeshSweeper

} // end namespace Graphics

#endif // __MeshSweeper_h
