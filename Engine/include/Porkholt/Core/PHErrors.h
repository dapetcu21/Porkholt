/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHERRORS_H
#define PHERRORS_H

#include <Porkholt/Core/PHUtilities.h>

void PHLog(const char * str, ...);
void PHLog(const string & str, ...);
void PHGLCheckError_(const char * file, int line);

inline void PHNop() {};

#ifdef PH_DEBUG
#define PHGLCheckError() PHGLCheckError_(__FILE__, __LINE__)
#define PHGLClearError PHGL::glGetError
#else
#define PHGLCheckError PHNop
#define PHGLClearError PHNop
#endif

extern string PHIOError;
extern string PHInvalidFileFormat;
extern string PHSysCallError;
extern string PHLuaError;
#endif
