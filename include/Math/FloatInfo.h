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
// OVERVIEW: FloatInfo.h
// ========
// Class definition for floating point type traits.
//
// Author: Paulo Pagliosa
// Last revision: 08/09/2014

#ifndef __FloatInfo_h
#define __FloatInfo_h

#include <float.h>
#include "Core/Global.h"

DS_BEGIN_NAMESPACE

template <typename real> struct FloatInfo;

#define DECLARE_FLOAT_INFO(type, EPS, MAX) \
template <> struct FloatInfo<type> \
{ \
  static type eps() { return EPS; } \
  static type inf() { return MAX; } \
}

DECLARE_FLOAT_INFO(float , FLT_EPSILON, FLT_MAX);
DECLARE_FLOAT_INFO(double, DBL_EPSILON, DBL_MAX);

DS_END_NAMESPACE

#endif // __FloatInfo_h
