#ifndef __Scene_h
#define __Scene_h

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
//  OVERVIEW: Scene.h
//  ========
//  Class definition for scene.

#include "Actor.h"
#include "Light.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Scene: scene class
// =====
class Scene: public NameableObject
{
public:
  Color backgroundColor;
  Color ambientLight;

  static Scene* New();	

  // Constructor
  Scene(const string& name):
    NameableObject(name),
    backgroundColor(Color::black),
    ambientLight(Color::gray),
    IOR(1)
  {
    modifiedBounds = false;
  }

  // Destructor
  virtual ~Scene();

  REAL getIOR() const
  {
    return IOR;
  }

  void setIOR(REAL ior)
  {
    IOR = ior > 0 ? ior : 0;
  }

  int getNumberOfActors() const
  {
    return actors.size();
  }

  ActorIterator getActorIterator() const
  {
    return ActorIterator(actors);
  }

  int getNumberOfLights() const
  {
    return lights.size();
  }

  LightIterator getLightIterator() const
  {
    return LightIterator(lights);
  }

  Actor* findActor(const string&) const;
  Light* findLight(const string&) const;

  void addActor(Actor*);
  void deleteActor(Actor*);
  void deleteActors();
  void addLight(Light*);
  void deleteLight(Light*);
  void deleteLights();

  void deleteAll()
  {
    deleteActors();
    deleteLights();
  }

  const Bounds3& boundingBox();

protected:
  bool modifiedBounds;
  Bounds3 bounds;
  REAL IOR;
  // Scene components
  Actors actors;
  Lights lights;

  void updateBounds();

}; // Scene

} // end namespace Graphics

#endif // __Scene_h
