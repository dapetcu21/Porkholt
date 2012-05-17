//
//  PHStartGame.mm
//  Porkholt
//
//  Created by Marius Petcu on 2/7/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#import "PHWindowing.h"
#import "PHAppDelegate.h"
#import "PHGLView.h"
#import "PHWindow.h"

extern PHWindow * PHWMainWindow;

int PHWMain(int argc, char * argv[], const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    [[NSApplication sharedApplication] setDelegate:[[PHAppDelegate alloc] initWithVM: vm flags: flags entryPoint:entryPoint ud:ud ]];
    [NSApp run];
    return 0;
}

void PHWSetVideoMode(const PHWVideoMode & vm)
{
    PHWMainWindow.videoMode = vm;
}

PHWVideoMode PHWGetVideoMode()
{
    return PHWMainWindow.videoMode;
}

static const int resolutions[] = {
    2880, 900,
    2560, 1600,
    2048, 1280,
    2048, 1152,
    1920, 1440,
    1920, 1200,
    1920, 1080,
    1856, 1392,
    1800, 1440,
    1792, 1344,
    1600, 900,
    1600, 1200,
    1440, 900,
    1400, 1050,
    1366, 768,
    1280, 800,
    1280, 720,
    1280, 1024,
    1152, 720,
    1024, 768,
    1024, 640,
    1024, 576,
    960, 540,
    800, 480,
    800, 600
};

const vector<PHWVideoMode> & PHWSupportedModes()
{
    static vector<PHWVideoMode> v;
    static bool inited = false;
    if (!inited)
    {
        inited = true;
        NSRect r = [[NSScreen mainScreen] frame];
        
        v.push_back(PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed));
        v.push_back(PHWVideoMode(r.size.width, r.size.height, 60, PHWVideoMode::FullscreenWindowed));
        v.push_back(PHWVideoMode(r.size.width, r.size.height, 60, PHWVideoMode::Fullscreen));
        
        for (int i=0; i<(sizeof(resolutions)/sizeof(int)); i+=2)
            if ((resolutions[i] < r.size.width) && (resolutions[i+1] < r.size.height))
                v.push_back(PHWVideoMode(resolutions[i], resolutions[i+1], 60, PHWVideoMode::Fullscreen));
    }
    return v;
}

void PHWClose()
{
    [PHWMainWindow close];
}

void PHWSetTitle(const string & title)
{
    PHWMainWindow.title = [NSString stringWithUTF8String:title.c_str()];
}

void PHWSetVSync(bool vsync)
{
    PHWMainWindow.view.verticalSync = vsync;
}

bool PHWGetVSync()
{
    return PHWMainWindow.view.verticalSync;
}

void PHWSetResizable(bool r)
{
    PHWMainWindow.resizable = r;
}

bool PHWGetResizable()
{
    return PHWMainWindow.resizable;
}