//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Global.h
// ========
// Global typedefs and utilities.
//
// Author: Paulo Pagliosa
// Last revision: 08/09/2014

#ifndef __Global_h
#define __Global_h

#ifdef __CUDACC__
#include <host_defines.h>
#else
#define __host__
#define __device__
#define __align__(i)
#endif

#define DS_NAMESPACE Ds
#define DS_USE_NAMESPACE using namespace ::DS_NAMESPACE
#define DS_BEGIN_NAMESPACE namespace DS_NAMESPACE {
#define DS_END_NAMESPACE }

#define D_UNUSED(x) (void)x;

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long int int64;
typedef unsigned long int uint64;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#define INT8(buf,i) (*(int8*)((uint8*)buf + (i)))
#define UINT8(buf,i) (*(uint8*)((uint8*)buf + (i)))
#define INT16(buf,i) (*(int16*)((uint8*)buf + (i)))
#define UINT16(buf,i) (*(uint16*)((uint8*)buf + (i)))
#define INT32(buf,i) (*(int32*)((uint8*)buf + (i)))
#define UINT32(buf,i) (*(uint32*)((uint8*)buf + (i)))
#define INT64(buf,i) (*(int64*)((uint8*)buf + (i)))
#define UINT64(buf,i) (*(uint64*)((uint8*)buf + (i)))
#define CHAR(buf,i) (*(char*)((uint8*)buf + (i)))
#define UCHAR(buf,i) (*(uchar*)((uint8*)buf + (i)))
#define SHORT(buf,i) (*(short*)((uint8*)buf + (i)))
#define USHORT(buf,i) (*(ushort*)((uint8*)buf + (i)))
#define INT(buf,i) (*(int*)((uint8*)buf + (i)))
#define UINT(buf,i) (*(uint*)((uint8*)buf + (i)))
#define LONG(buf,i) (*(long*)((uint8*)buf + (i)))
#define ULONG(buf,i) (*(ulong*)((uint8*)buf + (i)))

DS_BEGIN_NAMESPACE

/// Exchanges the values of a and b.
template <typename T>
__host__ __device__ inline void
dSwap(T& a, T& b)
{
  T temp = a;

  a = b;
  b = temp;
}

/// Returns the smallest of a and b.
template <typename T>
__host__ __device__ inline T
dMin(const T& a, const T& b)
{
  return a < b ? a : b;
}

/// Returns the largest of a and b.
template <typename T>
__host__ __device__ inline T
dMax(const T& a, const T& b)
{
  return a > b ? a : b;
}

DS_END_NAMESPACE

#endif // __Global_h
