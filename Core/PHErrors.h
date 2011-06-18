/*
 *  PHErrors.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHERRORS_H
#define PHERRORS_H
#include "PHMain.h"

void PHLog(const char * str, ...);
void PHLog(const string & str, ...);

extern string PHIOError;
extern string PHInvalidFileFormat;
extern string PHSysCallError;
#endif
