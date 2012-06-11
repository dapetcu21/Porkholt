/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMAIN_H
#define PHMAIN_H

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <climits>
#include <cfloat>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <utility>
using namespace std;

#ifdef _MSC_VER

    #define PH_CCALL __cdecl
    #pragma section(".CRT$XCU",read)
    #define PH_INITIALIZER(f) \
    static void __cdecl f(void); \
    __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)

#elif defined(__GNUC__)

    #define PH_CCALL
    #define PH_INITIALIZER(f) \
    static void f(void) __attribute__((constructor)); \
    static void f(void)

#endif

#ifdef __APPLE__
    #ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
        #define PH_IPHONE_OS
        #define PH_MOBILE
        #ifdef __i386__
            #define PH_SIMULATOR
        #endif
    #else
        #define PH_MAC_OS
        #define PH_DESKTOP
    #endif
    #define PH_DARWIN
#endif

#ifdef __linux__
    #define PH_LINUX
    #define PH_DESKTOP
#endif

#if (__STDC_VERSION__ >= 199901L)
    #define PH_RESTRICT restrict
#else
    #define PH_RESTRICT __restrict
#endif

typedef void * (*PHAllocator)(void);
typedef float ph_float;

template <class T>
void * PHAlloc(void)
{
    return (void*) new T;
}

//map<string,PHAllocator> * list;
#define PH_REGISTERCLASS(list,name,clss) PH_INITIALIZER( PHRegister_ ## clss ) { if (!list) list = new map<string,PHAllocator>; list->insert(make_pair<string,void * (*)(void)>(name,PHAlloc<clss>)); }

#ifdef PH_IPHONE_OS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#endif
#ifdef PH_MAC_OS
    #import <OpenGL/gl.h>
    #import <OpenGL/glext.h>
#endif
#ifdef PH_LINUX
#define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#if defined(DEBUG) || defined(_DEBUG)
    #define PH_DEBUG
#endif

template< bool cond > struct _PH_STATIC_ASSERTION_FAILURE;
template <> struct _PH_STATIC_ASSERTION_FAILURE< true > {};
template <> struct _PH_STATIC_ASSERTION_FAILURE< false > {};
#define _PH_TOKENPASTE(x, y) x ## y
#define PH_TOKENPASTE(x, y) _PH_TOKENPASTE(x, y)
#ifdef __COUNTER__
#define PH_UNIQUE_TOKEN __COUNTER__
#else
#define PH_UNIQUE_TOKEN __LINE__
#endif

#define PH_STATIC_ASSERT(cond) enum { PH_TOKENPASTE(dummy,PH_UNIQUE_TOKEN) = sizeof(_PH_STATIC_ASSERTION_FAILURE< (bool)(cond) >) }

struct lua_State; 

#define PH_NO_PACKED_STRUCT

#ifdef PH_NO_PACKED_STRUCT
#define PH_PACKED_STRUCT
#else
#ifdef __GNUC__
#define PH_PACKED_STRUCT __attribute__((packed))
#elif _MSC_VER
#define PH_PACKED_STRUCT_PRAGMA
#define PH_PACKED_STRUCT
#endif
#endif

#include <Porkholt/Core/PHInvocation.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHObject.h>
#include <Porkholt/Core/PHMessage.h>
#include <Porkholt/Geometry/PHGeometry.h>
#include <Porkholt/Core/PHErrors.h>

#include <Porkholt/Core/PHThread.h>
#include <Porkholt/Core/PHMutex.h>
#include <Porkholt/Core/PHSemaphore.h>

#endif
