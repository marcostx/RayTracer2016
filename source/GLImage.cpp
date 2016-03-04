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
//  OVERVIEW: GLImage.cpp
//  ========
//  Source file for GL image.

#include <memory.h>
#include "GLImage.h"

using namespace Graphics;

//
// Auxiliary functions
//
inline void
drawPixels(int w, int h, Pixel* buffer)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glRasterPos2i(0, 0);
  glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
  glFlush();
}

inline GLuint
createTexture(int w, int h)
{
  GLuint id;

  glEnable(GL_TEXTURE_2D);
  // Create texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // Initialize texture
  glTexImage2D(GL_TEXTURE_2D,
    0,
    GL_RGB,
    w,
    h,
    0,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    0);
  return id;
}

inline void
setTextureData(int w, int h, void* data)
{
  glTexSubImage2D(GL_TEXTURE_2D,
    0,
    0,
    0,
    w,
    h,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    data);
}


//////////////////////////////////////////////////////////
//
// GLImage implementation
// =======
GLImage::GLImage(int w, int h):
  ImageBuffer(w, h)
//[]----------------------------------------------------[]
//|  Constructor                                         |
//[]----------------------------------------------------[]
{
  buffer = new Pixel[w * h];
}

GLImage::~GLImage()
//[]----------------------------------------------------[]
//|  Destructor                                          |
//[]----------------------------------------------------[]
{
  delete []buffer;
}

void
GLImage::write(int i, Pixel pixels[])
//[]----------------------------------------------------[]
//|  Write                                               |
//[]----------------------------------------------------[]
{
  memcpy(buffer + i * W, pixels, W * sizeof(Pixel));
}

void
GLImage::draw() const
//[]----------------------------------------------------[]
//|  Draw                                                |
//[]----------------------------------------------------[]
{
  drawPixels(W, H, buffer);
}

Pixel*
GLImage::map(LockMode)
//[]----------------------------------------------------[]
//|  Map                                                 |
//[]----------------------------------------------------[]
{
  return buffer;
}

void
GLImage::unmap()
//[]----------------------------------------------------[]
//|  Unmap                                               |
//[]----------------------------------------------------[]
{
  // do nothing
}
