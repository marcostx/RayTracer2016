//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2015-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: GLGismoDrawer.cpp
//  ========
//  Source file for GL gismo drawer.

#include "GLGismoDrawer.h"
#include "MeshSweeper.h"

using namespace Graphics;

template <typename T>
inline GLsizeiptr
sizeOf(int n)
{
  return sizeof(T)* n;
}


//////////////////////////////////////////////////////////
//
// GLVertexArray implementation
// =============
inline
GLVertexArray::GLVertexArray(const TriangleMesh* mesh)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(4, buffers);

  const TriangleMesh::Arrays& a = mesh->getData();

  if (GLsizeiptr s = sizeOf<vec3>(a.numberOfVertices))
  {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, s, a.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }
  if (GLsizeiptr s = sizeOf<vec3>(a.numberOfNormals))
  {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, s, a.normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
  }
  if (GLsizeiptr s = sizeOf<Color>(a.numberOfVertexColors))
  {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, s, a.vertexColors, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
  }
  else
    glVertexAttrib4f(2, 0, 0, 0, 0);
  if (GLsizeiptr s = sizeOf<TriangleMesh::Triangle>(a.numberOfTriangles))
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, a.triangles, GL_STATIC_DRAW);
  }
  count = 3 * a.numberOfTriangles;
}

GLVertexArray::~GLVertexArray()
{
  glDeleteBuffers(4, buffers);
  glDeleteVertexArrays(1, &vao);
}


//////////////////////////////////////////////////////////
//
// GLGismoDrawer implementation
// =============
#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec3 normal;
  uniform mat4 modelMatrix;
  uniform mat4 vpMatrix;
  uniform vec3 lightPosition;
  uniform vec4 lightColor = vec4(1, 1, 1, 1);
  uniform vec4 Od;
  uniform int flatMode;
  out vec4 vColor;

  void main()
  {
    vec4 P = modelMatrix * position;
    vec3 L = normalize(lightPosition - vec3(P));
    vec3 N = normalize(mat3(modelMatrix) * normal);

    vColor = Od * lightColor * max(dot(N, L), float(flatMode));
    gl_Position = vpMatrix * P;
  }
);

static const char* fragmentShader = STRINGIFY(
  in vec4 vColor;
  out vec4 fragmentColor;

  void main()
  {
    fragmentColor = vColor;
  }
);

ObjectPtr<TriangleMesh> GLGismoDrawer::circle;
ObjectPtr<TriangleMesh> GLGismoDrawer::cone;
ObjectPtr<TriangleMesh> GLGismoDrawer::cube;
ObjectPtr<TriangleMesh> GLGismoDrawer::sphere;

inline TriangleMesh*
makeSphere()
{
  return MeshSweeper::makeSphere();
}

inline TriangleMesh*
makeCircle()
{
  const int nt = 20;
  const int nv = nt + 1;
  TriangleMesh::Arrays a;

  a.vertices = new vec3[a.numberOfVertices = nv];
  a.normals = new vec3[a.numberOfNormals = nv];
  a.triangles = new TriangleMesh::Triangle[a.numberOfTriangles = nt];
  a.vertices[0].set(0, 0, 0), a.normals[0].set(0, 0, 1);
  if (true)
  {
    REAL c = cos(REAL(2 * M_PI) / nt);
    REAL s = sin(REAL(2 * M_PI) / nt);
    REAL x = 0;
    REAL y = 1;

    for (int i = 1; i < nv; i++)
    {
      a.vertices[i].set(x, y, 0), a.normals[i].set(0, 0, 1);

      REAL tx = x;
      REAL ty = y;
      
      x = c * tx - s * ty;
      y = s * tx + c * ty;
    }
  }

  TriangleMesh::Triangle* t = a.triangles;

  for (int i = 1; i < nv; i++, t++)
    t->setVertices(0, i, i % nt + 1);
  return new TriangleMesh(a);
}

inline TriangleMesh*
makeCone()
{
  return MeshSweeper::makeCone();
}

inline TriangleMesh*
makeCube()
{
  return MeshSweeper::makeCube();
}

inline void
GLGismoDrawer::initMeshes()
{
  if (circle == 0)
    circle = makeCircle();
  if (cone == 0)
    cone = makeCone();
  if (cube == 0)
    cube = makeCube();
  if (sphere == 0)
    sphere = makeSphere();
}

GLGismoDrawer::GLGismoDrawer():
  program("gismo drawer"),
  flatMode(0)
{
  program.addShader(GL_VERTEX_SHADER, GLSL::STRING, vertexShader);
  program.addShader(GL_FRAGMENT_SHADER, GLSL::STRING, fragmentShader);
  program.use();
  modelMatrixLoc = program.getUniformLocation("modelMatrix");
  vpMatrixLoc = program.getUniformLocation("vpMatrix");
  lightPositionLoc = program.getUniformLocation("lightPosition");
  flatModeLoc = program.getUniformLocation("flatMode");
  OdLoc = program.getUniformLocation("Od");
  initMeshes();
}

inline mat4
computeMvpMatrix(const mat4& m, Camera* c)
{
  return c->getProjectionMatrix() * m;
}

void
GLGismoDrawer::drawLine(const vec3& p1, const vec3& p2)
{
  vec4 points[2];
  
  points[0] = vpMatrix.transform(vec4(p1, 1));
  points[1] = vpMatrix.transform(vec4(p2, 1));
  GLPainter::drawLine(points);
}

inline void
GLGismoDrawer::drawPolyline(const vec3* v, int n, const mat4& m, bool closed)
{
  auto f = m.transform3x4(v[0]);
  auto p = f;

  for (int i = 1; i < n; i++)
  {
    auto q = m.transform3x4(v[i]);

    drawLine(p, q);
    p = q;
  }
  if (closed)
    drawLine(p, f);
}

void
GLGismoDrawer::drawCircle(
  const vec3& center,
  REAL radius,
  const vec3& normal,
  GLenum polygonMode)
{
  vec3 n = normal.versor();
  vec3 u = vec3::up().cross(n);

  u = (u.isNull() ? vec3(1, 0, 0).cross(n) : u).versor();

  mat4 m;

  m[0].set(u * radius);
  m[1].set(n.cross(u) * radius);
  m[2].set(n * radius);
  m[3].set(center, 1);
  if (polygonMode == GL_LINE)
  {
    const auto& c = circle->getData();
    drawPolyline(c.vertices + 1, c.numberOfVertices - 1, m, true);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    drawMesh(circle, m);
  }
}

void
GLGismoDrawer::drawArc(
  const vec3& center,
  REAL radius,
  const vec3& normal,
  const vec3& startPoint,
  REAL angle)
{
  const int ns = int(ceil((20 / (2 * M_PI)) * angle));
  mat4 m = mat4::rotation(normal, REAL(angle / ns), center);
  vec3 p = startPoint;

  for (int i = 1; i <= ns; i++)
  {
    vec3 q = m.transform3x4(p);

    drawLine(p, q);
    p = q;
  }
}

void
GLGismoDrawer::drawBoundingBox(const Bounds3& box)
{
  const vec3& p1 = box.getMin();
  const vec3& p7 = box.getMax();
  vec3 p2(p7.x, p1.y, p1.z);
  vec3 p3(p7.x, p7.y, p1.z);
  vec3 p4(p1.x, p7.y, p1.z);
  vec3 p5(p1.x, p1.y, p7.z);
  vec3 p6(p7.x, p1.y, p7.z);
  vec3 p8(p1.x, p7.y, p7.z);

  drawLine(p1, p2);
  drawLine(p2, p3);
  drawLine(p3, p4);
  drawLine(p1, p4);
  drawLine(p5, p6);
  drawLine(p6, p7);
  drawLine(p7, p8);
  drawLine(p5, p8);
  drawLine(p3, p7);
  drawLine(p2, p6);
  drawLine(p4, p8);
  drawLine(p1, p5);
}

GLSL::Program*
GLGismoDrawer::setupProgram(const mat4& m)
{
  GLSL::Program* cp = GLSL::Program::getCurrent();

  program.use();
  program.setUniform(modelMatrixLoc, m);
  program.setUniform(vpMatrixLoc, vpMatrix);
  program.setUniform(lightPositionLoc, lightPosition);
  program.setUniform(flatModeLoc, flatMode);
  program.setUniform(OdLoc, getLineColor());
  return cp;
}

void
GLGismoDrawer::drawMesh(TriangleMesh* mesh, const mat4& m)
{
  if (GLVertexArray* a = vertexArray(mesh))
  {
    using namespace GLSL;

    Program* cp = setupProgram(m);

    a->render();
    Program::setCurrent(cp);
  }
}

void
GLGismoDrawer::drawNormals(TriangleMesh* mesh, const mat4& m)
{
  const TriangleMesh::Arrays& a = mesh->getData();

  if (a.normals == 0)
    return;
  setLineColor(Color::white);
  for (int i = 0; i < a.numberOfVertices; i++)
  {
    const vec3 p = m.transform3x4(a.vertices[i]);
    const vec3 N = m.transformVector(a.normals[i]).versor();

    drawVector(p, N, 0.5);
  }
}

void
GLGismoDrawer::drawVector(const vec3& p, const vec3& d, REAL s)
{
  vec3 a;

  if (Math::isZero(d.x) && Math::isZero(d.z))
    a = d.y < 0 ? vec3(0, 0, 1) : vec3::up();
  else
    a.set(d.x, d.y + 1, d.z);

  const vec3 end = p + d * s;
  const mat4 m = mat4::TRS(end, quat(180, a), vec3(0.1f, 0.4f, 0.1f));

  drawLine(p, end);
  drawCone(m);
}

inline Color
royalBlue()
{
  return Color(65, 105, 255);
}

void
GLGismoDrawer::drawAxes(const vec3& p, const mat3& r, REAL s)
{
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  GLint pm[2];
  
  glGetIntegerv(GL_POLYGON_MODE, pm);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_DEPTH_TEST);
  setLineColor(Color::red);
  flatMode = 1;
  drawVector(p, r[0], s);
  setLineColor(Color::green);
  drawVector(p, r[1], s);
  setLineColor(royalBlue());
  drawVector(p, r[2], s);
  glPolygonMode(GL_FRONT_AND_BACK, pm[0]);
  dt ? glEnable(GL_DEPTH_TEST) : 0;
  flatMode = 0;
}

void
GLGismoDrawer::drawGround(REAL size, REAL step)
{
  setLineColor(Color(0.2f, 0.2f, 0.2f));
  for (float s = step; s <= size; s += step)
  {
    drawLine(vec3(-size, 0, +s), vec3(size, 0, +s));
    drawLine(vec3(-size, 0, -s), vec3(size, 0, -s));
    drawLine(vec3(+s, 0, -size), vec3(+s, 0, size));
    drawLine(vec3(-s, 0, -size), vec3(-s, 0, size));
  }
  setLineColor(Color::red);
  drawLine(vec3(-size, 0, 0), vec3(size, 0, 0));
  setLineColor(royalBlue());
  drawLine(vec3(0, 0, -size), vec3(0, 0, size));
}
