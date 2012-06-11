/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHErrors.h>
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
