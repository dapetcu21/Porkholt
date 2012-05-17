//
//  PHWindow.mm
//  Porkholt
//
//  Created by Marius Petcu on 4/11/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import "PHWindow.h"

@implementation PHWindow

#define stylemask(windowed, resizable) ((windowed) ? (NSClosableWindowMask | NSTitledWindowMask | ((resizable)?NSResizableWindowMask:0)): NSBorderlessWindowMask)

-(id)initWithVM:(const PHWVideoMode &) _vm flags:(int)flags entryPoint:(void (*)(PHGameManager *))entryPoint ud:(void*)ud
{
    NSUInteger mask = stylemask((_vm.type == PHWVideoMode::Windowed),(flags & PHWResizable));
    
    NSRect r = [NSScreen mainScreen].frame;
    if (_vm.type == PHWVideoMode::Windowed)
        r = NSMakeRect((r.size.width - _vm.width)/2,(r.size.height - _vm.height)/2, _vm.width, _vm.height);
    
    if (self = [super initWithContentRect:r styleMask:mask backing:NSBackingStoreBuffered defer:NO])
    {
        vm = _vm;
        if (_vm.type != PHWVideoMode::Windowed)
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
        if (_vm.type == PHWVideoMode::Fullscreen)
        {
            [self setLevel:NSMainMenuWindowLevel+1];
            [self setHidesOnDeactivate:YES];
            [view setManualSize:NSMakeSize(_vm.width, _vm.height)];
        }
    } 
    return self;
}

-(void)dealloc
{
    view.gameManager->appQuits();
    [view release];
    [title release];
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
    view.gameManager->appResumed();
}

-(void)resignKeyWindow
{
    [super resignKeyWindow];
    view.gameManager->appSuspended();
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
        [self setCollectionBehavior:r?NSWindowCollectionBehaviorFullScreenPrimary:NSWindowCollectionBehaviorDefault];
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
            
    NSRect r = [NSScreen mainScreen].frame;
    if (vdm.type == PHWVideoMode::Windowed)
        r = NSMakeRect((r.size.width - vdm.width)/2,(r.size.height - vdm.height)/2, vdm.width, vdm.height);

    [self setStyleMask:mask];
    [super setTitle:title];
    [self setFrame:[NSWindow frameRectForContentRect:r styleMask:mask] display:YES animate:NO];
    if (vdm.type == PHWVideoMode::Windowed)
        [NSApp setPresentationOptions:0];
    else
        [NSApp setPresentationOptions:
            NSApplicationPresentationAutoHideMenuBar
            | NSApplicationPresentationAutoHideDock];
    
    if (vdm.type == PHWVideoMode::Fullscreen)
    {
        [self setLevel:NSMainMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        [view setManualSize:NSMakeSize(vdm.width, vdm.height)];
    } else {
        [self setHidesOnDeactivate:NO];
        [self setLevel:NSNormalWindowLevel];
        [view setAutomaticSize];
    }
    
    [self makeKeyAndOrderFront:self];
    vm = vdm;
}

@end
