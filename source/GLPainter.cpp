//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2014-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: GLPainter.cpp
//  ========
//  Source file for GL painter.

#include "GLPainter.h"

using namespace Graphics;

static const char* vertexShader =
  "#version 400\n"
  "uniform vec4 p[2];\n"
  "uniform vec4 lineColor;\n"
  "out vec4 color;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = p[gl_VertexID];\n"
  "  color = lineColor;\n"
  "}";

// The input variable "color" of the fragment shader is the
// (interpolated) output variable "color" of the vertex shader
// (note that the types and names have to be equal)
static const char* fragmentShader =
  "#version 400\n"
  "in vec4 color;\n"
  "out vec4 fragmentColor;\n"
  "void main()\n"
  "{\n"
  "  fragmentColor = color;\n"
 "}";


//////////////////////////////////////////////////////////
//
// GLPainter implementation
// =========
GLPainter::GLPainter():
program("painter")
{
  program.addShader(GL_VERTEX_SHADER, GLSL::STRING, vertexShader);
  program.addShader(GL_FRAGMENT_SHADER, GLSL::STRING, fragmentShader);
  program.use();
  pointsLoc = program.getUniformLocation("p[0]");
  lineColorLoc = program.getUniformLocation("lineColor");
  glGenVertexArrays(1, &vao);
}

GLPainter::~GLPainter()
{
  glDeleteVertexArrays(1, &vao);
}


void
GLPainter::drawLine(const vec4* p)
{
  using namespace GLSL;

  Program* current = Program::getCurrent();

  program.use();
  program.setUniform(lineColorLoc, lineColor);
  glBindVertexArray(vao);
  glUniform4fv(pointsLoc, 2, (float*)p);
  glDrawArrays(GL_LINES, 0, 2);
  Program::setCurrent(current);
}
