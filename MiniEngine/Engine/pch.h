#pragma once
#ifndef UNICODE
#define UNICODE
#endif

// ------------------------------------Include------------------------------------------- //
#include <fstream>
#include <algorithm>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <functional>
#include <string>
#include <array>
#include <mutex>
#include <vector>
#include <shared_mutex>
#include <iostream>
#include <thread>
#include <exception>

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <WindowsX.h>
#include <sdkddkver.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dinput.h>
#include <hidusage.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "Engine/render/PC/d3dx12.h"
#include "dxgi.h"

#pragma comment(lib, "Shcore.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#if defined(DEBUG) or defined(_DEBUG)
typedef HRESULT(WINAPI* pDXGIGetDebugInterface)(REFIID riid, void** pDebug);
#endif

using namespace Microsoft::WRL;

#undef min
#undef max

#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define MEM_VIRTUAL_ALLOC(size) VirtualAlloc(nullptr, (size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
#define MEM_VIRTUAL_FREE(mem) VirtualFree((mem), 0, MEM_RELEASE)

#if __cplusplus >= 202002L  // check cpp20
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;
#endif
#else
#define MEM_VIRTUAL_ALLOC(mSize) malloc((mSize))
#define MEM_VIRTUAL_FREE(mem) free((mem))
#endif
#ifdef ORBIS
#include <vectormath.h>
using byte = uint8_t;
#define  MAXUINT64 0xffffffffffffffff
#define  MAXUINT32 0xffffffff
#define  MAXUINT16 0xffff
#endif 


// -----------------------------------Definition----------------------------------------- //

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#ifndef __FILEW__
#define __FILEW__ WIDEN(__FILE__)
#endif
#ifndef __FUNCTIONW__
#define __FUNCTIONW__ WIDEN(__FUNCTION__)
#endif

#define DELETE_MOVE_CONSTRUCTOR(name) name(name&& other) noexcept = delete;
#define DELETE_COPY_CONSTRUCTOR(name) name(const name& other) = delete;
#define DELETE_MOVE_OPERATOR(name) name& operator=(name&& other) noexcept = delete;
#define DELETE_COPY_OPERATOR(name) name& operator=(const name& other) = delete;
#define DEFAULT_MOVE_CONSTRUCTOR(name) name(name&& other) noexcept = default;
#define DEFAULT_COPY_CONSTRUCTOR(name) name(const name& other) = default;
#define DEFAULT_MOVE_OPERATOR(name) name& operator=(name&& other) noexcept = default;
#define DEFAULT_COPY_OPERATOR(name) name& operator=(const name& other) = default;
#define DEFAULT_COPY_MOVE(name) DEFAULT_COPY_CONSTRUCTOR(name);\
    DEFAULT_MOVE_CONSTRUCTOR(name);\
    DEFAULT_COPY_OPERATOR(name);\
    DEFAULT_MOVE_OPERATOR(name);
#define NON_COPYABLE(name) DELETE_COPY_CONSTRUCTOR(name);\
    DELETE_COPY_OPERATOR(name);
#define NON_MOVEABLE(name) DELETE_MOVE_CONSTRUCTOR(name);\
    DELETE_MOVE_OPERATOR(name);

#ifdef UNICODE
#define TO_STRING(str) std::to_wstring(str)
#define __REFLECTION_FUNC_NAME__ __FUNCTIONW__
#define __REFLECTION_FILE_NAME__ __FILEW__
#ifndef TEXT
#define TEXT(x) WIDEN2(x)
#endif
using String = std::wstring;
using Char = wchar_t;
#ifdef WIN32
#define WARN(message) std::wcout << TEXT("[ WARN | ") << __REFLECTION_FILE_NAME__ << " | " << __REFLECTION_FUNC_NAME__ << " ]" << TEXT(##message);
#endif
#ifdef ORBIS
#define WARN(message) void(0);
#endif
#else
#define TO_STRING(str) std::to_string(str)
#ifndef TEXT
#define TEXT(x) x
#endif
using String = std::string;
using Char = char;
#define __REFLECTION_FUNC_NAME_ __FUNCTION__
#define __REFLECTION_FILE_NAME__ __FILE__
#endif