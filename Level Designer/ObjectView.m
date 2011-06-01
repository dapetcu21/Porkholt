//
//  ObjectView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ObjectView.h"
#import "PHObject.h"
#import "ObjectController.h"
#import "WorldView.h"

@implementation ObjectView
@synthesize selected;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

-(void)dealloc
{
	object.view = nil;
	[object release];
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
	if (selected)
		[[NSColor blueColor] setFill];
	else
		[[NSColor whiteColor] setFill];
    NSRectFill(dirtyRect);
}

-(void)setObject:(PHObject *)obj
{
	[obj retain];
	object.view = nil;
	[object release];
	object = obj;
	object.view = self;
}

-(PHObject*)object
{
	return object;
}

-(void)mouseDown:(NSEvent *)theEvent
{
	NSView * view = (WorldView*)[self superview];
	if ([view isKindOfClass:[WorldView class]])
	{
		if ([theEvent modifierFlags] & NSShiftKeyMask)
			[super mouseDown:theEvent];
		else
		{
			if (object.readOnly)
            {
				[super mouseDown:theEvent];
                return;
            }
			if (!object.selected)
				[[object.controller arrayController] setSelectedObjects:[NSArray arrayWithObject:object]];
			[(WorldView*)view beginDragging:theEvent];
		}
	} else {
		[super mouseDown:theEvent];
	}

}

@end
