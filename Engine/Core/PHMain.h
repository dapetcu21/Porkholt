/*
 *  PHMain.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/16/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHMAIN_H
#define PHMAIN_H

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <string>
#include <climits>
#include <cfloat>

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
        #ifdef __i386__
            #define PH_SIMULATOR
        #endif
    #else
        #define PH_MAC_OS
    #endif
	#define PH_DARWIN
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

#if defined(DEBUG) || defined(_DEBUG)
    #define PH_DEBUG
#endif

template< bool cond > struct _PH_STATIC_ASSERTION_FAILURE;
template <> struct _PH_STATIC_ASSERTION_FAILURE< true > {};
template <> struct _PH_STATIC_ASSERTION_FAILURE< false > {};
#define _PH_TOKENPASTE(x, y) x ## y
#define PH_TOKENPASTE(x, y) _PH_TOKENPASTE(x, y)
#define PH_STATIC_ASSERT(cond) enum { PH_TOKENPASTE(dummy,__LINE__) = sizeof(_PH_STATIC_ASSERTION_FAILURE< (bool)(cond) >) }

struct lua_State; 

#ifdef __GNUC__
#define PH_PACKED_STRUCT __attribute__((packed))
#elif _MSC_VER
#define PH_PACKED_STRUCT_PRAGMA
#endif

#include "PHInvocation.h"
#include "PHTime.h"
#include "PHObject.h"
#include "PHMessage.h"
#include "PHGeometry.h"
#include "PHErrors.h"

#include "PHThread.h"
#include "PHMutex.h"
#include "PHSemaphore.h"

#endif