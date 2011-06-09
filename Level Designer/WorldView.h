//
//  WorldView.h
//  Level Designer
//
//  Created by Marius Petcu on 4/11/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class WorldController;
@interface WorldView : NSView {
	int dragState;
	NSRect dragRect;
	NSPoint dragPoint,initialPoint;
    NSPoint opnt,vel;
    WorldController * controller;
    NSTimer * inscrolltimer;
    int ins,insn;
}

-(void)beginDragging:(NSEvent *)theEvent;
-(void)cancelAllDrags;

-(IBAction)scrollToOrigin:(id)sender;

@property(nonatomic,assign) WorldController * controller;
@property(nonatomic,assign) NSPoint position;

@end
