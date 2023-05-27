#ifndef PRECOMPILED_HEADER_H
#define PRECOMPILED_HEADER_H
/* [Precompiled Header Notes]
 * 1. To speed up build times, add headers that
 *    are infrequently changed and used frequently accross many files into this file
 *
 * 2. #include "pch.h" should only be added to source files (.cpp)
 *    and not header files (.h)
 */
// GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/random.hpp>
//#include <glm/gtx/projection.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/matrix_transform_2d.inl>
// 
// FreeType
//#include <ft2build.h>
//#include FT_FREETYPE_H 

//STB 
//#include "../../Dependencies/Vendor/stb_image.h"

// MSVC++ Run-Time Type Information
#include <typeinfo>
#include <typeindex>

// Enums - Reflective compile-time enum library
//#include <betterenum.h>

// Standard Libraries
//#include <cmath>
//#include <cassert>
//#include <stdexcept>
//#include <algorithm>
//#include <limits>
#include <iostream>

// STL Strings
#include <string>
#include <sstream>

// STL Container
#include <vector>
#include <array>

// Debugging
//#include "../Engine/Debugging/Memleak.h"
#include "../Engine/Debugging/Assert.h"
#include "../Engine/Debugging/Log.h"

// Math Libraries
#include "../Engine/Math/MathFunctions/Mathf.h"
#include "../Engine/Math/Matrix/Matrix3.h"
#include "../Engine/Math/Matrix/Matrix4.h"
#include "../Engine/Math/Vector/Vector2.h"
#include "../Engine/Math/Vector/Vector3.h"
#include "../Engine/Math/Vector/Vector4.h"

// Utilities

// Dear ImGui
#include "imgui/imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui/imgui_internal.h"


#endif