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
#include <utility>
using namespace std;

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

#ifdef PH_IPHONE_OS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#endif
#ifdef PH_MAC_OS
    #import <OpenGL/gl.h>
    #import <OpenGL/glext.h>
#endif

struct lua_State; 

#include "PHTime.h"
#include "PHObject.h"
#include "PHMessage.h"
#include "PHGeometry.h"
#include "PHErrors.h"

#include "PHThread.h"
#include "PHMutex.h"
#include "PHSemaphore.h"

#endif