#ifndef __GLGismoDrawer_h
#define __GLGismoDrawer_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2014, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: GLGismoDrawer.h
//  ========
//  Class definition for GL gismo drawer.

#include "Camera.h"
#include "GLPainter.h"
#include "TriangleMeshShape.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// GLVertexArray: GL vertex array class
// =============
class GLVertexArray: public Object
{
public:
  // Contructor
  GLVertexArray(const TriangleMesh*);

  // Destructor
  ~GLVertexArray();

  // Render triangles
  void render()
  {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
  }

private:
  GLuint vao;
  GLuint buffers[4];
  GLsizei count;

}; // GLVertexArray

inline GLVertexArray*
getVertexArray(const TriangleMesh* mesh)
{
  return dynamic_cast<GLVertexArray*>((Object*)mesh->userData);
}

inline GLVertexArray*
vertexArray(TriangleMesh* mesh)
{
  GLVertexArray* a = getVertexArray(mesh);

  if (a == 0)
  {
    a = new GLVertexArray(mesh);
    mesh->userData = a;
  }
  return a;
}

inline mat4
computeVpMatrix(Camera* c)
{
  return c->getProjectionMatrix() * c->getWorldToCameraMatrix();
}


//////////////////////////////////////////////////////////
//
// GLGismoDrawer: GL gismo drawer class
// =============
class GLGismoDrawer: public GLPainter
{
public:
  // Constructor
  GLGismoDrawer();

  void drawLine(const vec3&, const vec3&);
  void drawCircle(const vec3&, REAL, const vec3&, GLenum = GL_LINE);
  void drawArc(const vec3&, REAL, const vec3&, const vec3&, REAL);
  void drawBoundingBox(const Bounds3&);
  void drawVector(const vec3&, const vec3&, REAL);
  void drawNormals(TriangleMesh*, const mat4&);
  void drawAxes(const vec3&, const mat3&, REAL = 1);
  void drawGround(REAL, REAL);

  void drawCube(const mat4& m = mat4::identity())
  {
    drawMesh(cube, m);
  }

  void drawSphere(const mat4& m = mat4::identity())
  {
    drawMesh(sphere, m);
  }

  void drawSphere(const vec3& c, REAL r = 1)
  {
    drawSphere(mat4::TRS(c, quat::identity(), vec3(r)));
  }

  void drawCone(const mat4& m = mat4::identity())
  {
    drawMesh(cone, m);
  }

  void update(Camera* camera)
  {
    vpMatrix = computeVpMatrix(camera);
    lightPosition = camera->getPosition();
  }

protected:
  GLSL::Program program;
  mat4f vpMatrix;
  vec4f lightPosition;
  GLint flatMode;
  GLint modelMatrixLoc;
  GLint vpMatrixLoc;
  GLint lightPositionLoc;
  GLint flatModeLoc;
  GLint OdLoc;

  void drawMesh(TriangleMesh*, const mat4&);
  void drawPolyline(const vec3*, int, const mat4&, bool = false);

private:
  static ObjectPtr<TriangleMesh> cube;
  static ObjectPtr<TriangleMesh> sphere;
  static ObjectPtr<TriangleMesh> cone;
  static ObjectPtr<TriangleMesh> circle;

  static void initMeshes();

  GLSL::Program* setupProgram(const mat4& = mat4::identity());

}; // GLGismoDrawer

} // end namespace Graphics

#endif // __GLGismoDrawer_h
