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
//  OVERVIEW: RayTracer.cpp
//  ========
//  Source file for simple ray tracer.

#include <map>
#include <time.h>
#include "BVH.h"
#include "RayTracer.h"
#include "Intersection.h"
#include "Model.h"

#define MAX_RAY_DEPTH 3 

using namespace std;
using namespace Graphics;

void
printElapsedTime(const char* s, clock_t time)
{
  printf("%sElapsed time: %.4f s\n", s, (REAL)time / CLOCKS_PER_SEC);
}


//////////////////////////////////////////////////////////
//
// RayTracer implementation
// =========
RayTracer::RayTracer(Scene& scene, Camera* camera):
  Renderer(scene, camera),
  maxRecursionLevel(6),
  minWeight(MIN_WEIGHT)
//[]---------------------------------------------------[]
//|  Constructor                                        |
//[]---------------------------------------------------[]
{
  // TODO: UNCOMMENT THE CODE BELOW
  
  int n = scene.getNumberOfActors();

  printf("Building aggregates for %d actors...\n", n);

  clock_t t = clock();
  Array<ModelPtr> models(n);
  map<uint, ModelPtr> aggregates;
  int totalNodes = 0;
  int i = 1;

  for (ActorIterator ait(scene.getActorIterator()); ait; i++)
  {
    const Actor* a = ait++;

    printf("Processing actor %d/%d...\r", i, n);
    if (!a->isVisible())
      continue;

    Primitive* p = dynamic_cast<Primitive*>(a->getModel());
    const TriangleMesh* mesh = p->triangleMesh();

    if (mesh != 0)
    {
      ModelPtr& a = aggregates[mesh->id];

      if (a == 0)
      {
        BVH* bvh = new BVH(std::move(p->refine()));

        totalNodes += bvh->size();
        a = bvh;
      }
      models.add(new ModelInstance(*a, *p));
    }
  }
  printf("Building scene aggregate...\n");
  {
    BVH* bvh = new BVH(std::move(models));

    totalNodes += bvh->size();
    aggregate = bvh;
  }
  printf("BVH(s) built: %d (%d nodes)\n", aggregates.size() + 1, totalNodes);
  printElapsedTime("", clock() - t);
}

//
// Auxiliary VRC
//
static vec3 VRC_u;
static vec3 VRC_v;
static vec3 VRC_n;

//
// Auxiliary mapping variables
//
static REAL V_h;
static REAL V_w;
static REAL I_h;
static REAL I_w;

void
RayTracer::render()
//[]---------------------------------------------------[]
//|  Render                                             |
//[]---------------------------------------------------[]
{
  System::warning("Invoke renderImage(image) to run the ray tracer\n");
}

static int64 numberOfRays;
static int64 numberOfHits;

void
RayTracer::renderImage(Image& image)
//[]---------------------------------------------------[]
//|  Run the ray tracer                                 |
//[]---------------------------------------------------[]
{
  clock_t t = clock();

  image.getSize(W, H);
  // init auxiliary VRC
  VRC_n = camera->getViewPlaneNormal();
  VRC_v = camera->getViewUp();
  VRC_u = VRC_v.cross(VRC_n);
  
  // init auxiliary mapping variables
  I_w = Math::inverse<REAL>(REAL(W));
  I_h = Math::inverse<REAL>(REAL(H));

  REAL height = camera->windowHeight();

  W >= H ? V_w = (V_h = height) * W * I_h : V_h = (V_w = height) * H * I_w;
  scan(image);
  printf("\nNumber of rays: %lu", numberOfRays);
  printf("\nNumber of hits: %lu", numberOfHits);
  printElapsedTime("\nDONE! ", clock() - t);
}

static Ray pixelRay;

inline vec3
VRC_point(REAL x, REAL y)
{
  return V_w * (x * I_w - 0.5f) * VRC_u + V_h * (y * I_h - 0.5f) * VRC_v;
}

void
RayTracer::setPixelRay(REAL x, REAL y)
//[]---------------------------------------------------[]
//|  Set pixel ray                                      |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//[]---------------------------------------------------[]
{
  vec3 p = VRC_point(x, y);

  switch (camera->getProjectionType())
  {
    case Camera::Perspective:
      pixelRay.direction = (p - camera->getDistance() * VRC_n).versor();
      break;

    case Camera::Parallel:
      pixelRay.origin = camera->getPosition() + p;
      break;
  }
}

void
RayTracer::scan(Image& image)
//[]---------------------------------------------------[]
//|  Basic scan with optional jitter                    |
//[]---------------------------------------------------[]
{
  // init pixel ray
  pixelRay = Ray(camera->getPosition(), -VRC_n);
  numberOfRays = numberOfHits = 0;

  Pixel* pixels = new Pixel[W];

  for (int j = 0; j < H; j++)
  {
    REAL y = j + 0.5f;

    printf("Scanning line %d of %d\r", j + 1, H);
    for (int i = 0; i < W; i++)
      pixels[i] = shoot(i + 0.5f, y);
    image.write(j, pixels);
  }
  delete []pixels;
}

Color
RayTracer::shoot(REAL x, REAL y)
//[]---------------------------------------------------[]
//|  Shoot a pixel ray                                  |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//|  @return RGB color of the pixel                     |
//[]---------------------------------------------------[]
{
  // set pixel ray
  setPixelRay(x, y);

  // trace pixel ray
  Color color = trace(pixelRay, 0, 1.0f);

  // adjust RGB color
  if (color.r > 1.0f)
    color.r = 1.0f;
  if (color.g > 1.0f)
    color.g = 1.0f;
  if (color.b > 1.0f)
    color.b = 1.0f;
  // return pixel color
  return color;
}

Color
RayTracer::trace(const Ray& ray, uint level, REAL weight)
//[]---------------------------------------------------[]
//|  Trace a ray                                        |
//|  @param the ray                                     |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color of the ray                           |
//[]---------------------------------------------------[]
{
  // TODO: dado um raio (direcao e origem), o nivel de recursao,
  // que nao sera sempre 0 e o peso, calcular a cor do raio
  // de pixel (cor do objeto no ponto Ri)
  // 

  // Variaveis
  ModelPtr object = NULL; 
  float minDist = INFINITY; 
  Intersection pHit; 
  Normal nHit;

  minDistance = 10000000

  /**
    pegando o objeto mais perto para traçar o raio
  **/
  for (int k = 0; k < objects.size(); ++k) { 
      if (objects[k].intersect(ray, &pHit)) { 
          
          float distance = pHit.distance; 
          // atualizando o objeto mais proximo
          if (distance < minDistance) { 
              object = objects[i]; 
              minDistance = distance; 
          } 
      } 
  } 

  // se nao tem ninguem na cena, retorna 0 
  if (object == NULL) 
      return 0; 

  // se o material tem reflexao ...
  // Obs.: ainda tem que arrumar
  if (object.getMateria().hasRefletion() && level < MAX_RAY_DEPTH) { 
      // compute reflection
      Ray reflectionRay; 
      reflectionRay = computeReflectionRay(ray.direction, nHit); 
      // recurse
      Color reflectionColor = trace(reflectionRay, level + 1,weight);
      return reflectionColor; 
  } 

  // O objeto eh opaco       
  // verificar se o raio de sombra intercepta alguem
  Ray shadowRay; 
  shadowRay.direction = lightPosition - pHit; 
  Intersection shadowRayIntersection;
  for (int k = 0; k < objects.size(); ++k) { 
      if (objects[k].intersect(shadowRay, shadowRayIntersection)) { 
          return 0; 
      } 
  }

  // se nao intercepta ninguem, l ilumina diretamente o objeto
  // TODO: calcular a cor do objeto no ponto de acordo com o modelo local de Phong 
  // 
  return object->color;
}
