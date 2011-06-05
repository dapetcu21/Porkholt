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
    
    WorldController * controller;
}

-(void)beginDragging:(NSEvent *)theEvent;
-(void)cancelAllDrags;

@property(nonatomic,assign) WorldController * controller;

@end
