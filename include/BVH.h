#ifndef __BVH_h
#define __BVH_h

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
//  OVERVIEW: BVH.h
//  ========
//  Class definition for BVH.

#include "BVHNode.h"
#include "TriangleMeshShape.h"


//////////////////////////////////////////////////////////
//
// BVH: BVH class
// ===
class BVH: public Aggregate
{
public:
  /// Constructs a BVH object from model array.
  BVH(Array<ModelPtr>&&);

  /// Destructor.
  ~BVH();

  int32 size() const
  {
    return numberOfNodes;
  }

  const BVHNode* getNodes() const
  {
    return nodes;
  }

  int32 getMaxLevel() const
  {
    return maxLevel;
  }

  void dump(const char* fileName) const
  {
    FILE* file = fopen(fileName, "w");

    dump(nodes, 0, file);
    fclose(file);
  }

  bool intersect(const Ray&, Intersection&) const;
  Bounds3 boundingBox() const;

protected:
  Array<ModelPtr> models;

private:
  BVHNode* nodes;
  int32 numberOfNodes;
  int32 maxLevel;

  void build(BVHNode&, int32, int32);
  void split(BVHNode&, int32);

  static void dump(const BVHNode*, int32, FILE* = stdout);

}; // BVH

#endif // __BVH_h
