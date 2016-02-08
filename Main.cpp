#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLImage.h"
#include "GLRenderer.h"
#include "MeshReader.h"
#include "MeshSweeper.h"
#include "RayTracer.h"

#define WIN_W 1024
#define WIN_H 768

using namespace Graphics;

// Render globals
GLRenderer* renderer;
RayTracer* rayTracer;
GLImage* frame;
uint timestamp;
bool traceFlag;
int W;
int H;

// Light globals
Light* light;

// Mouse globals
int mouseX;
int mouseY;

// Keyboard globals
const int MAX_KEYS = 256;
bool keys[MAX_KEYS];

// Camera globals
const float CAMERA_RES = 0.01f;
const float ZOOM_SCALE = 1.01f;

// Animation globals
bool animateFlag;
const int UPDATE_RATE = 40;

inline void
printControls()
{
  printf("\n"
    "Camera controls:\n"
    "----------------\n"
    "(w) pan forward  (s) pan backward\n"
    "(q) pan up       (z) pan down\n"
    "(a) pan left     (d) pan right\n"
    "(+) zoom in      (-) zoom out\n"
    "GL render mode controls:\n"
    "------------------------\n"
    "(b) bounds       (n) normals       (v) axes\n"
    "(.) wireframe    (;) hiddenlines   (/) smooth\n\n");
}

static bool drawAxes = false;
static bool drawBounds = false;
static bool drawNormals = false;

void
processKeys()
{
  Camera* camera = renderer->getCamera();

  for (int i = 0; i < MAX_KEYS; i++)
  {
    if (!keys[i])
      continue;

    float len = camera->getDistance() * CAMERA_RES;

    switch (i)
    {
      // Camera controls
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
      case 'p':
        camera->changeProjectionType();
        break;
      case 'b':
        drawBounds ^= true;
        renderer->flags.enable(GLRenderer::DrawSceneBounds, drawBounds);
        renderer->flags.enable(GLRenderer::DrawActorBounds, drawBounds);
        break;
      case 'v':
        drawAxes ^= true;
        renderer->flags.enable(GLRenderer::DrawAxes, drawAxes);
        break;
      case 'n':
        drawNormals ^= true;
        renderer->flags.enable(GLRenderer::DrawNormals, drawNormals);
        break;
      case '.':
        renderer->renderMode = GLRenderer::Wireframe;
        break;
      case ';':
        renderer->renderMode = GLRenderer::HiddenLines;
        break;
      case '/':
        renderer->renderMode = GLRenderer::Smooth;
        break;
    }
  }
  if (camera->isModified())
    traceFlag = false;
}

void
initGL(int *argc, char **argv)
{
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  //glutInitContextProfile(GLUT_CORE_PROFILE);
  //glutInitContextVersion(3, 3);
  glutInitWindowSize(WIN_W, WIN_H);
  glutCreateWindow("RT");
  GLSL::init();
  glutReportErrors();
}

void
displayCallback()
{
  processKeys();
  if (!traceFlag)
    renderer->render();
  else
  {
    if (frame == 0)
      frame = new GLImage(W, H);

    Camera* camera = rayTracer->getCamera();
    uint ct = camera->updateView();

    if (timestamp != ct)
    {
      light->position = camera->getPosition();
      frame->lock(ImageBuffer::Write);
      rayTracer->renderImage(*frame);
      frame->unlock();
      timestamp = ct;
    }
    frame->draw();
  }
  glutSwapBuffers();
}

void
reshapeCallback(int w, int h)
{
  W = roundupImageWidth(w);
  renderer->setImageSize(W, H = h);
  renderer->getCamera()->setAspectRatio(REAL(W) / REAL(H));
  if (frame != 0)
  {
    delete frame;
    frame = 0;
    timestamp = 0;
    traceFlag = false;
  }
  printf("Image size: %dx%d\n", w, h);
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
  Camera* camera = renderer->getCamera();
  const float da = camera->getViewAngle() * CAMERA_RES;
  const float ay = (mouseX - x) * da * !keys['x'];
  const float ax = (mouseY - y) * da * !keys['y'];

  mouseX = x;
  mouseY = y;
  if (ax != 0 || ay != 0)
  {
    keys['r'] ? camera->roll(ay) : camera->rotateYX(ay, ax);
    traceFlag = false;
    glutPostRedisplay();
  }
}

void
mouseWheelCallback(int, int dir, int, int y)
{
  if (y == 0)
    return;
  if (dir > 0)
    renderer->getCamera()->zoom(ZOOM_SCALE);
  else
    renderer->getCamera()->zoom(1.0f / ZOOM_SCALE);
  traceFlag = false;
  glutPostRedisplay();
}

void
idleCallback()
{
  static GLint currentTime;
  GLint time = glutGet(GLUT_ELAPSED_TIME);

  if (abs(time - currentTime) >= UPDATE_RATE)
  {
    Camera* camera = renderer->getCamera();

    camera->azimuth(camera->getHeight() * CAMERA_RES);
    currentTime = time;
    traceFlag = false;
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
    case 't':
      traceFlag ^= true;
      glutPostRedisplay();
      break;
    case 'o':
      animateFlag ^= true;
      glutIdleFunc(animateFlag ? idleCallback : 0);
      glutPostRedisplay();
      break;
  }
}

Actor*
newActor(TriangleMesh* mesh,
  const vec3f& position = vec3f::null(),
  const vec3f& size = vec3f(1),
  const Color& color = Color::white)
{
  Primitive* p = new TriangleMeshShape(mesh);

  p->setMaterial(MaterialFactory::New(color));
  p->setTransform(position, quat::identity(), size);
  return new Actor(*p);
}

Scene*
createTestScene()
{
  Scene* scene = new Scene("test");
  TriangleMesh* s = MeshSweeper::makeSphere();

  scene->addActor(newActor(s, vec3(-3, -3, 0), vec3(1, 1, 1), Color::yellow));
  scene->addActor(newActor(s, vec3(+3, -3, 0), vec3(2, 1, 1), Color::green));
  scene->addActor(newActor(s, vec3(+3, +3, 0), vec3(1, 2, 1), Color::red));
  scene->addActor(newActor(s, vec3(-3, +3, 0), vec3(1, 1, 2), Color::blue));
  s = MeshReader().execute("f-16.obj");
  scene->addActor(newActor(s, vec3(2, -4, -10)));
  return scene;
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
  glutMouseWheelFunc(mouseWheelCallback);
  glutKeyboardFunc(keyboardCallback);
  glutKeyboardUpFunc(keyboardUpCallback);
  // print controls
  printControls();

  // create scene and camera
  Scene* scene = createTestScene();
  Camera* camera = new Camera();
  // create the renderers
  renderer = new GLRenderer(*scene, camera);
  renderer->renderMode = GLRenderer::Smooth;
  rayTracer = new RayTracer(*scene, camera);
  // create a light
  light = new Light(vec3::null());
  scene->addLight(light);
  glutMainLoop();
  return 0;
}
