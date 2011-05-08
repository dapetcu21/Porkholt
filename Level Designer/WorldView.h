//
//  WorldView.h
//  Level Designer
//
//  Created by Marius Petcu on 4/11/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface WorldView : NSView {
	int dragState;
	NSRect dragRect;
	NSPoint dragPoint;
}

-(void)beginDragging:(NSEvent *)theEvent;

@end
