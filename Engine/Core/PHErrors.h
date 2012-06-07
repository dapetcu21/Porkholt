/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHERRORS_H
#define PHERRORS_H
#include "PHMain.h"

void PHLog(const char * str, ...);
void PHLog(const string & str, ...);

extern string PHIOError;
extern string PHInvalidFileFormat;
extern string PHSysCallError;
extern string PHLuaError;
#endif
