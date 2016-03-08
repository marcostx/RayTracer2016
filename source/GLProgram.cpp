//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2012-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
// OVERVIEW: GLProgram.cpp
// ========
// Source code for GLSL shader and program.

#include <stdarg.h>
#include <stdlib.h>
#include "GLProgram.h"

enum GLSL_ErrorCode
{
  GLSL_INITIALIZATION_ERROR,
  SUPPORT_FOR_OPENGL_EXTENSIONS_MISSING,
  GLSL_NOT_INITIALIZED,
  UNABLE_TO_OPEN_SHADER_FILE,
  COMPILE_ERROR,
  CANNOT_ATTACH_SHADER,
  LINK_ERROR,
  CANNOT_USE_PROGRAM,
  PROGRAM_NOT_IN_USE,
  VARIABLE_NOT_FOUND,
  SUBROUTINE_NOT_FOUND
};

static const char* GLSL_errorMessages[] =
{
  "GLSL initialization error: %s\n",
  "Support for OpenGL extensions missing",
  "GLSL not initialized",
  "Unable to open shader file '%s'",
  "'%s': compile error:\n%s",
  "'%s': cannot attach shader '%s': program in use",
  "'%s': link error:\n%s",
  "'%s': cannot use program: no shader",
  "'%s': program not in use",
  "'%s': variable '%s' not found",
  "'%s': subroutine '%s' not found"
};

#define EM_MAXLEN 1024

//
// Auxiliary functions
//
void
error(GLSL_ErrorCode code, ...)
{
  va_list args;
  char msg[EM_MAXLEN];

  va_start(args, code);
  vsnprintf(msg, EM_MAXLEN, GLSL_errorMessages[code], args);
  printf("Error: %s\n", msg);
  printf("Press any key to exit...\n");
  getchar();
  exit(EXIT_FAILURE);
}

static char*
readShaderFile(const char* fileName)
//[]----------------------------------------------------[]
//|  Read file                                           |
//[]----------------------------------------------------[]
{
  FILE* file = fopen(fileName, "rt");

  if (file == 0)
    error(UNABLE_TO_OPEN_SHADER_FILE, fileName);
  fseek(file, 0, SEEK_END);

  long fileLength = ftell(file);
  char* buffer = new char[fileLength + 1];

  rewind(file);
  buffer[fread(buffer, sizeof(char), fileLength, file)] = 0;
  fclose(file);
  return buffer;
}

typedef void (*ObjectParamFunc)(GLuint, GLenum, GLint*);
typedef void (*InfoLogFunc)(GLuint, GLsizei, GLsizei*, GLchar*);

string
getInfoLog(GLuint obj, ObjectParamFunc getParam, InfoLogFunc getLog)
//[]----------------------------------------------------[]
//|  Get info log                                        |
//[]----------------------------------------------------[]
{
  GLint maxLen = 0;
  string log;

  getParam(obj, GL_INFO_LOG_LENGTH, &maxLen);
  if (maxLen > 0)
  {
    GLchar* buf = new char[maxLen];
    GLsizei len = 0;

    getLog(obj, maxLen, &len, buf);
    log = string(buf, len);
    delete[]buf;
  }
  return log;
}

//
// GLSL initialization
//
static bool GLSL_ready;

inline void
checkGLSL()
//[]----------------------------------------------------[]
//|  Check GLSL                                          |
//[]----------------------------------------------------[]
{
  if (!GLSL_ready)
    error(GLSL_NOT_INITIALIZED);
}

namespace GLSL
{ // begin namespace GLSL

void
init()
//[]----------------------------------------------------[]
//|  Init                                                |
//[]----------------------------------------------------[]
{
  if (GLSL_ready)
    return;

  GLenum code = glewInit();

  if (GLEW_OK != code)
    error(GLSL_INITIALIZATION_ERROR, glewGetErrorString(code));
  //if (!glewIsSupported("GL_VERSION_3_0"))
  //  error(SUPPORT_FOR_OPENGL_EXTENSIONS_MISSING);
  GLSL_ready = true;
}


//////////////////////////////////////////////////////////
//
// Shader: GLSL shader class
// ======
class Shader
{
public:
  // Constructor
  Shader(GLenum shaderType):
    handle(glCreateShader(shaderType)),
    name(shaderName(shaderType)),
    compiled(false)
  {
    // do nothing
  }

  // Destructor
  ~Shader()
  {
    glDeleteShader(handle);
  }

  // Load source from file
  void loadSourceFromFile(const char*);

  // Set source
  void setSource(const char*);

  // Cast to handle type
  operator GLuint() const
  {
    return handle;
  }

  // Get shader name
  const char* getName() const
  {
    return name.c_str();
  }

  bool isCompiled() const
  {
    return compiled;
  }

private:
  GLuint handle;
  string name;
  bool compiled;

  // Compile shader
  void compile();

  static const char* shaderName(GLenum shaderType)
  {
    switch (shaderType)
    {
      default:
        return "unknown shader";
      case GL_VERTEX_SHADER:
        return "vertex shader";
      case GL_TESS_CONTROL_SHADER:
        return "tess control shader";
      case GL_TESS_EVALUATION_SHADER:
        return "tess evaluation shader";
      case GL_GEOMETRY_SHADER:
        return "geometry shader";
      case GL_FRAGMENT_SHADER:
        return "fragment shader";
      case GL_COMPUTE_SHADER:
        return "compute shader";
    }
  }

}; // Shader


//////////////////////////////////////////////////////////
//
// Shader implementation
// ======
inline void
Shader::loadSourceFromFile(const char* fileName)
//[]----------------------------------------------------[]
//|  Load source from file                               |
//[]----------------------------------------------------[]
{
  if (fileName == 0)
    return;

  const char* buffer = readShaderFile(fileName);

  // Set shader source code
  glShaderSource(handle, 1, &buffer, 0);
  compiled = false;
  // Delete buffer
  delete []buffer;
  // Compile shader
  compile();
}

inline void
Shader::setSource(const char* buffer)
//[]----------------------------------------------------[]
//|  Set source                                          |
//[]----------------------------------------------------[]
{
  if (buffer == 0)
    return;
  // Set shader source code
  glShaderSource(handle, 1, &buffer, 0);
  // Compile shader
  compile();
}

void
Shader::compile()
//[]----------------------------------------------------[]
//|  Compile                                             |
//[]----------------------------------------------------[]
{
  // Compile shader
  glCompileShader(handle);

  GLint ok;

  // Get compile status
  glGetShaderiv(handle, GL_COMPILE_STATUS, &ok);
  if (ok == GL_TRUE)
    compiled = true;
  else
  {
    string log = getInfoLog(handle, glGetShaderiv, glGetShaderInfoLog);
    error(COMPILE_ERROR, getName(), log.c_str());
  }
}


//////////////////////////////////////////////////////////
//
// Program implementtaion
// =======
Program* Program::current = 0;

Program::Program(const char* programName):
  name(programName),
  state(CREATED)
//[]----------------------------------------------------[]
//|  Constructor                                         |
//[]----------------------------------------------------[]
{
  checkGLSL();
  handle = glCreateProgram();
}

Program::~Program()
//[]----------------------------------------------------[]
//|  Destructor                                          |
//[]----------------------------------------------------[]
{
  disuse();
  // Delete program
  glDeleteProgram(handle);
}

void
Program::addShader(GLenum type, ShaderSource where, const char* source)
//[]----------------------------------------------------[]
//|  Add shader                                          |
//[]----------------------------------------------------[]
{
  Shader s(type);

  if (state == IN_USE)
    error(CANNOT_ATTACH_SHADER, getName(), s.getName());
  where == FILE ? s.loadSourceFromFile(source) : s.setSource(source);
  // Attach shader
  glAttachShader(handle, s);
  state = MODIFIED;
}

void
Program::use()
//[]----------------------------------------------------[]
//|  Use                                                 |
//[]----------------------------------------------------[]
{
  switch (state)
  {
    case IN_USE:
      break;
    case CREATED:
      error(CANNOT_USE_PROGRAM, getName());
      break;
    case MODIFIED:
      link();
    case BUILT:
      if (current != 0)
        current->state = BUILT;
      glUseProgram(handle);
      state = IN_USE;
      current = this;
  }
}

inline void
Program::checkInUse() const
//[]----------------------------------------------------[]
//|  Check if program is in use                          |
//[]----------------------------------------------------[]
{
  if (state != IN_USE)
    error(PROGRAM_NOT_IN_USE, getName());
}

void
Program::disuse()
//[]----------------------------------------------------[]
//|  Disuse program                                      |
//[]----------------------------------------------------[]
{
  if (state == IN_USE)
  {
    current = 0;
    glUseProgram(0);
    state = BUILT;
  }
}

GLint
Program::getAttributeLocation(const char* name) const
//[]----------------------------------------------------[]
//|  Get attribute location                              |
//[]----------------------------------------------------[]
{
  checkInUse();

  GLint loc = glGetAttribLocation(handle, name);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, getName(), name);
  return loc;
}

GLint
Program::getUniformLocation(const char* name) const
//[]----------------------------------------------------[]
//|  Get uniform variable location                       |
//[]----------------------------------------------------[]
{
  checkInUse();

  GLint loc = glGetUniformLocation(handle, name);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, getName(), name);
  return loc;
}

GLuint
Program::getSubroutineIndex(GLenum shader, const char* name) const
//[]----------------------------------------------------[]
//|  Get subroutine index                                |
//[]----------------------------------------------------[]
{
  checkInUse();

  GLuint index = glGetSubroutineIndex(handle, shader, name);

  if (index == GL_INVALID_INDEX)
    error(SUBROUTINE_NOT_FOUND, getName(), name);
  return index;
}

void
Program::link()
//[]----------------------------------------------------[]
//|  Link                                                |
//[]----------------------------------------------------[]
{
  // Link program
  glLinkProgram(handle);

  GLint ok;

  // Get link status
  glGetProgramiv(handle, GL_LINK_STATUS, &ok);
  if (ok == GL_TRUE)
    state = BUILT;
  else
  {
    string log = getInfoLog(handle, glGetProgramiv, glGetProgramInfoLog);
    error(LINK_ERROR, getName(), log.c_str());
  }
}

} // end namespace GLSL
