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
#import <CoreVideo/CoreVideo.h>

extern PHWindow * PHWMainWindow;

int PHWMain(int argc, char * argv[], const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHAppDelegate * del = [[PHAppDelegate alloc] initWithVM: vm flags: flags entryPoint:entryPoint ud:ud ];
    [[NSApplication sharedApplication] setDelegate:del];
    [NSApp run];
    [del release];
    return 0;
}

void PHWSetVideoMode(const PHWVideoMode & vm)
{
	[PHWMainWindow setVideoMode:vm];
}

PHWVideoMode PHWGetVideoMode()
{
    return PHWMainWindow.videoMode;
}

const vector<PHWVideoMode> & PHWSupportedModes()
{
    static vector<PHWVideoMode> v;
    static bool inited = false;
    if (!inited)
    {
        inited = true;
        NSRect r = [[NSScreen mainScreen] frame];
        
        CVDisplayLinkRef displayLink = nil;
        CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
        CVTime tm = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(displayLink);
        double grefresh = round(double(tm.timeScale)/tm.timeValue);
        CVDisplayLinkRelease(displayLink);
        
        v.push_back(PHWVideoMode(800, 600, grefresh, PHWVideoMode::Windowed));
        v.push_back(PHWVideoMode(r.size.width, r.size.height, grefresh, PHWVideoMode::FullscreenWindowed));
        
        set<pair< pair<int, int>,double> > s;
        
        CGDirectDisplayID d = kCGDirectMainDisplay;
        CFArrayRef modelist = CGDisplayCopyAllDisplayModes(d, NULL);
        int count = CFArrayGetCount(modelist);
        for (int i=0; i<count; i++)
        {
            CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modelist, i);
            long width = CGDisplayModeGetWidth(mode);
            long height = CGDisplayModeGetHeight(mode);
            double refresh = CGDisplayModeGetRefreshRate(mode);
            
            CFStringRef pixelEncoding = CGDisplayModeCopyPixelEncoding(mode);
            if (CFStringCompare(pixelEncoding,CFSTR(IO32BitDirectPixels),0)!=kCFCompareEqualTo)
            {
                CFRelease(pixelEncoding);
                continue;
            }
            CFRelease(pixelEncoding);
            if (refresh == 0)
                refresh = grefresh;
            if (s.count(make_pair(make_pair(width, height),refresh)))
                continue;
            s.insert(make_pair(make_pair(width, height),refresh));
            v.push_back(PHWVideoMode(width, height, refresh, PHWVideoMode::Fullscreen));
        }
        CFRelease(modelist);
        
        sort(v.begin(), v.end());
    }
    return v;
}

void PHWClose()
{
    [PHWMainWindow close];
}

void PHWSetTitle(const string & title)
{
    NSAutoreleasePool * ap = [[NSAutoreleasePool alloc] init];
    PHWMainWindow.title = [NSString stringWithUTF8String:title.c_str()];
    [ap drain];
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