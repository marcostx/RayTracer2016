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
//  OVERVIEW: Renderer.cpp
//  ========
//  Source file for generic renderer.

#include "Renderer.h"

//
// Canonical view volume
//
#define CVVX1 (REAL)-1.0
#define CVVX2 (REAL)+1.0
#define CVVY1 (REAL)-1.0
#define CVVY2 (REAL)+1.0

#define DFL_IMAGe_W 400
#define DFL_IMAGE_H 400

using namespace Graphics;


//////////////////////////////////////////////////////////
//
// Renderer implementation
// ========
Renderer::Renderer(Scene& aScene, Camera* aCamera):
  scene(&aScene),
  camera(aCamera != 0 ? aCamera : new Camera()),
  defaultLight(0)
//[]---------------------------------------------------[]
//|  Constructor                                        |
//[]---------------------------------------------------[]
{
  makeDefaultLight();
}

Renderer::~Renderer()
//[]---------------------------------------------------[]
//|  Destructor                                         |
//[]---------------------------------------------------[]
{
  delete defaultLight;
}

Light*
Renderer::makeDefaultLight()
//[]---------------------------------------------------[]
//|  Make default light                                 |
//[]---------------------------------------------------[]
{
  const vec3& p = camera->getPosition();

  if (defaultLight == 0)
  {
    defaultLight = new Light(p, Color::gray);
    System::makeUse(defaultLight);
  }
  //else
  //  defaultLight->position = p;
  return defaultLight;
}

void
Renderer::setScene(Scene& scene)
//[]---------------------------------------------------[]
//|  Set scene                                          |
//[]---------------------------------------------------[]
{
  if (&scene != this->scene)
    this->scene = &scene;
}

void
Renderer::setCamera(Camera* camera)
//[]---------------------------------------------------[]
//|  Set camera                                         |
//[]---------------------------------------------------[]
{
  if (camera != this->camera)
    this->camera = camera != 0 ? camera : new Camera();
}

void
Renderer::setImageSize(int w, int h)
//[]---------------------------------------------------[]
//|  Set imagem size                                    |
//[]---------------------------------------------------[]
{
  W = w;
  H = h;
}

void
Renderer::update()
//[]---------------------------------------------------[]
//|  Update                                             |
//[]---------------------------------------------------[]
{
  camera->updateView();
}

inline mat4
vpMatrix(const Camera* c)
{
  return c->getProjectionMatrix() * c->getWorldToCameraMatrix();
}

inline vec3
normalize(const vec4& p)
{
  return vec3(p) * (1 / p.w);
}

vec3
Renderer::project(const vec3& p) const
{
  vec3 w = normalize(vpMatrix(camera) * vec4(p, 1));

  w.x = (w.x * 0.5f + 0.5f) * W;
  w.y = (w.y * 0.5f + 0.5f) * H;
  w.z = (w.z * 0.5f + 0.5f);
  return w;
}

vec3
Renderer::unproject(const vec3& w) const
{
  vec4 p(w.x / W * 2 - 1, w.y / H * 2 - 1, w.z * 2 - 1, 1);
  mat4 m = vpMatrix(camera);

  m.invert();
  return normalize(m * p);
}
