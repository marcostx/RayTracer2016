#ifndef __MeshReader_h
#define __MeshReader_h

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
//  OVERVIEW: MeshReader.h
//  ========
//  Class definition for mesh reader.

#include "TriangleMeshShape.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// MeshReader: mesh reader class
// ==========
class MeshReader
{
public:
  TriangleMesh* execute(const char*);

}; // MeshReader

} // end namespace Graphics

#endif // __MeshReader_h
