/*
 *  PHErrors.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHErrors.h"
#include <cstdarg>

string PHIOError("I/O Error");
string PHInvalidFileFormat("Invalid File Format");
string PHSysCallError("System Call Error");
string PHLuaError("Lua Error");

void PHLog(const char * str, ...)
{
	va_list al;
	va_start(al,str);
	fprintf(stderr, "Porkholt: ");
	vfprintf(stderr, str, al);
	fprintf(stderr, "\n");
	va_end(al);
}

void PHLog(const string & str, ...)
{
	va_list al;
	va_start(al,str);
	fprintf(stderr, "Porkholt: ");
	vfprintf(stderr, str.c_str(), al);
	fprintf(stderr, "\n");
	va_end(al);
}