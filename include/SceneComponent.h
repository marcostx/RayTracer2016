#ifndef __SceneComponent_h
#define __SceneComponent_h

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
//  OVERVIEW: SceneComponent.h
//  ========
//  Class definition for scene component.

#include "NameableObject.h"

using namespace System;

namespace Graphics
{ // begin namespace Graphics

//
// Forward definition
//
class Scene;


//////////////////////////////////////////////////////////
//
// SceneComponent: scene component class
// ==============
class SceneComponent: public NameableObject
{
public:
  // Destructor
  virtual ~SceneComponent()
  {
    // do nothing
  }

  Scene* getScene() const
  {
    return scene;
  }

protected:
  Scene* scene;

  // Protected constructor
  SceneComponent():
    scene(0)
  {
    // do nothing
  }

  friend class Scene;

}; // SceneComponent

} // end namespace Graphics

#endif // __SceneComponent_h
