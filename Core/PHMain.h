/*
 *  PHMain.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/16/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef PHMAIN_H
#define PHMAIN_H

#include <cstdlib>
#include <cmath>
#include <cstdio>

#ifdef __APPLE__
	#define PH_IPHONE_OS
	#ifdef TARGET_IPHONE_SIMULATOR
		#define PH_SIMULATOR
	#endif
#endif

#define PHLog(x,args...) printf(x,args)

#include "PHObject.h"
#include "PHGeometry.h"

#ifdef PH_IPHONE_OS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
#endif

#endif