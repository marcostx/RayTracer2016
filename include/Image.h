#ifndef __Image_h
#define __Image_h

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
//  OVERVIEW: Image.h
//  ========
//  Class definition for image.

#include "Graphics/Color.h"

#define MIN_IMAGE_WIDTH 4

inline int
roundupImageWidth(int w)
{
  return (w + MIN_IMAGE_WIDTH - 1) & -MIN_IMAGE_WIDTH;
}

using namespace Ds;


//////////////////////////////////////////////////////////
//
// Pixel: pixel class
// =====
struct Pixel
{
  uint8 r;
  uint8 g;
  uint8 b;

  __host__ __device__
  Pixel()
  {
      // do nothing
  }

  __host__ __device__
  Pixel(uint8 r, uint8 g, uint8 b)
  {
    set(r, g, b);
  }

  __host__ __device__
  Pixel(const Color& c)
  {
    set(c);
  }

  __host__ __device__
  void set(uint8 r, uint8 g, uint8 b)
  {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  __host__ __device__
  void set(const Color& c)
  {
    r = (uint8)(255 * c.r);
    g = (uint8)(255 * c.g);
    b = (uint8)(255 * c.b);
  }

  __host__ __device__
  Pixel& operator +=(const Pixel& p)
  {
    r += p.r;
    g += p.g;
    b += p.b;
    return *this;
  }

  __host__ __device__
  Pixel& operator +=(const Color& c)
  {
    r += (uint8)(255 * c.r);
    g += (uint8)(255 * c.g);
    b += (uint8)(255 * c.b);
    return *this;
  }

}; // Pixel

namespace Graphics
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// Image: generic image class
// =====
class Image
{
public:
  // Destructor
  virtual ~Image()
  {
    // do nothing
  }

  virtual void getSize(int&, int&) const = 0;
  virtual void write(int, Pixel[]) = 0;

}; // Image


//////////////////////////////////////////////////////////
//
// ImageBuffer: generic image buffer class
// ===========
class ImageBuffer: public Image
{
public:
  enum LockMode
  {
    Read,
    Write
  };

  int getWidth() const
  {
    return W;
  }

  int getHeight() const
  {
    return H;
  }

  void getSize(int& w, int& h) const
  {
    w = W;
    h = H;
  }

  // Lock image to read/write
  Pixel* lock(LockMode mode)
  {
    if (buffer == 0)
      buffer = map(mode);
    return buffer;
  }

  void unlock()
  {
    if (buffer != 0)
    {
      unmap();
      buffer = 0;
    }
  }

  Pixel readPixel(int i, int j) const
  {
    return buffer[i * W + j];
  }

  void writePixel(int i, int j, const Pixel& pixel)
  {
    buffer[i * W + j] = pixel;
  }

  // Draw image
  virtual void draw() const = 0;

protected:
  int W;
  int H;
  Pixel* buffer;

  // Constructor
  ImageBuffer(int w, int h):
    W(w),
    H(h),
    buffer(0)
  {
    // do nothing
  }

  // Map image buffer
  virtual Pixel* map(LockMode) = 0;

  // Unmap image buffer
  virtual void unmap() = 0;

}; // Image

} // end namespace Graphics

#endif // __Image_h
