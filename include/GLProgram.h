#ifndef __GLProgram_h
#define __GLProgram_h

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
// OVERVIEW: GLProgram.h
// ========
// Class definitions for GLSL shader and program.

#include "GL/glew.h"
#include <GL/gl.h>
#include <stdio.h>
#include <string>
#include "Graphics/Color.h"
#include "DsMath"

using namespace std;
using namespace Ds;

namespace GLSL
{ // begin namespace GLSL

//
// Init GLSL
//
extern void init();

enum ShaderSource
{
  FILE,
  STRING
};


//////////////////////////////////////////////////////////
//
// Program: GLSL program class
// =======
class Program
{
public:
  enum State
  {
    CREATED,
    MODIFIED,
    BUILT,
    IN_USE
  };

  // Constructor
  Program(const char*);

  // Destructor
  ~Program();

  // Cast to handle type
  operator GLuint() const
  {
    return handle;
  }

  // Get program name
  const char* getName() const
  {
    return name.c_str();
  }

  // Get program state
  GLuint getState() const
  {
    return state;
  }

  // Add shader
  void addShader(GLenum, ShaderSource, const char*);

  // Use/disuse program
  void use();
  void disuse();

  // Get uniform variable location
  GLint getUniformLocation(const char*) const;

  // Set uniform variable by location
  static void setUniform(GLint, GLint);
  static void setUniform(GLint, float);
  static void setUniform(GLint, float, float);
  static void setUniform(GLint, float, float, float);
  static void setUniform(GLint, float, float, float, float);
  static void setUniform(GLint, const vec3f&);
  static void setUniform(GLint, const vec4f&);
  static void setUniform(GLint, const mat3f&);
  static void setUniform(GLint, const mat4f&);
  static void setUniform(GLint, const Color&);

  // Set uniform variable by name
  void setUniform(const char*, GLint);
  void setUniform(const char*, float);
  void setUniform(const char*, float, float);
  void setUniform(const char*, float, float, float);
  void setUniform(const char*, float, float, float, float);
  void setUniform(const char*, const vec3f&);
  void setUniform(const char*, const vec4f&);
  void setUniform(const char*, const mat3f&);
  void setUniform(const char*, const mat4f&);
  void setUniform(const char*, const Color&);

  // Get atributte location
  GLint getAttributeLocation(const char*) const;

  // Get subroutine index
  GLuint getSubroutineIndex(GLenum, const char*) const;
  GLuint getVertexSubroutineIndex(const char*) const;
  GLuint getFragmentSubroutineIndex(const char*) const;

  // Set subroutine by index
  static void setSubroutine(GLenum, GLuint&);
  static void setVertexSubroutine(GLuint&);
  static void setFragmentSubroutine( GLuint&);

  // Set subroutine by name
  void setSubroutine(GLenum, const char*);
  void setVertexSubroutine(const char*);
  void setFragmentSubroutine(const char*);

  // Get the current program
  static Program* getCurrent()
  {
    return current;
  }

  // Set the current program
  static void setCurrent(Program* c)
  {
    if (c != current)
    {
      if (c == 0)
        current->disuse();
      else
        c->use();
    }
  }

protected:
  // Link program
  void link();

private:
  static Program* current;

  GLuint handle;
  string name;
  GLuint state;

  // Check if program is in use
  void checkInUse() const;

}; // Program


//////////////////////////////////////////////////////////
//
// Program inline implementtaion
// =======
inline void
Program::setUniform(GLint loc, GLint i0)
{
  glUniform1i(loc, i0);
}

inline void
Program::setUniform(GLint loc, float f0)
{
  glUniform1f(loc, f0);
}

inline void
Program::setUniform(GLint loc, float f0, float f1)
{
  glUniform2f(loc, f0, f1);
}

inline void
Program::setUniform(GLint loc, float f0, float f1, float f2)
{
  glUniform3f(loc, f0, f1, f2);
}

inline void
Program::setUniform(GLint loc, float f0, float f1, float f2, float f3)
{
  glUniform4f(loc, f0, f1, f2, f3);
}

inline void
Program::setUniform(GLint loc, const vec3f& v)
{
  glUniform3fv(loc, 1, &v[0]);
}

inline void
Program::setUniform(GLint loc, const vec4f& v)
{
  glUniform4fv(loc, 1, &v[0]);
}

inline void
Program::setUniform(GLint loc, const mat3f& v)
{
  glUniformMatrix3fv(loc, 1, GL_FALSE, v.data());
}

inline void
Program::setUniform(GLint loc, const mat4f& v)
{
  glUniformMatrix4fv(loc, 1, GL_FALSE, v.data());
}

inline void
Program::setUniform(GLint loc, const Color& c)
{
  glUniform4fv(loc, 1, &c[0]);
}

inline void
Program::setUniform(const char* name, GLint i0)
{
  setUniform(getUniformLocation(name), i0);
}

inline void
Program::setUniform(const char* name, float f0)
{
  setUniform(getUniformLocation(name), f0);
}

inline void
Program::setUniform(const char* name, float f0, float f1)
{
  setUniform(getUniformLocation(name), f0, f1);
}

inline void
Program::setUniform(const char* name, float f0, float f1, float f2)
{
  setUniform(getUniformLocation(name), f0, f1, f2);
}

inline void
Program::setUniform(const char* name, float f0, float f1, float f2, float f3)
{
  setUniform(getUniformLocation(name), f0, f1, f2, f3);
}

inline void
Program::setUniform(const char* name, const vec3f& v)
{
  setUniform(getUniformLocation(name), v);
}

inline void
Program::setUniform(const char* name, const vec4f& v)
{
  setUniform(getUniformLocation(name), v);
}

inline void
Program::setUniform(const char* name, const mat3f& v)
{
  setUniform(getUniformLocation(name), v);
}

inline void
Program::setUniform(const char* name, const mat4f& v)
{
  setUniform(getUniformLocation(name), v);
}

inline void
Program::setUniform(const char* name, const Color& c)
{
  setUniform(getUniformLocation(name), c);
}

inline GLuint
Program::getVertexSubroutineIndex(const char* name) const
{
  return getSubroutineIndex(GL_VERTEX_SHADER, name);
}

inline GLuint
Program::getFragmentSubroutineIndex(const char* name) const
{
  return getSubroutineIndex(GL_FRAGMENT_SHADER, name);
}

inline void
Program::setSubroutine(GLenum shader, GLuint& index)
{
  glUniformSubroutinesuiv(shader, 1, &index);
}

inline void
Program::setVertexSubroutine(GLuint& index)
{
  setSubroutine(GL_VERTEX_SHADER, index);
}

inline void
Program::setFragmentSubroutine(GLuint& index)
{
  setSubroutine(GL_FRAGMENT_SHADER, index);
}

inline void
Program::setSubroutine(GLenum shader, const char* name)
{
  GLuint index = getSubroutineIndex(shader, name);
  setSubroutine(shader, index);
}

inline void
Program::setVertexSubroutine(const char* name)
{
  setSubroutine(GL_VERTEX_SHADER, name);
}

inline void
Program::setFragmentSubroutine(const char* name)
{
  setSubroutine(GL_FRAGMENT_SHADER, name);
}

} // end namespace GLSL

#endif // __GLSL_h
