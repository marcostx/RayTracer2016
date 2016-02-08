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
//  OVERVIEW: GLRenderer.cpp
//  ========
//  Source file for GL renderer.

#include "GLRenderer.h"
#include "MeshSweeper.h"

using namespace Graphics;


//////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec3 normal;
  layout(location = 2) in vec4 color;
  uniform mat4 mvMatrix;
  uniform mat3 normalMatrix;
  uniform mat4 mvpMatrix;
  out vec3 vPosition;
  out vec3 vNormal;
  out vec4 vColor;

  void main()
  {
    vPosition = vec3(mvMatrix * position);
    vNormal = normalize(normalMatrix * normal);
    gl_Position = mvpMatrix * position;
    vColor = color;
  }
);

static const char* geometryShader = STRINGIFY(
  layout(triangles) in;
  layout(triangle_strip, max_vertices = 3) out;

  in vec3 vPosition[];
  in vec3 vNormal[];
  in vec4 vColor[];
  uniform mat4 viewportMatrix;
  out vec3 gPosition;
  out vec3 gNormal;
  out vec4 gColor;
  noperspective out vec3 gEdgeDistance;

  void main()
  {
    // transform each vertex into viewport space
    vec2 p0 = vec2(viewportMatrix * 
      (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(viewportMatrix * 
      (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(viewportMatrix * 
      (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos((b * b + c * c - a * a) / (2 * b * c));
    float delta = acos((a * a + c * c - b * b) / (2 * a * c));
    float ha = abs(c * sin(delta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    gEdgeDistance = vec3(ha, 0, 0);
    gPosition = vPosition[0];
    gNormal = vNormal[0];
    gColor = vColor[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gEdgeDistance = vec3(0, hb, 0);
    gPosition = vPosition[1];
    gNormal = vNormal[1];
    gColor = vColor[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gEdgeDistance = vec3(0, 0, hc);
    gPosition = vPosition[2];
    gNormal = vNormal[2];
    gColor = vColor[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
  }
);

static const char* fragmentShader = STRINGIFY(
  struct LightInfo
  {
    vec4 position; // light position in eye coordinates
    vec4 color;    // light color
  };

  struct MaterialInfo
  {
    vec4 Oa; // ambient color
    vec4 Od; // diffuse color
    vec4 Os; // specular spot color
    float s; // specular shininess exponent
  };

  struct LineInfo
  {
    float width;
    vec4 color;
  };

  subroutine vec4 mixColorType(vec4 color);
  subroutine void matPropsType(out MaterialInfo m);

  in vec3 gPosition;
  in vec3 gNormal;
  in vec4 gColor;
  noperspective in vec3 gEdgeDistance;
  uniform int nbLights;
  uniform LightInfo lights[8];
  uniform MaterialInfo material;
  uniform LineInfo line;
  subroutine uniform mixColorType mixColor;
  subroutine uniform matPropsType matProps;
  layout(location = 0) out vec4 fragmentColor;

  subroutine(matPropsType)
  void modelMaterial(out MaterialInfo m)
  {
    m = material;
  }

  subroutine(matPropsType)
  void colorMapMaterial(out MaterialInfo m)
  {
    const float cmOa = 0.2;
    const float cmOd = 0.5;
    const float cmOs = 0.5;

    m = MaterialInfo(gColor * cmOa, gColor * cmOd, gColor * cmOs, 100);
  }

  vec4 phong(vec3 P, vec3 N)
  {
    vec4 color = vec4(0.0);

    for (int i = 0; i < nbLights; i++)
    {
      vec3 L = normalize(vec3(lights[i].position) - P);
      vec3 V = normalize(P.xyz);
      vec3 R = reflect(L, N);
      MaterialInfo m;
      
      matProps(m);
      color += lights[i].color * (m.Oa +
        m.Od * max(dot(L, N), 0) +
        m.Os * pow(max(dot(R, V), 0), m.s));
    }
    return color;
  }

  subroutine(mixColorType)
  vec4 noMix(vec4 color)
  {
    return color;
  }

  subroutine(mixColorType)
  vec4 lineColorMix(vec4 color)
  {
    // find the smallest distance
    float d = min(min(gEdgeDistance.x, gEdgeDistance.y), gEdgeDistance.z);
    float mixVal;

    if (d < line.width - 1)
      mixVal = 1;
    else if (d > line.width + 1)
      mixVal = 0;
    else
    {
      float x = d - (line.width - 1);
      mixVal = exp2(-2 * (x * x));
    }
    return mix(color, line.color, mixVal);
  }

  void main()
  {
    fragmentColor = mixColor(phong(gPosition, gNormal));
  }
);

inline void
GLRenderer::getUniformLocations()
{
  mvMatrixLoc = program.getUniformLocation("mvMatrix");
  normalMatrixLoc = program.getUniformLocation("normalMatrix");
  mvpMatrixLoc = program.getUniformLocation("mvpMatrix");
  viewportMatrixLoc = program.getUniformLocation("viewportMatrix");
  nbLightsLoc = program.getUniformLocation("nbLights");
  lightLocs[0].position = program.getUniformLocation("lights[0].position");
  lightLocs[0].color = program.getUniformLocation("lights[0].color");
  lightLocs[1].position = program.getUniformLocation("lights[1].position");
  lightLocs[1].color = program.getUniformLocation("lights[1].color");
  lightLocs[2].position = program.getUniformLocation("lights[2].position");
  lightLocs[2].color = program.getUniformLocation("lights[2].color");
  lightLocs[3].position = program.getUniformLocation("lights[3].position");
  lightLocs[3].color = program.getUniformLocation("lights[3].color");
  lightLocs[4].position = program.getUniformLocation("lights[4].position");
  lightLocs[4].color = program.getUniformLocation("lights[4].color");
  lightLocs[5].position = program.getUniformLocation("lights[5].position");
  lightLocs[5].color = program.getUniformLocation("lights[5].color");
  lightLocs[6].position = program.getUniformLocation("lights[6].position");
  lightLocs[6].color = program.getUniformLocation("lights[6].color");
  lightLocs[7].position = program.getUniformLocation("lights[7].position");
  lightLocs[7].color = program.getUniformLocation("lights[7].color");
  OaLoc = program.getUniformLocation("material.Oa");
  OdLoc = program.getUniformLocation("material.Od");
  OsLoc = program.getUniformLocation("material.Os");
  nsLoc = program.getUniformLocation("material.s");
  lineWidthLoc = program.getUniformLocation("line.width");
  lineColorLoc = program.getUniformLocation("line.color");
}

void
GLRenderer::getSubroutineIndices()
{
  noMixIdx = program.getFragmentSubroutineIndex("noMix");
  lineColorMixIdx = program.getFragmentSubroutineIndex("lineColorMix");
  modelMaterialIdx = program.getFragmentSubroutineIndex("modelMaterial");
  colorMapMaterialIdx = program.getFragmentSubroutineIndex("colorMapMaterial");
}

GLRenderer::GLRenderer(Scene& scene, Camera* camera):
  Renderer(scene, camera),
  renderMode(Smooth),
  program("renderer program"),
  renderFunc(nullptr)
{
  flags.set(UseLights | DrawGround);
  program.addShader(GL_GEOMETRY_SHADER, GLSL::STRING, geometryShader);
  program.addShader(GL_VERTEX_SHADER, GLSL::STRING, vertexShader);
  program.addShader(GL_FRAGMENT_SHADER, GLSL::STRING, fragmentShader);
  program.use();
  getUniformLocations();
  getSubroutineIndices();
  glEnable(GL_DEPTH_TEST);
}

void
GLRenderer::update()
{
  Renderer::update();
  GLGismoDrawer::update(camera);
  glViewport(0, 0, W, H);

  float w2 = W / 2.0f;
  float h2 = H / 2.0f;

  viewportMatrix[0].set(w2,  0, 0, 0);
  viewportMatrix[1].set( 0, h2, 0, 0);
  viewportMatrix[2].set( 0,  0, 1, 0);
  viewportMatrix[3].set(w2, h2, 0, 0);
}

inline void
GLRenderer::renderDefaultLights()
{
  program.setUniform(lightLocs[0].position, vec4(0, 0, 0, 1));
  program.setUniform(lightLocs[0].color, 1, 1, 1, 0);
  program.setUniform(nbLightsLoc, 1);
}

void
GLRenderer::renderLights()
{
  if (scene->getNumberOfLights() == 0)
  {
    renderDefaultLights();
    return;
  }

  const mat4& vm = camera->getWorldToCameraMatrix();
  int nl = 0;

  for (auto lit = scene->getLightIterator(); lit;)
  {
    const Light* light = lit++;

    if (light->isTurnedOn())
    {
      program.setUniform(lightLocs[nl].position, vm * vec4(light->position));
      program.setUniform(lightLocs[nl].color, light->color);
      if (++nl == MAX_LIGHTS)
        break;
    }
  }
  program.setUniform("nbLights", nl);
}

void
GLRenderer::renderActors()
{
  for (auto ait = scene->getActorIterator(); ait;)
  {
    const Actor* actor = ait++;

    if (!actor->isVisible())
      continue;
    drawMesh(actor->getModel());
  }
}

void
GLRenderer::startRender()
{
  update();

  const Color& bc = scene->backgroundColor;

  glClearColor((float)bc.r, (float)bc.g, (float)bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  program.use();
  program.setUniform(viewportMatrixLoc, viewportMatrix);
  glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);
}

void
GLRenderer::render()
{
  startRender();
  renderLights();
  renderActors();
  endRender();
}

void
GLRenderer::endRender()
{
  if (flags.isSet(DrawSceneBounds))
  {
    setLineColor(Color(0.2f, 0.2f, 0.2f));
    drawBoundingBox(scene->boundingBox());
  }
  if (flags.isSet(DrawGround))
  {
    REAL s = camera->windowHeight();

    s = dMin<REAL>(s, s * camera->getAspectRatio());
    drawGround(s, s / 20);
  }
  if (renderFunc != nullptr)
    renderFunc(*this);
  glFlush();
  program.disuse();
}

inline mat4
computeMvpMatrix(const mat4& m, Camera* c)
{
  return c->getProjectionMatrix() * m;
}

void
GLRenderer::drawMesh(const Model* model)
{
  TriangleMesh* mesh = (TriangleMesh*)model->triangleMesh();

  if (mesh == 0)
    return;
  if (GLVertexArray* a = vertexArray(mesh))
  {
    if (flags.isSet(DrawActorBounds))
    {
      setLineColor(Color(0.2f, 0.2f, 0.2f));
      drawBoundingBox(model->boundingBox());
    }

    const Material* m = model->getMaterial();
    const mat4& t = model->getLocalToWorldMatrix();
    mat4 mvMatrix = camera->getWorldToCameraMatrix() * t;

    program.setUniform(mvMatrixLoc, mvMatrix);
    program.setUniform(normalMatrixLoc, mat3(mvMatrix));
    program.setUniform(mvpMatrixLoc, computeMvpMatrix(mvMatrix, camera));
    program.setUniform(OaLoc, m->surface.ambient);
    program.setUniform(OdLoc, m->surface.diffuse);
    program.setUniform(OsLoc, m->surface.spot);
    program.setUniform(nsLoc, m->surface.shine);
    program.setUniform(lineWidthLoc, m->lineWidth);
    program.setUniform(lineColorLoc, m->lineColor);

    GLuint i[2];

    i[0] = renderMode == HiddenLines ? lineColorMixIdx : noMixIdx;
    i[1] = mesh->hasVertexColors() ? colorMapMaterialIdx : modelMaterialIdx;
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, i);
    a->render();
    if (flags.isSet(DrawNormals))
      drawNormals(mesh, t);
    if (flags.isSet(DrawAxes))
      drawAxes(t);
  }
}

void
GLRenderer::drawAxes(const mat4& m)
{
  mat3 r(m);

  r[0].normalize();
  r[1].normalize();
  r[2].normalize();
  GLGismoDrawer::drawAxes(vec3(m[3]), r);
}
