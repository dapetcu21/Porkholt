//
//  OverlayWindow.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "OverlayWindow.h"

@implementation OverlayWindow

- (BOOL) canBecomeKeyWindow { return YES; }
- (BOOL) canBecomeMainWindow { return NO; }
- (BOOL) acceptsFirstResponder { return YES; }
- (BOOL) becomeFirstResponder { return YES; }
- (BOOL) resignFirstResponder { return YES; }

-(void)undo:(id)sender
{
    //this must be here or else the undo won't work
}

-(void)redo:(id)sender
{
    //this must be here or else the redo won't work
}

@end
