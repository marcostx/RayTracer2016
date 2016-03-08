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
//  OVERVIEW: Camera.cpp
//  ========
//  Source file for camera.

#include "Camera.h"
#include "Exception.h"

using namespace Graphics;

//
// Auxiiary function
//
inline void
error(const char* msg)
{
  throw Exception(msg);
}


//////////////////////////////////////////////////////////
//
// Camera implementation
// ======
uint Camera::nextId;

inline string
Camera::defaultName()
{
  char name[16];

  sprintf(name, "camera%d", ++nextId);
  return string(name);
}

inline void
Camera::updateFocalPoint()
{
  focalPoint = position + directionOfProjection * distance;
  viewModified = true;
}

inline void
Camera::updateDOP()
{
  directionOfProjection = (focalPoint - position) * (REAL)(1 / distance);
  viewModified = true;
}

Camera::Camera(
  ProjectionType projectionType,
  const vec3& position,
  const vec3& dop,
  const vec3& viewUp,
  REAL angle,
  REAL aspect):
  NameableObject(defaultName()),
  timestamp(0)
//[]---------------------------------------------------[]
//|  Constructor                                        |
//[]---------------------------------------------------[]
{
  this->projectionType = projectionType;
  this->position = position;
  distance = dop.length();
  if (distance < MIN_DISTANCE)
    distance = MIN_DISTANCE;
  directionOfProjection = dop * Math::inverse<REAL>(distance);
  focalPoint = position + dop;
  this->viewUp = viewUp;
  if (angle < MIN_ANGLE)
    angle = MIN_ANGLE;
  else if (angle > MAX_ANGLE)
    angle = MAX_ANGLE;
  viewAngle = angle;
  height = 2 * distance * (REAL)tan(Math::toRadians<REAL>(angle) * 0.5);
  aspectRatio = aspect < MIN_ASPECT ? MIN_ASPECT : aspect;
  F = (REAL)0.1;
  B = (REAL)1000.1;
  viewModified = true;
}

void
Camera::setPosition(const vec3& value)
//[]---------------------------------------------------[]
//|  Set the camera's position                          |
//|                                                     |
//|  Setting the camera's position will not change      |
//|  neither the direction of projection nor the        |
//|  distance between the position and the focal point. |
//|  The focal point will be moved along the direction  |
//|  of projection.                                     |
//[]---------------------------------------------------[]
{
  if (position != value)
  {
    position = value;
    updateFocalPoint();
  }
}

void
Camera::setDirectionOfProjection(const vec3& value)
//[]---------------------------------------------------[]
//|  Set the direction of projection                    |
//|                                                     |
//|  Setting the direction of projection will not       |
//|  change the distance between the position and the   |
//|  focal point. The focal point will be moved along   |
//|  the direction of projection.                       |
//[]---------------------------------------------------[]
{
  if (value.isNull())
    error("Direction of projection cannot be null");

  vec3 dop = value.versor();

  if (directionOfProjection != dop)
  {
    directionOfProjection = dop;
    updateFocalPoint();
  }
}

void
Camera::setViewUp(const vec3& value)
//[]---------------------------------------------------[]
//|  Set the camera's view up                           |
//[]---------------------------------------------------[]
{
  if (value.isNull())
    error("View up cannot be null");
  if (directionOfProjection.cross(value).isNull())
    error("View up cannot be parallel to DOP");

  vec3 vup = value.versor();

  if (viewUp != vup)
  {
    viewUp = vup;
    viewModified = true;
  }
}

void
Camera::setProjectionType(ProjectionType value)
//[]---------------------------------------------------[]
//|  Set the camera's projection type                   |
//[]---------------------------------------------------[]
{
  if (projectionType != value)
  {
    projectionType = value;
    viewModified = true;
  }
}

void
Camera::setDistance(REAL value)
//[]---------------------------------------------------[]
//|  Set the camera's distance                          |
//|                                                     |
//|  Setting the distance between the position and      |
//|  focal point will move the focal point along the    |
//|  direction of projection.                           |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Distance must be positive");
  if (!Math::isEqual(distance, value))
  {
    distance = dMax(value, MIN_DISTANCE);
    updateFocalPoint();
  }
}

void
Camera::setViewAngle(REAL value)
//[]---------------------------------------------------[]
//|  Set the camera's view angle                        |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("View angle must be positive");
  if (!Math::isEqual(viewAngle, value))
  {
    viewAngle = dMin(dMax(value, MIN_ANGLE), MAX_ANGLE);
    if (projectionType == Perspective)
      viewModified = true;
  }
}

void
Camera::setHeight(REAL value)
//[]---------------------------------------------------[]
//|  Set the camera's view height                       |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Height of the view window must be positive");
  if (!Math::isEqual(height, value))
  {
    height = dMax(value, MIN_HEIGHT);
    if (projectionType == Parallel)
      viewModified = true;
  }
}

void
Camera::setAspectRatio(REAL value)
//[]---------------------------------------------------[]
//|  Set the camera's aspect ratio                      |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Aspect ratio must be positive");
  if (!Math::isEqual(aspectRatio, value))
  {
    aspectRatio = dMax(value, MIN_ASPECT);
    viewModified = true;
  }
}

void
Camera::setClippingPlanes(REAL F, REAL B)
//[]---------------------------------------------------[]
//|  Set the distance of the clippling planes           |
//[]---------------------------------------------------[]
{
  if (F <= 0 || B <= 0)
    error("Clipping plane distance must be positive");
  if (F > B)
  {
    REAL temp = F;

    F = B;
    B = temp;
  }
  if (F < MIN_FRONT_PLANE)
    F = MIN_FRONT_PLANE;
  if ((B - F) < MIN_DEPTH)
    B = F + MIN_DEPTH;
  if (!Math::isEqual(this->F, F) || !Math::isEqual(this->B, B))
  {
    this->F = F;
    this->B = B;
    viewModified = true;
  }
}

void
Camera::setNearPlane(REAL F)
//[]---------------------------------------------------[]
//|  Set the distance of the near clipping plane        |
//[]---------------------------------------------------[]
{
  if (F > MIN_FRONT_PLANE && B - F > MIN_DEPTH && !Math::isEqual(this->F, F))
  {
    this->F = F;
    viewModified = true;
  }
}

void
Camera::azimuth(REAL angle)
//[]---------------------------------------------------[]
//|  Azimuth                                            |
//|                                                     |
//|  Rotate the camera's position about the view up     |
//|  vector centered at the focal point.                |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(angle))
  {
    mat4 r = mat4::rotation(viewUp, angle, focalPoint);

    position = r.transform3x4(position);
    updateDOP();
  }
}

void
Camera::elevation(REAL angle)
//[]---------------------------------------------------[]
//|  Elevation                                          |
//|                                                     |
//|  Rotate the camera's position about the cross       |
//|  product of the view plane normal and the view up   |
//|  vector centered at the focal point.                |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(angle))
  {
    vec3 axis = directionOfProjection.cross(viewUp);
    mat4 r = mat4::rotation(axis, angle, focalPoint);

    position = r.transform3x4(position);
    updateDOP();
    viewUp = axis.cross(directionOfProjection);
  }
}

void
Camera::rotateYX(REAL ay, REAL ax)
//[]---------------------------------------------------[]
//|  Rotate YX                                          |
//|                                                     |
//|  Composition of an azimuth of ay with an elevation  |
//|  of ax (in degrees).                                |
//[]---------------------------------------------------[]
{
  /*
  mat4 m = mat4::rotation(quat::eulerAngles(ax, ay, 0), focalPoint);

  position = m.transform3x4(position);
  updateDOP();
  viewUp = m.transformVector(viewUp);
  */
  vec3 y(0, 1, 0);
  quat q = quat(ax, vec3(1, 0, 0)) * quat(ay, y);
  mat4 m = inverseMatrix * mat4::rotation(q, vec3(0, 0, -distance));

  position = m.transform3x4(vec3::null());
  updateDOP();
  viewUp = m.transformVector(y);
}

void
Camera::roll(REAL angle)
//[]---------------------------------------------------[]
//|  Roll                                               |
//|                                                     |
//|  Rotate the view up vector around the view plane    |
//|  normal                                             |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(angle))
  {
    mat4 r = mat4::rotation(directionOfProjection, -angle, position);

    viewUp = r.transformVector(viewUp);
    viewModified = true;
  }
}

void
Camera::yaw(REAL angle)
//[]---------------------------------------------------[]
//|  Yaw                                                |
//|                                                     |
//|  Rotate the focal point about the view up vector    |
//|  centered at the camera's position.                 |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(angle))
  {
    mat4 r = mat4::rotation(viewUp, angle, position);

    focalPoint = r.transform3x4(focalPoint);
    updateDOP();
  }
}

void
Camera::pitch(REAL angle)
//[]---------------------------------------------------[]
//|  Pitch                                              |
//|                                                     |
//|  Rotate the focal point about the cross product of  |
//|  the view up vector and the view plane normal       |
//|  centered at the camera's position.                 |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(angle))
  {
    vec3 axis = directionOfProjection.cross(viewUp);
    mat4 r = mat4::rotation(axis, angle, position);

    focalPoint = r.transform3x4(focalPoint);
    updateDOP();
    viewUp = axis.cross(directionOfProjection);
  }
}

void
Camera::zoom(REAL zoom)
//[]---------------------------------------------------[]
//|  Zoom                                               |
//|                                                     |
//|  Change the view angle (or height) of the camera so |
//|  that more or less of a scene occupies the view     |
//|  window.  A value > 1 is a zoom-in. A value < 1 is  |
//|  zoom-out.                                          |
//[]---------------------------------------------------[]
{
  if (zoom > 0)
    if (projectionType == Perspective)
      setViewAngle(viewAngle / zoom);
    else
      setHeight(height / zoom);
}

void
Camera::move(REAL dx, REAL dy, REAL dz)
//[]---------------------------------------------------[]
//|  Move the camera                                    |
//[]---------------------------------------------------[]
{
  if (!Math::isZero(dx))
    position += directionOfProjection.cross(viewUp) * dx;
  if (!Math::isZero(dy))
    position += viewUp * dy;
  if (!Math::isZero(dz))
    position -= directionOfProjection * dz;
  updateFocalPoint();
}

void
Camera::setDefaultView()
//[]---------------------------------------------------[]
//|  Set default view                                   |
//[]---------------------------------------------------[]
{
  position.set(0, 0, 10);
  directionOfProjection.set(0, 0, -1);
  focalPoint.set(0, 0, 0);
  distance = 10;
  viewUp.set(0, 1, 0);
  projectionType = Perspective;
  viewAngle = 60;
  height = (REAL)(20 * M_SQRT3); // 2 * tan(viewAngle / 2) * distance
  aspectRatio = 1;
  F = (REAL)0.1;
  B = (REAL)1000.01;
  viewModified = true;
}

//
// Auxiliary struct
//
struct VRC
{
  vec3 O;
  vec3 u;
  vec3 v;
  vec3 n;

  VRC(const vec3& VRP, const vec3& VPN, const vec3& VUP)
  {
    O = VRP;
    n = VPN;
    u = VUP.cross(n).versor();
    v = VPN.cross(u);
  }

}; // VRC

inline vec3
vAxis(const vec3& DOP, const vec3& VUP)
{
  vec3 u = DOP.cross(VUP).versor();
  return u.cross(DOP);
}

uint
Camera::updateView()
//[]---------------------------------------------------[]
//|  Update matrix                                      |
//[]---------------------------------------------------[]
{
  if (viewModified)
  {
    if (projectionType == Parallel)
    {
      REAL t = height * REAL(0.5);
      REAL r = t * aspectRatio;

      projectionMatrix = mat4::ortho(-r, r, -t, t, F, B);
    }
    else
      projectionMatrix = mat4::perspective(viewAngle, aspectRatio, F, B);
    viewUp = vAxis(directionOfProjection, viewUp);
    matrix = mat4::lookAt(position, focalPoint, viewUp);
    matrix.inverse(inverseMatrix);
    viewModified = false;
    timestamp++;
  }
  return timestamp;
}

inline const char*
Camera::getProjectionName() const
//[]---------------------------------------------------[]
//|  Projection name                                    |
//[]---------------------------------------------------[]
{
  static const char* projectionName[] = { "Parallel", "Perspective" };
  return projectionName[projectionType];
}

void
Camera::print(FILE* f) const
//[]---------------------------------------------------[]
//|  Print camera                                       |
//[]---------------------------------------------------[]
{
  fprintf(f, "Camera name: \"%s\"\n", getName().c_str());
  fprintf(f, "Projection type: %s\n", getProjectionName());
  position.print("Position: ", f);
  directionOfProjection.print("Direction of projection: ", f);
  fprintf(f, "Distance: %f\n", distance);
  viewUp.print("View up: ", f);
  fprintf(f, "View angle/height: %f/%f\n", viewAngle, height);
}
