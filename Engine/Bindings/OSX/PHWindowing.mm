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

int PHWMain(int argc, char * argv[], const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    [[NSApplication sharedApplication] setDelegate:[[PHAppDelegate alloc] initWithResX: vm.width resY: vm.height flags: flags entryPoint:entryPoint ud:ud ]];
    [NSApp run];
    return 0;
}

void * PHCreateWindow(const string & title, unsigned int resolutionX, unsigned int resolutionY, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    NSRect r = [NSScreen mainScreen].frame;
    NSUInteger mask;
    r = NSMakeRect((r.size.width - resolutionX)/2,(r.size.height - resolutionY)/2, resolutionX, resolutionY);
    mask = (((flags&PHStartGame_Resizable)?NSResizableWindowMask:0) | NSClosableWindowMask | NSTitledWindowMask);
    PHWindow * window = [[PHWindow alloc] initWithContentRect:r styleMask:mask backing:NSBackingStoreBuffered defer:NO];
    PHGLView * view = [[PHGLView alloc] initWithFrame:NSMakeRect(0,0,resolutionX, resolutionY) resourcePath:[[NSBundle mainBundle] resourcePath] entryPoint:entryPoint flags:flags];
    [view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [window.contentView addSubview:view];
    [view release];
    [window makeKeyAndOrderFront:nil];
    if (flags & (PHStartGame_Resizable | PHStartGame_FullScreen))
    {
        if (floor(NSAppKitVersionNumber)>=NSAppKitVersionNumber10_7)
        {
            [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
            if (flags & PHStartGame_FullScreen)
                [window performSelector:@selector(toggleFullScreen:) withObject:nil afterDelay:0.0f];
        } else {
            if (flags & PHStartGame_FullScreen)
                [window setFullScreen:YES animated:NO];
        }
    }
    PHWindowSetTitle((void*)window, title);
    return window;
}

void PHResizeWindow(void * window, unsigned int resolutionX, unsigned int resolutionY)
{
    PHWindow * w = (PHWindow*)window;
    NSRect r = [w contentRectForFrameRect:w.frame];
    r.origin.x = r.origin.x - (resolutionX - r.size.width)/2;
    r.origin.y = r.origin.y - (resolutionY - r.size.height)/2;
    r.size.width = resolutionX;
    r.size.height = resolutionY;
    r = [w frameRectForContentRect:r];
    [w setFrame:r display:YES animate:YES];
}

void PHWindowSetFullScreen(void * window, bool fullScreen)
{
    PHWindow * w = (PHWindow*)window;
    if (floor(NSAppKitVersionNumber)>=NSAppKitVersionNumber10_7)
    {
        bool fs = ((w.styleMask & NSFullScreenWindowMask) != 0);
        if (fullScreen != fs)
            [w toggleFullScreen:nil];
    } else {
        [w setFullScreen:fullScreen animated:YES];
    }
}

void PHWindowSetTitle(void * window, const string & title)
{
    PHWindow * w = (PHWindow*)window;
    [w setTitle:[NSString stringWithUTF8String:title.c_str()]];
}

void PHCloseWindow(void * window)
{
    PHWindow * w = (PHWindow*)window;
    [w close];
    [w release];
}
