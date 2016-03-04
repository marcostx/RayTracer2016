#ifndef __Camera_h
#define __Camera_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        GVSG Foundation Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Camera.h
//  ========
//  Class definition for camera.

#include "Math/Matrix4x4.h"
#include "NameableObject.h"

typedef unsigned int uint;

using namespace Ds;
using namespace System;

namespace Graphics
{ // begin namespace Graphics

#define MIN_HEIGHT      (REAL)0.1
#define MIN_ASPECT      (REAL)0.1
#define MIN_DISTANCE    (REAL)0.1
#define MIN_ANGLE       (REAL)1
#define MAX_ANGLE       (REAL)179
#define MIN_DEPTH       (REAL)0.1
#define MIN_FRONT_PLANE (REAL)0.1


//////////////////////////////////////////////////////////
//
// Camera: camera class
// ======
class Camera: public NameableObject
{
public:
  enum ProjectionType
  {
    Parallel,
    Perspective
  };

  // Constructors
  Camera();
  Camera(
    ProjectionType, // projection type
      const vec3&,  // position
      const vec3&,  // direction of projection
      const vec3&,  // view up vector
      REAL,         // vertical view angle
      REAL = 1.0f); // aspect ratio

  vec3 getPosition() const;
  vec3 getDirectionOfProjection() const;
  vec3 getFocalPoint() const;
  vec3 getViewUp() const;
  vec3 getViewPlaneNormal() const;
  ProjectionType getProjectionType() const;
  const char* getProjectionName() const;
  REAL getDistance() const;
  REAL getViewAngle() const;
  REAL getHeight() const;
  REAL getAspectRatio() const;
  void getClippingPlanes(REAL&, REAL&) const;
  REAL getNearPlane() const;
  REAL windowHeight() const;

  void setPosition(const vec3&);
  void setDirectionOfProjection(const vec3&);
  void setViewUp(const vec3&);
  void setProjectionType(ProjectionType);
  void setDistance(REAL);
  void setViewAngle(REAL);
  void setHeight(REAL);
  void setAspectRatio(REAL);
  void setClippingPlanes(REAL, REAL);
  void setNearPlane(REAL);

  void setDefaultView();
  uint updateView();
  void changeProjectionType();

  uint getTimestamp() const;
  bool isModified() const;

  void azimuth(REAL);
  void elevation(REAL);
  void rotateYX(REAL, REAL);
  void roll(REAL);
  void yaw(REAL);
  void pitch(REAL);
  void zoom(REAL);
  void move(REAL, REAL, REAL);
  void move(const vec3&);
  void moveNearPlane(REAL);

  mat4 getWorldToCameraMatrix() const;
  mat4 getCameraToWorldMatrix() const;
  mat4 getProjectionMatrix() const;

  vec3 worldToCamera(const vec3&) const;
  vec3 cameraToWorld(const vec3&) const;

  void print(FILE* = stdout) const;

protected:
  static uint nextId;

  vec3 position;
  vec3 directionOfProjection;
  vec3 focalPoint;
  vec3 viewUp;
  ProjectionType projectionType;
  REAL distance;
  REAL viewAngle;
  REAL height;
  REAL aspectRatio;
  REAL F;
  REAL B;
  bool viewModified;
  uint timestamp;
  mat4 matrix; // view matrix
  mat4 inverseMatrix;
  mat4 projectionMatrix;

  static string defaultName();

  void updateFocalPoint();
  void updateDOP();

  friend class Renderer;

}; // Camera


//////////////////////////////////////////////////////////
//
// Camera inline implementation
// ======
inline
Camera::Camera():
  NameableObject(defaultName()),
  timestamp(0)
{
  setDefaultView();
}

inline vec3
Camera::getPosition() const
{
  return position;
}

inline vec3
Camera::getDirectionOfProjection() const
{
  return directionOfProjection;
}

inline vec3
Camera::getFocalPoint() const
{
  return focalPoint;
}

inline vec3
Camera::getViewUp() const
{
  return viewUp;
}

inline vec3
Camera::getViewPlaneNormal() const
{
  return -directionOfProjection;
}

inline Camera::ProjectionType
Camera::getProjectionType() const
{
  return projectionType;
}

inline REAL
Camera::getDistance() const
{
  return distance;
}

inline REAL
Camera::getViewAngle() const
{
  return viewAngle;
}

inline REAL
Camera::getHeight() const
{
  return height;
}

inline REAL
Camera::getAspectRatio() const
{
  return aspectRatio;
}

inline void
Camera::getClippingPlanes(REAL& F, REAL& B) const
{
  F = this->F;
  B = this->B;
}

inline uint
Camera::getTimestamp() const
{
  return timestamp;
}

inline bool
Camera::isModified() const
{
  return viewModified;
}

inline void
Camera::move(const vec3& d)
{
  if (d.isNull())
    return;
  move(d.x, d.y, d.z);
}

inline mat4
Camera::getWorldToCameraMatrix() const
{
  return matrix;
}

inline mat4
Camera::getCameraToWorldMatrix() const
{
  return inverseMatrix;
}

inline mat4
Camera::getProjectionMatrix() const
{
  return projectionMatrix;
}

inline vec3
Camera::worldToCamera(const vec3& p) const
{
  return matrix.transform3x4(p);
}

inline vec3
Camera::cameraToWorld(const vec3& p) const
{
  return inverseMatrix.transform3x4(p);
}

inline REAL
Camera::windowHeight() const
{
  if (projectionType == Parallel)
    return height;
  return 2 * distance * REAL(tan(Math::toRadians<REAL>(viewAngle) * .5));
}

inline void
Camera::changeProjectionType()
{
  setProjectionType(projectionType == Parallel ? Perspective : Parallel);
}

inline REAL
Camera::getNearPlane() const
{
  return F;
}

inline void
Camera::moveNearPlane(REAL d)
{
  setNearPlane(F + d);
}

} // end namespace Graphics

#endif // __Camera_h
