#ifndef __Actor_h
#define __Actor_h

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
//  OVERVIEW: Actor.h
//  ========
//  Class definition for actor.

#include "Core/Flags.h"
#include "List.h"
#include "Model.h"
#include "SceneComponent.h"

using namespace Ds;
using namespace System;
using namespace System::Collections;

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Actor: actor class
// =====
class Actor: public SceneComponent
{
public:
  enum
  {
    Visible = 1,
    Dynamic = 2
  };

  Flags flags;

  // Constructor
  Actor(Model& aModel):
    flags(Visible),
    model(&aModel)
  {
    // do nothing
  }

  bool isVisible() const
  {
    return flags.isSet(Visible);
  }

  void setVisible(bool state)
  {
    flags.enable(Visible, state);
  }

  bool isDynamic() const
  {
    return flags.isSet(Dynamic);
  }

  void setDynamic(bool state)
  {
    flags.enable(Dynamic, state);
  }

  Model* getModel() const
  {
    return model;
  }

  void setModel(Model& model)
  {
    this->model = &model;
  }

protected:
  ObjectPtr<Model> model;

  DECLARE_LIST_ELEMENT(Actor);

  friend class Scene;

}; // Actor

typedef ListImp<Actor> Actors;
typedef ListIteratorImp<Actor> ActorIterator;

} // end namespace Graphics

#endif // __Actor_h
