//
//  PHWindow.h
//  Porkholt
//
//  Created by Marius Petcu on 4/11/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

#ifndef NSAppKitVersionNumber10_7 
#define NSAppKitVersionNumber10_7 1110 
#endif

@interface PHWindow : NSWindow
{
    BOOL fullScreen;
    NSRect windowedFrame;
    NSUInteger windowedMask;
}

-(void)toggleFullScreen:(id)sender;
-(void)setFullScreen:(BOOL)fs animated:(BOOL)anim;

@end
