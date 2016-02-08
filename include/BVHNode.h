#ifndef __BVHNode_h
#define __BVHNode_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2010-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: BVHNode.h
//  ========
//  Class definition for BVH node.

#include "Geometry/Bounds3.h"
#include "Model.h"

using namespace Ds;
using namespace Graphics;


//////////////////////////////////////////////////////////
//
// BVHNode: BVH node class
// =======
class BVHNode: public Bounds3
{
public:
  __host__ __device__
  int lChild() const
  {
    return c1;
  }

  __host__ __device__
  int rChild() const
  {
    return c2;
  }

  __host__ __device__
  int begin() const
  {
    return -1 - c1;
  }

  __host__ __device__
  int end() const
  {
    return -1 - c2;
  }

  __host__ __device__
  void lChild(int32 i)
  {
    c1 = i;
  }

  __host__ __device__
  void rChild(int32 i)
  {
    c2 = i;
  }

  __host__ __device__
  void begin(int32 i)
  {
    c1 = -1 - i;
  }

  __host__ __device__
  void end(int32 i)
  {
    c2 = -1 - i;
  }

  bool intersect(const PreparedRay& r, REAL& d) const
  {
    return Bounds3::intersect(*this, r, d);
  }

private:
  int32 c1;
  int32 c2;

}; // BVHNode

inline __host__ __device__ REAL
intersectLeaf(
  BVHNode* leaf,
  const Array<ModelPtr>& models,
  const Ray& ray,
  Intersection& hit)
{
  for (int e = leaf->end(), i = leaf->begin(); i <= e; i++)
  {
    Intersection h;

    if (models[i]->intersect(ray, h) && h.distance < hit.distance)
      hit = h;
  }
  return hit.distance;
}

#define BVH_STACK_SIZE 30

inline __host__ __device__ bool
intersectBVH(
  BVHNode* bvh,
  const Array<ModelPtr>& models,
  const Ray& ray,
  Intersection& hit)
{
  Bounds3::PreparedRay r(ray);

  hit.distance = r.maxD;
  hit.object = 0;
  {
    REAL d;

    if (!bvh[0].intersect(r, d))
      return false;
  }

  int32 stack[BVH_STACK_SIZE];
  int32 top = 0;
  BVHNode* node = bvh;

  stack[top++] = -1;
  while (top != 0)
  {
    if (node->lChild() < 0)
    {
      Intersection h;

      h.distance = hit.distance;
      if (intersectLeaf(node, models, ray, h) < hit.distance)
        hit = h;
      node = bvh + stack[--top];
      continue;
    }

    do
    {
      REAL d1;
      REAL d2;
      int32 lChild = node->lChild();
      int32 rChild = node->rChild();
      bool inter1 = bvh[lChild].intersect(r, d1);
      bool inter2 = bvh[rChild].intersect(r, d2);

      if (inter1)
      {
        if (inter2)
        {
          if (d2 < d1)
            dSwap<int32>(lChild, rChild);
          stack[top++] = rChild;
        }
        node = bvh + lChild;
      }
      else if (inter2)
        node = bvh + rChild;
      else
        node = bvh + stack[--top];
    } while (top != 0 && node->lChild() >= 0);
  }
  return hit.object != 0;
}

#endif // __BVHNode_h
