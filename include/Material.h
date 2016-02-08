#ifndef __Material_h
#define __Material_h

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
// OVERVIEW: Material.h
// ========
// Class definition for material.

#include "Array.h"
#include "Graphics/Color.h"
#include "NameableObject.h"

using namespace Ds;
using namespace System;
using namespace System::Collections;

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Finish: finish class
// ======
class Finish
{
public:
  // Constructor
  Finish():
    ambient(0.2f),
    diffuse(0.8f),
    spot(0),
    shine(100)
  {
    // do nothing
  }

  float ambient;
  float diffuse;
  float spot;
  float shine;

}; // Finish


//////////////////////////////////////////////////////////
//
// Material: material class
// ========
class Material: public NameableObject
{
public:
  class Surface
  {
  public:
    Color ambient;      // ambient color
    Color diffuse;      // diffuse color
    Color spot;         // specular spot color
    float shine;        // specular spot exponent
    Color specular;     // specular color
    Color transparency; // transparency color
    float IOR;          // index of refraction

  }; // Surface

  Surface surface;
  float lineWidth;
  Color lineColor;

  uint getIndex() const
  {
    return index;
  }

  static Material* getDefault();

  void setSurface(const Color&, const Finish&);

protected:
  // Constructor
  Material(const string&, const Color&);

private:
  uint index;

  friend class MaterialFactory;

}; // Material

typedef PointerArrayIterator<Material> MaterialIterator;


//////////////////////////////////////////////////////////
//
// MaterialFactory: material factory class
// ===============
class MaterialFactory
{
public:
  static Material* New(const Color& = Color::white);
  static Material* New(const string&, const Color& = Color::white);

  static Material* get(const string&);

  static Material* get(uint id)
  {
    return materials[id];
  }

  static Material* getDefaultMaterial()
  {
    return materials.defaultMaterial;
  }

  static int size()
  {
    return materials.size();
  }

  static MaterialIterator iterator()
  {
    return MaterialIterator(materials);
  }

private:
  class Materials: public PointerArray<Material>
  {
  public:
    Material* defaultMaterial;

    // Constructor
    Materials()
    {
      defaultMaterial = MaterialFactory::New("default");
    }

  }; // Materials

  static Materials materials;

  static void add(Material* material)
  {
    material->index = materials.size();
    materials.add(makeUse(material));
  }

}; // MaterialFactory


//////////////////////////////////////////////////////////
//
// Material inline implementtaion
// ========
inline Material*
Material::getDefault()
{
  return MaterialFactory::getDefaultMaterial();
}

} // end namespace Graphics

#endif // __Material_h
