//
//  PHX11.h
//  Porkholt
//
//  Created by Marius Petcu on 2/7/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHX11_H
#define PHX11_H

#include "PHMain.h"
#include "PHWindowing.h"

void PHXCreateContext(const string & title, int & resX, int & resY, int & refresh, int fstype, int flags);
void PHXSetVideoMode(int & w, int & h, int & r, int t, bool resize);
void PHXDestroyContext();
void PHXTerminate();
void PHXMainLoop();
void PHXSetVSync(bool vsync);
void PHXSetTitle(const string & s);
const vector<PHWVideoMode> & PHXVideoModes();


#endif
