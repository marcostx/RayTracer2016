#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLImage.h"
#include "GLRenderer.h"
#include "MeshReader.h"
#include "MeshSweeper.h"
#include "RayTracer.h"
#include "pugixml.hpp"
#include "Parser.h"
#include <iostream>

#define WIN_W 1024
#define WIN_H 768

using namespace Graphics;

// Render globals
GLRenderer* render;
// Ray Tracer 
RayTracer* rayTracer;

GLImage* frame;
uint timestamp;

// Windows Ids
int currentWindowId;
int mainWindowId;
int windowId;

// Window size
int W = WIN_W;
int H = WIN_H;

// Pixels to trace a ray to
int X = 0, Y = 0;
bool traceFlag;
bool trace_adaptativeFlag;

// Subwindows sizes
int x_, y_, W_, H_;
int border = 20;

// Scene
Scene* scene;

// Camera
Camera* camera;

// Actor Camera
Primitive* cameraPrimitive;

// Scene Parser
Parser* sceneParser;

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

void drawFrustum();

inline void
printControls()
{
  printf("\n"
    "Options:\n"
    "----------------\n"
    "(o) OpenGL       (t) Ray Tracer(normal)\n (i) Ray Tracer(adaptative super-sampling) \n");
}

static bool drawAxes = false;
static bool drawBounds = false;
static bool drawNormals = false;
static bool drawFrustumFlag = false;

void
processKeys()
{
	Camera* camera = render->getCamera();

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
				render->flags.enable(GLRenderer::DrawSceneBounds, drawBounds);
				render->flags.enable(GLRenderer::DrawActorBounds, drawBounds);
				break;
			case 'v':
				drawAxes ^= true;
				render->flags.enable(GLRenderer::DrawAxes, drawAxes);
				break;
			case 'n':
				drawNormals ^= true;
				render->flags.enable(GLRenderer::DrawNormals, drawNormals);
				break;
			case '.':
				render->renderMode = GLRenderer::Wireframe;
				break;
			case ';':
				render->renderMode = GLRenderer::HiddenLines;
				break;
			case '/':
				render->renderMode = GLRenderer::Smooth;
				break;
		}
	}
	if (camera->isModified()){
		trace_adaptativeFlag = false;
		traceFlag = false;
	}
}

void
initGL(int *argc, char **argv)
{
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(W, H);
  mainWindowId = glutCreateWindow(scene->getName().c_str());
  GLSL::init();
  glutReportErrors();
}

void
displayCallback()
{
  glutSetWindow(mainWindowId);
  processKeys();
  if (!traceFlag && !trace_adaptativeFlag)
    render->render();
  else if (traceFlag)
  {
    if (frame == 0)
      frame = new GLImage(W, H);

    Camera* camera = rayTracer->getCamera();
    uint ct = camera->updateView();

    if (timestamp != ct)
    {
      frame->lock(ImageBuffer::Write);
      rayTracer->renderImage(*frame,false);
      frame->unlock();
      timestamp = ct;
    }
    frame->draw();
  }
  else if (trace_adaptativeFlag)
  {
	  if (frame == 0)
		  frame = new GLImage(W, H);

	  Camera* camera = rayTracer->getCamera();
	  uint ct = camera->updateView();

	  if (timestamp != ct)
	  {
		  frame->lock(ImageBuffer::Write);
		  rayTracer->renderImage(*frame, true);
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
  render->setImageSize(W, H = h);
  render->getCamera()->setAspectRatio(REAL(W) / REAL(H));
  if (frame != 0)
  {
    delete frame;
    frame = 0;
    timestamp = 0;
    traceFlag = false;
  }
  printf("Image new size: %dx%d\n", w, h);
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
  Camera *camera = render->getCamera();
  float da = camera->getViewAngle() * CAMERA_RES;
  float ay = (mouseX - x) * da;
  float ax = (mouseY - y) * da;

  camera->rotateYX(ay, ax);

  mouseX = x;
  mouseY = y;
  
  traceFlag = false;
  trace_adaptativeFlag = false;

  glutPostRedisplay();
}

void
mouseWheelCallback(int, int dir, int, int y)
{
  if (y == 0)
    return;
  if (dir > 0)
    render->getCamera()->zoom(ZOOM_SCALE);
  else
    render->getCamera()->zoom(1.0f / ZOOM_SCALE);
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
    Camera* camera = render->getCamera();

    camera->azimuth(camera->getHeight() * CAMERA_RES);
    currentTime = time;
    traceFlag = false;
    glutPostRedisplay();
  }
}

void
keyboardCallback(unsigned char key, int /*x*/, int /*y*/)
{
	glutSetWindow(mainWindowId);
    keys[key] = true;
    glutPostRedisplay();
}

void
keyboardUpCallback(unsigned char key, int /*x*/, int /*y*/)
{
	glutSetWindow(mainWindowId);
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
  case 'i':
	  trace_adaptativeFlag ^= true;
	  glutPostRedisplay();
	  break;
  case 'o':
    animateFlag ^= true;
    glutIdleFunc(animateFlag ? idleCallback : 0);
    glutPostRedisplay();
    break;
  }
}

void glutCallbacks()
{
  glutMouseFunc(mouseCallback);
  glutMotionFunc(motionCallback);
  glutMouseWheelFunc(mouseWheelCallback);
  glutKeyboardFunc(keyboardCallback);
  glutKeyboardUpFunc(keyboardUpCallback);
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
	Actor* act = newActor(s, vec3(-3, -3, 0), vec3(1, 1, 1), Color::yellow);
	act->setName("Xinfonildo");
	scene->addActor(act);
	scene->addActor(newActor(s, vec3(+3, -3, 0), vec3(2, 1, 1), Color::green));
	scene->addActor(newActor(s, vec3(+3, +3, 0), vec3(1, 2, 1), Color::red));
	scene->addActor(newActor(s, vec3(-3, +3, 0), vec3(1, 1, 2), Color::blue));
	s = MeshReader().execute("f-16.obj");
	Actor *aac = newActor(s, vec3(2, -4, -10));
	aac->setName("f-16.obj");
	scene->addActor(aac);
	return scene;
}

int
main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Informe o arquivo XML com a cena.");
    return 0;
  }

  sceneParser = new Parser(argv[1]);
  sceneParser->parseImage(H, W);

  camera = sceneParser->parseCamera();
  scene = sceneParser->parseScene();

  // init OpenGL
  initGL(&argc, argv);
  glutDisplayFunc(displayCallback);
  glutReshapeFunc(reshapeCallback);
  glutMouseFunc(mouseCallback);
  glutMotionFunc(motionCallback);
  glutMouseWheelFunc(mouseWheelCallback);
  glutKeyboardFunc(keyboardCallback);
  glutKeyboardUpFunc(keyboardUpCallback);
  glutCallbacks();

  // create the renderers
  render = new GLRenderer(*scene, camera);
  render->renderMode = GLRenderer::Smooth;

  rayTracer = new RayTracer(*scene, camera);

  // print usage
  printControls();

  // glut loop
  glutMainLoop();

  return 0;
}
