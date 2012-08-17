// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>
#define istWindows
#endif // _WIN32


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define __ist_with_OpenGL__
#define __ist_with_zlib__
#define __ist_with_png__
#define __ist_with_gli__

#ifdef __ist_with_EASTL__
#   include <EASTL/algorithm.h>
#   include <EASTL/sort.h>
#   include <EASTL/vector.h>
#   include <EASTL/list.h>
#   include <EASTL/set.h>
#   include <EASTL/map.h>
#   include <EASTL/string.h>
namespace stl = eastl;
#else // __ist_with_EASTL__
#   include <vector>
#   include <list>
#   include <map>
#   include <string>
#   include <algorithm>
namespace stl = std;
#endif // __ist_with_EASTL__


#ifdef __ist_with_OpenGL__
#   define GLEW_STATIC
#   include <GL/glew.h>
#   ifdef istWindows
#       include <GL/wglew.h>
#       pragma comment(lib, "glew32s.lib")
#       pragma comment(lib, "opengl32.lib")
#   endif // istWindows
#endif // __ist_with_OpenGL__

#ifdef __ist_with_zlib__
#   include "zlib/zlib.h"
#   pragma comment(lib, "zlib.lib")
#endif // __ist_with_zlib__

#ifdef __ist_with_png__
#   include <libpng/png.h>
#   pragma comment(lib,"libpng15s.lib")
#endif // __ist_with_png__

#ifdef __ist_with_jpeg__
#   include <jpeglib.h>
#   include <jerror.h>
#   pragma comment(lib,"libjpeg.lib")
#endif // __ist_with_jpeg__

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef float               float32;
using glm::vec2;
using glm::ivec2;
using glm::uvec2;
using glm::vec4;
using glm::ivec4;
using glm::uvec4;
using glm::mat4;

#define istPrint(...)
#define istAssert(...)
#define istInterModule

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
