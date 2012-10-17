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
    #pragma section(".CRT$XCU",read)
    #define PH_INITIALIZER(f) \
    static void __cdecl f(void); \
    __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)

#elif defined(__GNUC__)
    #define PH_INITIALIZER(f) \
    static void f(void) __attribute__((constructor, used)); \
    static void f(void)
#endif

#ifdef __APPLE__
    #ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
        #define PH_IPHONE_OS
        #define PH_MOBILE
        #define PH_GLES
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
    #ifdef __ANDROID__
        #define PH_ANDROID
        #define PH_MOBILE
        #define PH_GLES
    #else
        #define PH_DESKTOP
    #endif
#endif

#if !defined(PH_MOBILE) && !defined(PH_DESKTOP)
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

//map<string,PHAllocator> * list;
#define PH_REGISTERCLASS(list,name,clss) PH_INITIALIZER( PH_TOKENPASTE(PH_TOKENPASTE(PHRegister_, clss), PH_UNIQUE_TOKEN)) { if (!list) list = new map<string,PHAllocator>; list->insert(make_pair<string,void * (*)(void)>(name,PHAlloc<clss>)); }

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

#include <Porkholt/Core/PHGL.h>
#include <Porkholt/Core/PHUtilities.h>
#include <Porkholt/Core/PHInvocation.h>
#include <Porkholt/Core/PHObject.h>
#include <Porkholt/Geometry/PHGeometry.h>
#include <Porkholt/Core/PHErrors.h>

class PHThread;
class PHMutex;
class PHSemaphore;

#endif
