#ifndef __GLRenderer_h
#define __GLRenderer_h

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
//  OVERVIEW: GLRenderer.h
//  ========
//  Class definition for GL renderer.

#include "GLGismoDrawer.h"
#include "Renderer.h"
#include "TriangleMesh.h"

#define MAX_LIGHTS 8

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// GLRenderer: GL renderer class
// ==========
class GLRenderer: public Renderer, public GLGismoDrawer
{
public:
  enum RenderMode
  {
    Wireframe = 1,
    HiddenLines = 2,
    Flat = 4,
    Smooth = 0
  };

  // Flags
  enum
  {
    UseLights = 1,
    DrawSceneBounds = 2,
    DrawActorBounds = 4,
    DrawGround = 8,
    DrawAxes = 16,
    DrawNormals = 32,
    UseVertexColors = 64
  };

  typedef void (*RenderFunc)(GLRenderer&);

  RenderMode renderMode;
  Flags flags;

  // Constructor
  GLRenderer(Scene&, Camera* = 0);

  void update();
  void render();

  using GLGismoDrawer::drawAxes;

  void drawMesh(const Model*);

  void setRenderFunc(RenderFunc f)
  {
    renderFunc = f;
  }

protected:
  RenderFunc renderFunc;

  virtual void startRender();
  virtual void endRender();
  virtual void renderActors();
  virtual void renderLights();

  void drawAxes(const mat4&);

private:
  struct LightLoc
  {
    GLint position;
    GLint color;
  };

  GLSL::Program program;
  mat4f viewportMatrix;
  GLint mvMatrixLoc;
  GLint normalMatrixLoc;
  GLint mvpMatrixLoc;
  GLint viewportMatrixLoc;
  GLint nbLightsLoc;
  LightLoc lightLocs[MAX_LIGHTS];
  GLint OaLoc;
  GLint OdLoc;
  GLint OsLoc;
  GLint nsLoc;
  GLint lineWidthLoc;
  GLint lineColorLoc;
  GLuint noMixIdx;
  GLuint lineColorMixIdx;
  GLuint modelMaterialIdx;
  GLuint colorMapMaterialIdx;

  void getUniformLocations();
  void getSubroutineIndices();
  void renderDefaultLights();

}; // GLRenderer

} // end namespace Graphics

#endif // __GLRenderer_h
