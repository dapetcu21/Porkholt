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
#include <climits>
#include <cfloat>
using namespace std;

#ifdef __APPLE__
	#define PH_IPHONE_OS
	#define PH_DARWIN
	#ifdef TARGET_IPHONE_SIMULATOR
		#define PH_SIMULATOR
	#endif
#endif

#ifdef PH_IPHONE_OS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
#endif

#include "PHTime.h"
#include "PHObject.h"
#include "PHGeometry.h"
#include "PHErrors.h"
#include "PHFileManager.h"
#include "PHImage.h"
#include "PHTouch.h"
#include "PHEventHandler.h"
#include "PHMotion.h"
#include "PHThread.h"
#include "PHMutex.h"
#include "PHRandom.h"

#ifndef INSIDE_PHVIEW_H
	#include "PHView.h"
	#include "PHTestView.h"
	#include "PHImageView.h"
	#include "PHButtonView.h"
#endif

#include "PHMainEvents.h"

#ifndef INSIDE_PHVIEWCONTROLLER_H
	#include "PHViewController.h"
	#include "PHTestViewController.h"
	#include "PHNavigationController.h"
#endif

#include "PHLevelSystem.h"

#endif