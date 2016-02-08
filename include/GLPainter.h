#ifndef __GLPainter_h
#define __GLPainter_h

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
//  OVERVIEW: GLPainter.h
//  ========
//  Class definition for GL renderer.

#include "GLProgram.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// GLPainter: GL painter class
// =========
class GLPainter
{
public:
  // Constructor
  GLPainter();

  // Destructor
  ~GLPainter();

  // Get line color
  const Color& getLineColor() const
  {
    return lineColor;
  }

  // Set line color
  void setLineColor(const Color& c)
  {
    lineColor = c;
  }

protected:
  // Draw line in NDC
  void drawLine(const vec4*);

private:
  GLuint vao;
  GLSL::Program program;
  Color lineColor;
  GLint pointsLoc;
  GLint lineColorLoc;

}; // GLPainter

} // end namespace Graphics

#endif // __GLPainter_h
