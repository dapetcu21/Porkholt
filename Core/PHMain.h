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
#include <string>
using namespace std;

#ifdef __APPLE__
	#define PH_IPHONE_OS
	#define PH_DARWIN
	#ifdef TARGET_IPHONE_SIMULATOR
		#define PH_SIMULATOR
	#endif
#endif

#include "PHObject.h"
#include "PHGeometry.h"
#include "PHErrors.h"
#include "PHFileManager.h"
#include "PHImage.h"

#ifndef INSIDE_PHVIEW_H
	#include "PHView.h"
	#include "PHTestView.h"
	#include "PHImageView.h"
#endif

#ifdef PH_IPHONE_OS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
#endif

#endif