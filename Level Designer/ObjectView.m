//
//  ObjectView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ObjectView.h"
#import "PHObject.h"

@implementation ObjectView
@synthesize selected;
@synthesize rotation;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

-(void)dealloc
{
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
	
	NSPoint loc = object.position;
	[self setFrame:NSMakeRect(loc.x-0.04, loc.y-0.04, 0.08f, 0.08f)];
	[self setRotation:object.rotation];
}

-(PHObject*)object
{
	return object;
}

-(double)rotation
{
	return rotation;
}

-(void)setRotation:(double)rot
{
//	CGSize size = self.frame.size;
	rotation = rot;
	NSRect frame = [self frame];
	NSPoint center = frame.origin;
	center.x+=frame.size.width/2;
	center.y+=frame.size.height/2;
	[self setFrameCenterRotation:rot];
	[self setFrameOrigin:center];
}

@end
