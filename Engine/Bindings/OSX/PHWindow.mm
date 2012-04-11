//
//  PHWindow.mm
//  Porkholt
//
//  Created by Marius Petcu on 4/11/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import "PHWindow.h"

@implementation PHWindow

-(void)toggleFullScreen:(id)sender
{
    if (floor(NSAppKitVersionNumber)>=NSAppKitVersionNumber10_7)
        [super toggleFullScreen:sender];
    else
        [self setFullScreen:!fullScreen animated:YES];
}

-(void)setFullScreen:(BOOL)fs animated:(BOOL)anim
{
    if (fullScreen == fs) return;
    fullScreen = fs;
    if (fullScreen)
    {
        windowedFrame = self.frame;
        windowedMask = self.styleMask;
        NSRect r = [NSScreen mainScreen].frame;
        [self setStyleMask:NSBorderlessWindowMask];
        [self setFrame:r display:YES animate:anim];
        [NSApp
         setPresentationOptions:   NSApplicationPresentationAutoHideMenuBar
         | NSApplicationPresentationAutoHideDock];
    } else {
        [self setStyleMask:windowedMask];
        [self setFrame:windowedFrame display:YES animate:anim];
        [NSApp
         setPresentationOptions: 0];
    }
}

@end
