/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHX11_H
#define PHX11_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHWindowing.h>

void PHXCreateContext(const string & title, int & resX, int & resY, int & refresh, int fstype, int flags);
void PHXSetVideoMode(int & w, int & h, int & r, int t, bool resize);
void PHXDestroyContext();
void PHXTerminate();
void PHXMainLoop();
void PHXSetVSync(bool vsync);
void PHXSetTitle(const string & s);
const vector<PHWVideoMode> & PHXVideoModes();


#endif
