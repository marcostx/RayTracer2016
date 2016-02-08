#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLRenderer.h"
#include "MeshReader.h"

#define WIN_W 1024
#define WIN_H 768

using namespace Graphics;

// Render globals
GLSL::Program* program;
void (*renderFunc)();
int W;
int H;

// Camera globals
Camera* camera;
const float CAMERA_RES = 0.01f;
const float ZOOM_SCALE = 1.01f;

// Mouse globals
int mouseX;
int mouseY;

// Keyboard globals
const int MAX_KEYS = 256;
bool keys[MAX_KEYS];

// Animation globals
bool animateFlag;
const int UPDATE_RATE = 40;

extern void processKeys();

void
initGL(int *argc, char **argv)
{
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WIN_W, WIN_H);
  glutCreateWindow("GL Test");
  GLSL::init();
  glutReportErrors();
}

void
displayCallback()
{
  processKeys();
  if (renderFunc != nullptr)
    renderFunc();
  glutSwapBuffers();
}

void
reshapeCallback(int w, int h)
{
  glViewport(0, 0, w, h);
}

void
mouseCallback(int, int, int x, int y)
{
  mouseX = x;
  mouseY = y;
}

void
motionCallback(int x, int y)
{
  const float da = camera->getViewAngle() * CAMERA_RES;
  const float ay = (mouseX - x) * da * !keys['x'];
  const float ax = (mouseY - y) * da * !keys['y'];

  mouseX = x;
  mouseY = y;
  if (ax != 0 || ay != 0)
  {
    keys['r'] ? camera->roll(ay) : camera->rotateYX(ay, ax);
    glutPostRedisplay();
  }
}

void
idleCallback()
{
  static GLint currentTime;
  GLint time = glutGet(GLUT_ELAPSED_TIME);

  if (abs(time - currentTime) >= UPDATE_RATE)
  {
    // TODO
    glutPostRedisplay();
  }
}

void
keyboardCallback(unsigned char key, int /*x*/, int /*y*/)
{
  keys[key] = true;
  glutPostRedisplay();
}

void
keyboardUpCallback(unsigned char key, int /*x*/, int /*y*/)
{
  keys[key] = false;
  switch (key)
  {
    case 27:
      exit(EXIT_SUCCESS);
      break;
    case 'o':
      animateFlag ^= true;
      glutIdleFunc(animateFlag ? idleCallback : 0);
      glutPostRedisplay();
      break;
  }
}

int
main(int argc, char **argv)
{
  // init OpenGL
  initGL(&argc, argv);
  glutDisplayFunc(displayCallback);
  glutReshapeFunc(reshapeCallback);
  glutMouseFunc(mouseCallback);
  glutMotionFunc(motionCallback);
  glutKeyboardFunc(keyboardCallback);
  glutKeyboardUpFunc(keyboardUpCallback);
  camera = new Camera();
  glutMainLoop();
  return 0;
}

inline void
clearScreen(const Color& c = Color::black)
{
  // glClearBufferfv(GL_COLOR, 0, &c[0]);
  glClearColor(c.r, c.g, c.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool
makeProgram(const char* name, const char* vs, const char* fs)
{
  if (program != nullptr && !strcmp(program->getName(), name))
    return false;
  delete program;
  printf("Making program '%s'\n", name);
  program = new GLSL::Program(name);
  program->addShader(GL_VERTEX_SHADER, GLSL::STRING, vs);
  program->addShader(GL_FRAGMENT_SHADER, GLSL::STRING, fs);
  program->use();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  return true;
}

#define STRINGIFY(A) "#version 330\n"#A

void
test1()
{
  static const char* vs = STRINGIFY(
    out vec4 vColor;

    void main()
    {
      gl_Position = vec4(0, 0, 0, 1);
      vColor = vec4(0, 1, 1, 1);
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );

  makeProgram("test1", vs, fs);
  clearScreen(Color::red);
  glPointSize(40);
  glDrawArrays(GL_POINTS, 0, 1);
}

void
test2()
{
  static const char* vs = STRINGIFY(
    uniform vec4 v[] = vec4[5](
      vec4(-1, -1, 0, 1),
      vec4(+1, -1, 0, 1),
      vec4(0, 0, 0, 1),
      vec4(-1, +1, 0, 1),
      vec4(+1, +1, 0, 1));
    uniform vec4 c[] = vec4[5](
      vec4(1, 1, 0, 1),
      vec4(0, 1, 0, 1),
      vec4(0, 1, 1, 1),
      vec4(1, 1, 0, 1),
      vec4(0, 1, 0, 1));

    out vec4 vColor;

    void main()
    {
      gl_Position = v[gl_VertexID];
      vColor = c[gl_VertexID];
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );

  makeProgram("test2", vs, fs);
  clearScreen(Color::red);
  glPointSize(3);
  glDrawArrays(GL_POINTS, 0, 5);
}

void
test3()
{
  static const char* vs = STRINGIFY(
    uniform vec4 v[] = vec4[8](
      vec4(-1, -1, 0, 1),
      vec4(-1, +1, 0, 1),
      vec4(-1, +1, 0, 1),
      vec4(+1, +1, 0, 1),
      vec4(+1, +1, 0, 1),
      vec4(+1, -1, 0, 1),
      vec4(+1, -1, 0, 1),
      vec4(-1, -1, 0, 1));
    uniform vec4 c[] = vec4[4](
      vec4(0, 1, 0, 1),
      vec4(1, 0, 0, 1),
      vec4(1, 1, 0, 1),
      vec4(0, 1, 1, 0));

    out vec4 vColor;

    void main()
    {
      gl_Position = v[gl_VertexID];
      vColor = c[gl_VertexID / 2];
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );

  makeProgram("test3", vs, fs);
  glLineWidth(1);
  clearScreen(Color::black);
  glDrawArrays(GL_LINES, 0, 8);
}

void
test4()
{
  static const char* vs = STRINGIFY(
    uniform vec4 v[] = vec4[6](
      vec4(-1, -1, 0, 1),
      vec4(0, 0, 0, 1),
      vec4(-1, +1, 0, 1),
      vec4(+1, +1, 0, 1),
      vec4(0, 0, 0, 1),
      vec4(+1, -1, 0, 1));
    uniform vec4 c[] = vec4[2](
      vec4(0, 1, 0, 1),
      vec4(1, 1, 0, 1));

    out vec4 vColor;

    void main()
    {
      gl_Position = v[gl_VertexID];
      vColor = c[gl_VertexID / 3];
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );

  makeProgram("test4", vs, fs);
  glLineWidth(1);
  clearScreen(Color::black);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void
test5()
{
  static const char* vs = STRINGIFY(
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec4 color;
    out vec4 vColor;

    void main()
    {
      gl_Position = position;
      vColor = color;
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );
  static const vec4 v[6] =
  {
    vec4(-1, -1, 0, 1),
    vec4(0, 0, 0, 1),
    vec4(-1, +1, 0, 1),
    vec4(+1, +1, 0, 1),
    vec4(0, 0, 0, 1),
    vec4(+1, -1, 0, 1)
  };
  static const vec4 c[6] =
  {
    vec4(1, 0, 0, 1),
    vec4(1, 0, 0, 1),
    vec4(1, 0, 0, 1),
    vec4(0, 0, 1, 1),
    vec4(0, 0, 1, 1),
    vec4(0, 0, 1, 1)
  };

  makeProgram("test5", vs, fs);

  GLuint vao;
  GLuint buffers[2];

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(2, buffers);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(c), c, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);
  glLineWidth(1);
  clearScreen(Color::black);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(1, &vao);
}

void
test6()
{
  static const char* vs = STRINGIFY(
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec3 normal;
    uniform vec4 Od = vec4(0.85f, 0.85f, 0.10f, 1);
    uniform mat4 vMatrix;
    uniform mat4 pMatrix;
    out vec4 vColor;

    void main()
    {
      vec4 P = vMatrix * position;
      vec3 L = normalize(vec3(P) - vec3(1, 1, 1));
      vec3 N = normalize(mat3(vMatrix) * normal);

      gl_Position = pMatrix * P;
      vColor = Od * max(dot(-N, L), 0);
    }
  );
  static const char* fs = STRINGIFY(
    in vec4 vColor;
    out vec4 fColor;

    void main()
    {
      fColor = vColor;
    }
  );
  static TriangleMesh* mesh = MeshReader().execute("f-16.obj");

  if (mesh == nullptr)
  {
    puts("Unable to read mesh file");
    return;
  }

  static GLint vMatrixLoc;
  static GLint pMatrixLoc;

  if (makeProgram("test6", vs, fs))
  {
    vMatrixLoc = program->getUniformLocation("vMatrix");
    pMatrixLoc = program->getUniformLocation("pMatrix");
    glEnable(GL_DEPTH_TEST);
  }

  GLuint vao;
  GLuint buffers[3];
  GLsizeiptr s;
  const TriangleMesh::Arrays& a = mesh->getData();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(3, buffers);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  s = sizeof(vec3) * a.numberOfVertices;
  glBufferData(GL_ARRAY_BUFFER, s, a.vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  s = sizeof(vec3) * a.numberOfNormals;
  glBufferData(GL_ARRAY_BUFFER, s, a.normals, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
  s = sizeof(TriangleMesh::Triangle) * a.numberOfTriangles;
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, a.triangles, GL_STATIC_DRAW);
  camera->updateView();
  program->setUniform(vMatrixLoc, camera->getWorldToCameraMatrix());
  program->setUniform(pMatrixLoc, camera->getProjectionMatrix());
  glLineWidth(1);
  clearScreen(Color::black);
  glDrawElements(GL_TRIANGLES, 3 * a.numberOfTriangles, GL_UNSIGNED_INT, 0);
  glDeleteBuffers(3, buffers);
  glDeleteVertexArrays(1, &vao);
  glFlush();
}

void
processKeys()
{
  for (int i = 0; i < MAX_KEYS; i++)
  {
    if (!keys[i])
      continue;

    const  float len = camera->getDistance() * CAMERA_RES;

    switch (i)
    {
      case '1':
        renderFunc = test1;
        break;
      case '2':
        renderFunc = test2;
        break;
      case '3':
        renderFunc = test3;
        break;
      case '4':
        renderFunc = test4;
        break;
      case '5':
        renderFunc = test5;
        break;
      case '6':
        renderFunc = test6;
      case 'l':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
      case 'p':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
      case 'w':
        camera->move(0, 0, -len);
        break;
      case 's':
        camera->move(0, 0, +len);
        break;
      case 'q':
        camera->move(0, +len, 0);
        break;
      case 'z':
        camera->move(0, -len, 0);
        break;
      case 'a':
        camera->move(-len, 0, 0);
        break;
      case 'd':
        camera->move(+len, 0, 0);
        break;
      case '-':
        camera->zoom(1.0f / ZOOM_SCALE);
        keys[i] = false;
        break;
      case '+':
        camera->zoom(ZOOM_SCALE);
        keys[i] = false;
        break;
    }
  }
}
