/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <cstdarg>

#ifdef PH_ANDROID
#include <android/log.h>
#endif

string PHIOError("I/O Error");
string PHInvalidFileFormat("Invalid File Format");
string PHSysCallError("System Call Error");
string PHLuaError("Lua Error");

void PHGLCheckError_(const char * file, int line)
{
    GLenum e = PHGL::glGetError(); 
    if (e!=GL_NO_ERROR) 
        PHLog("OpenGL error 0x%x in %s:%d", int(e), file, line);   
}

#ifdef PH_ANDROID
void PHLog(const char * str, ...)
{
	va_list al;
	va_start(al,str);
	__android_log_vprint(ANDROID_LOG_INFO, "Porkholt", str, al);
	va_end(al);
}

void PHLog(const string & str, ...)
{
	va_list al;
	va_start(al,str);
	__android_log_vprint(ANDROID_LOG_INFO, "Porkholt", str.c_str(), al);
	va_end(al);
}

#elif defined(PH_DARWIN)

extern "C" void PHNSLog(const char * str, va_list args);

void PHLog(const char * str, ...)
{
    va_list al;
    va_start(al, str);
    PHNSLog(str, al);
    va_end(al);
}

void PHLog(const string & str, ...)
{
    va_list al;
    va_start(al, str);
    PHNSLog(str.c_str(), al);
    va_end(al);
}

#else

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

#endif
