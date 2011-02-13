/*
 *  PHErrors.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHErrors.h"
#include <cstdarg>

string PHIOError("I/O Error");
string PHInvalidFileFormat("Invalid File Format");

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
