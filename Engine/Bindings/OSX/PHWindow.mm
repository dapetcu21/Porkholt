//
//  PHWindow.mm
//  Porkholt
//
//  Created by Marius Petcu on 4/11/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import "PHWindow.h"
#import <CoreVideo/CoreVideo.h>

@implementation PHWindow

#define stylemask(windowed, resizable) ((windowed) ? (NSClosableWindowMask | NSTitledWindowMask | ((resizable)?NSResizableWindowMask:0)): NSBorderlessWindowMask)

-(id)initWithVM:(const PHWVideoMode &) _vm flags:(int)flags entryPoint:(void (*)(PHGameManager *))entryPoint ud:(void*)ud
{
    NSUInteger mask = stylemask((_vm.type == PHWVideoMode::Windowed),(flags & PHWResizable));
    
    PHWVideoMode vmm = _vm;
#ifndef PH_OSX_FAKERESOLUTION
    if (vmm.type == PHWVideoMode::Fullscreen)
        [PHWindow setResolution:vmm];
#endif
    
    NSRect r = [NSScreen mainScreen].frame;
    if (_vm.type == PHWVideoMode::Windowed)
        r = NSMakeRect((r.size.width - vmm.width)/2,(r.size.height - vmm.height)/2, vmm.width, vmm.height);
    
    if (self = [super initWithContentRect:r styleMask:mask backing:NSBackingStoreBuffered defer:NO])
    {
        vm = vmm;
        if (vm.type != PHWVideoMode::Windowed)
        {
            vm.width = r.size.width;
            vm.height = r.size.height;
            [NSApp setPresentationOptions: NSApplicationPresentationAutoHideMenuBar | NSApplicationPresentationAutoHideDock];
        }
        if ((flags & PHWResizable) && (floor(NSAppKitVersionNumber)>=NSAppKitVersionNumber10_7))
        {
            [self setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        }
        resizable = (flags & PHWResizable);
        
        [self setOpaque:YES];
        
        view = [[PHGLView alloc] initWithFrame:NSMakeRect(0,0,vm.width,vm.height) resourcePath:[[NSBundle mainBundle] resourcePath] entryPoint:entryPoint flags:flags];
        [view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [self.contentView addSubview:view];
        if (vm.type == PHWVideoMode::Fullscreen)
        {
#ifdef PH_OSX_FAKERESOLUTION
            [self setLevel:NSMainMenuWindowLevel+2];
            [self setHidesOnDeactivate:YES];
            [view setManualSize:NSMakeSize(vm.width, vm.height)];
#else
            [self setLevel:CGShieldingWindowLevel()];
#endif
        }
    } 
    return self;
}

-(void)dealloc
{
    view.gameManager->appQuits();
    [view release];
    [title release];
#ifndef PH_OSX_FAKERESOLUTION
    if (vm.type == PHWVideoMode::Fullscreen)
        [PHWindow restoreResolution];
#endif
    [super dealloc];
    
}

-(void)setTitle:(NSString*)t
{
    [t retain];
    [title release];
    title = t;
    [super setTitle:t];
}

-(void)becomeKeyWindow
{
    [super becomeKeyWindow];
#ifndef PH_OSX_FAKERESOLUTION
    if (vm.type == PHWVideoMode::Fullscreen)
        [PHWindow undoRestoreResolution];
#endif
    view.gameManager->appResumed();
}

-(void)resignKeyWindow
{
    view.gameManager->appSuspended();
#ifndef PH_OSX_FAKERESOLUTION
    [PHWindow restoreResolution];
#endif
    [super resignKeyWindow];
}

-(BOOL)resizable
{
    return resizable;
}

-(void)setResizable:(BOOL)r
{
    if (resizable == r) return;
    resizable = r;
    if (floor(NSAppKitVersionNumber)>=NSAppKitVersionNumber10_7)
        [self setCollectionBehavior:(r?((int)NSWindowCollectionBehaviorFullScreenPrimary):((int)NSWindowCollectionBehaviorDefault))];
    NSUInteger mask = stylemask((vm.type == PHWVideoMode::Windowed),r);
    [self setStyleMask:mask];
}

-(PHGLView*)view 
{ 
    return view;
}

-(PHWVideoMode)videoMode
{
    vm.width = view.gameManager->screenWidth();
    vm.height = view.gameManager->screenHeight();
    return vm;
}

-(void)setVideoMode:(PHWVideoMode)vdm
{
    NSUInteger mask = stylemask((vdm.type == PHWVideoMode::Windowed),resizable);
    
    vm = vdm;
    
#ifndef PH_OSX_FAKERESOLUTION
    if (vm.type == PHWVideoMode::Fullscreen)
        [PHWindow setResolution:vm];
    else
        [PHWindow restoreResolution];
#endif
    
    NSRect r = [NSScreen mainScreen].frame;
    if (vm.type == PHWVideoMode::Windowed)
        r = NSMakeRect((r.size.width - vm.width)/2,(r.size.height - vm.height)/2, vm.width, vm.height);

    [self setStyleMask:mask];
    [super setTitle:title];
    [self setFrame:[NSWindow frameRectForContentRect:r styleMask:mask] display:YES animate:NO];
    if (vdm.type == PHWVideoMode::Windowed)
        [NSApp setPresentationOptions:0];
    else
        [NSApp setPresentationOptions:
            NSApplicationPresentationAutoHideMenuBar
            | NSApplicationPresentationAutoHideDock];
    
#ifdef PH_OSX_FAKERESOLUTION
    if (vm.type == PHWVideoMode::Fullscreen)
    {
        [self setLevel:NSMainMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        [view setManualSize:NSMakeSize(vdm.width, vdm.height)];
    } else {
        [self setHidesOnDeactivate:NO];
        [self setLevel:NSNormalWindowLevel];
        [view setAutomaticSize];
    }
#else
    if (vm.type == PHWVideoMode::Fullscreen)
        [self setLevel:CGShieldingWindowLevel()]; 
    else
        [self setLevel:NSNormalWindowLevel];
#endif
    
    CVDisplayLinkRef displayLink = nil;
	CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
	CVTime tm = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(displayLink);
    view.refreshRate = round(double(tm.timeScale)/tm.timeValue);
	CVDisplayLinkRelease(displayLink);
    
    [self makeKeyAndOrderFront:self];
}

#ifndef PH_OSX_FAKERESOLUTION

static CGDisplayModeRef PHCurrentMode = NULL;
static CGDisplayModeRef PHOrigMode = NULL;

+(void)setResolution:(PHWVideoMode &)vm
{
    CGDirectDisplayID d = kCGDirectMainDisplay;
    CFArrayRef modelist = CGDisplayCopyAllDisplayModes(d, NULL);
    int count = CFArrayGetCount(modelist);
    CGDisplayModeRef md = NULL;
    int mdiff = 0x3f3f3f3f;
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
        int diff = abs(vm.width  - width) + abs(vm.height - height) + abs(vm.refresh - refresh);
        if (diff<mdiff)
        {
            md = mode;
            mdiff = diff;
        }
    }
    CFRetain(md);
    CFRelease(modelist);
    if (PHCurrentMode)
    {
        CFRelease(PHCurrentMode);
    } else {
        if (PHOrigMode)
            CFRelease(PHOrigMode);
        PHOrigMode = CGDisplayCopyDisplayMode(d);
    }
    PHCurrentMode = md;
    if (!CGDisplayIsCaptured(d))
        CGDisplayCapture(d);
    CGDisplaySetDisplayMode(d, md, NULL);
}

+(void)restoreResolution
{
    if (!PHCurrentMode)
        return;
    CGDirectDisplayID d = kCGDirectMainDisplay;
    CGDisplaySetDisplayMode(d, PHOrigMode, NULL);
    if (CGDisplayIsCaptured(d))
        CGDisplayRelease(d);
    CFRelease(PHOrigMode);
    PHOrigMode = PHCurrentMode;
    PHCurrentMode = NULL;
}

+(void)undoRestoreResolution
{
    CGDirectDisplayID d = kCGDirectMainDisplay;
    if (PHCurrentMode)
        return;
    if (!PHOrigMode)
        return;
    PHCurrentMode = PHOrigMode;
    PHOrigMode = CGDisplayCopyDisplayMode(d);
    if (!CGDisplayIsCaptured(d))
        CGDisplayCapture(d);
    CGDisplaySetDisplayMode(d, PHCurrentMode, NULL);
}
#endif

@end
