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
    int selectModifier;
	NSRect dragRect;
	NSPoint dragPoint,initialPoint;
    NSPoint opnt,vel;
    WorldController * controller;
    NSTimer * inscrolltimer;
    int ins,insn;
    double rotated;
    BOOL rotating;
    id sender;
    BOOL isZooming;
}

-(void)beginDragging:(NSEvent *)theEvent;
-(void)cancelAllDrags;

-(IBAction)scrollToOrigin:(id)sender;

- (void)rotate:(double)amount;
- (void)endRotate;

@property(nonatomic,assign) WorldController * controller;
@property(nonatomic,assign) NSPoint position;
@property(nonatomic,assign) id sender;

@end
