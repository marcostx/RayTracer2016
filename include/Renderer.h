#ifndef __Renderer_h
#define __Renderer_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Renderer.h
//  ========
//  Class definition for generic renderer.

#include "Camera.h"
#include "Scene.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Renderer: generic renderer class
// ========
class Renderer
{
  friend class RenderWindow;

public:
  // Constructors
  Renderer()
  {
    // do nothing
  }

  Renderer(Scene&, Camera* = 0);

  // Destructor
  virtual ~Renderer();

  Scene* getScene() const
  {
    return scene;
  }

  Camera* getCamera() const
  {
    return camera;
  }

  void getImageSize(int& w, int &h) const
  {
    w = W;
    h = H;
  }

  void setScene(Scene&);
  void setCamera(Camera*);
  void setImageSize(int, int);

  vec3 project(const vec3&) const;
  vec3 unproject(const vec3&) const;

  virtual void update();
  virtual void render() = 0;

protected:
  ObjectPtr<Scene> scene;
  ObjectPtr<Camera> camera;
  Light* defaultLight;
  int W;
  int H;

  Light* makeDefaultLight();

}; // Renderer

} // end namespace Graphics

#endif // __Renderer_h
