#ifndef __GLImage_h
#define __GLImage_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        GVSG Foundation Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: GLImage.h
//  ========
//  Class definition for GL image.

#include "GLProgram.h"
#include "Image.h"

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// GLImage: GL image class
// =======
class GLImage: public ImageBuffer
{
public:
  // Constructor
  GLImage(int, int);

  // Destructor
  ~GLImage();

  // Write pixels
  void write(int, Pixel[]);

  // Draw
  void draw() const;

protected:
  class Renderer: public GLSL::Program
  {
  public:
    // Constructor
    Renderer();

    // Render image
    void render(const GLImage&);

  }; // Renderer

  static Renderer* getRenderer();

  Pixel* map(LockMode);
  void unmap();

private:
  GLuint handle;
  Pixel* buffer;

}; // GLImage

} // end namespace Graphics

#endif // __GLImage_h
