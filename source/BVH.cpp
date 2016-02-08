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
//  OVERVIEW: BVH.cpp
//  ========
//  Source file for BVH.

#include "BVH.h"

inline void
inflate(Bounds3& b, const Model* m)
{
  b.inflate(m->boundingBox());
}

inline vec3
center(const Model* m)
{
  return m->boundingBox().center();
}


//////////////////////////////////////////////////////////
//
// BVH implementation
// ===
static const int32 binDim = 16;
static int32* bin;
static int32* nextBin;

BVH::BVH(Array<ModelPtr>&& m):
  models(std::move(m))
//[]---------------------------------------------------[]
//|  Constructor                                        |
//[]---------------------------------------------------[]
{
  maxLevel = -1;
  if (int32 n = models.size())
  {
    numberOfNodes = 1;
    nodes = new BVHNode[n << 1];
    build(nodes[0], 0, n - 1);
  }
  else
  {
    numberOfNodes = 0;
    nodes = 0;
  }
}

BVH::~BVH()
//[]---------------------------------------------------[]
//|  Destructor                                         |
//[]---------------------------------------------------[]
{
  delete nodes;
}

bool
BVH::intersect(const Ray& ray, Intersection& hit) const
//[]---------------------------------------------------[]
//|  Intersect                                          |
//[]---------------------------------------------------[]
{
  return nodes == 0 ? false : intersectBVH(nodes, models, ray, hit);
}

Bounds3
BVH::boundingBox() const
//[]---------------------------------------------------[]
//|  Bounding box                                       |
//[]---------------------------------------------------[]
{
  return nodes == 0 ? Bounds3() : *nodes;
}

void
BVH::build(BVHNode& node, int32 begin, int32 end)
//[]---------------------------------------------------[]
//|  Build                                              |
//[]---------------------------------------------------[]
{
  int32 n = end + begin + 1;

  node.begin(begin);
  node.end(end);
  bin = new int32[n << 1];
  nextBin = bin + n;
  split(node, 0);
  delete bin;
}

void
BVH::split(BVHNode& node, int32 level)
//[]---------------------------------------------------[]
//|  Split                                              |
//[]---------------------------------------------------[]
{
  int32 begin = node.begin();
  int32 end = node.end();
  int32 numberOfModels = end - begin + 1;

  for (int32 i = begin; i <= end; i++)
    inflate(node, models[i]);
  if (numberOfModels <= 8)
    return;
  if (maxLevel > 0 && level == maxLevel)
    return;

  vec3 size = node.size();
  REAL minCost = FloatInfo<REAL>::inf();
  REAL minFirstPlane;
  REAL k[3];
  int32 minCostPlane = binDim + 1;
  int32 minAxis;
  int32 splitPoint;

  for (int32 axis = 0; axis < 3; axis++)
  {
    REAL fPlane = node.getMin()[axis];
    REAL lPlane = node.getMax()[axis];

    if (Math::isZero(lPlane - fPlane))
      return;

    int32 listNodes = 0;
    int32 binsSize[binDim];
    int32 binsHead[binDim];
    int32 binsTail[binDim];

    for (int32 i = 0; i < binDim; i++)
    {
      binsSize[i] = 0;
      binsHead[i] = binsTail[i] = -1;
    }
    k[axis] = binDim * (1.0f - 1e-6f) / (lPlane - fPlane);

    for (int32 i = begin; i <= end; i++)
    {
      int32 bid = (int)(k[axis] * (center(models[i])[axis] - fPlane));

      if (bid >= binDim)
        bid = binDim - 1;
      try
      {
        PRECONDITION(bid >= 0 && bid < binDim);
      }
      catch (...)
      {
        printf("split BVH: invalid bin id: %d axis: %d\n", bid, axis);
        exit(0);
      }
      bin[listNodes] = i;
      nextBin[listNodes] = -1;
      if (binsHead[bid] == -1)
        binsHead[bid] = listNodes;
      else
        nextBin[binsTail[bid]] = listNodes;
      binsTail[bid] = listNodes;
      listNodes++;
    }

    REAL sap[binDim];
    REAL sas[binDim];
    REAL saInv = Math::inverse<REAL>(node.area());
    Bounds3 temp;

    for (int32 i = 0; i < binDim; i++)
    {
      int32 size = 0;

      for (int32 bid = binsHead[i]; bid != -1; bid = nextBin[bid], size++)
        inflate(temp, models[bin[bid]]);
      sap[i] = temp.area();
      binsSize[i] = size;
      if (i > 0)
        binsSize[i] += binsSize[i - 1];
    }
    temp.setEmpty();
    for (int32 i = binDim - 1; i >= 0; i--)
    {
      for (int32 bid = binsHead[i]; bid != -1; bid = nextBin[bid])
        inflate(temp, models[bin[bid]]);
      sas[i] = temp.area();
    }

    REAL minLocalCost = saInv * sap[binDim - 1] * numberOfModels;
    int32 minLocalCostPlane = -1;

    for (int32 i = 0; i < binDim; i++)
    {
      REAL cost = (sap[i] * binsSize[i] + sas[i] * (binsSize[binDim - 1] -
        binsSize[i])) * saInv;

      if (cost < minLocalCost)
      {
        minLocalCost = cost;
        minLocalCostPlane = i;
      }
    }
    if (minLocalCostPlane == -1)
    {
      /*
      if (numberOfModels <= 8)
        return;
      */
      int32 i = 0;

      while (i < binDim && binsSize[i] < binsSize[binDim - 1] - binsSize[i])
        i++;
      minLocalCostPlane = i;
    }
    if (minLocalCost < minCost)
    {
      minCost = minLocalCost;
      minCostPlane = minLocalCostPlane;
      minFirstPlane = fPlane;
      minAxis = axis;
      splitPoint = binsSize[minCostPlane] + begin - 1;
    }
  }

  int32 l = begin;
  int32 r = end;

  for (;;)
  {
    while (l < r &&
      (int)(k[minAxis] * (center(models[l])[minAxis] -
        minFirstPlane)) <= minCostPlane)
      l++;
    while (l < r &&
      (int)(k[minAxis] * (center(models[r])[minAxis] -
        minFirstPlane)) >  minCostPlane)
      r--;
    if (l == r)
      break;
    dSwap(models[l], models[r]);
  }
  node.lChild(numberOfNodes++);
  node.rChild(numberOfNodes++);

  int32 lChild = node.lChild();
  int32 rChild = node.rChild();

  nodes[lChild].begin(begin);
  nodes[lChild].end(splitPoint);
  nodes[rChild].begin(splitPoint + 1);
  nodes[rChild].end(end);
  level++;
  split(nodes[lChild], level);
  split(nodes[rChild], level);
}

void
BVH::dump(const BVHNode* bvh, int32 id, FILE* file)
{
  const BVHNode* node = bvh + id;
  const vec3& p1 = node->getMin();
  const vec3& p2 = node->getMax();

  fprintf(file,
    "Node %d [<%.2f,%.2f,%.2f>, <%.2f,%.2f,%.2f>] ",
    id, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
  if (node->lChild() < 0)
    fprintf(file, "begin: %d end: %d\n", node->begin(), node->end());
  else
  {
    int32 lChild = node->lChild();
    int32 rChild = node->rChild();

    fprintf(file, "lChild: %d rChild: %d\n", lChild, rChild);
    dump(bvh, lChild, file);
    dump(bvh, rChild, file);
  }
}
