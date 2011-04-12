//
//  WorldView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/11/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "WorldView.h"


@implementation WorldView

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

#define MAJORLINES 1.0f
#define MEDIUMLINES 0.5f
#define MINORLINES 0.1f
#define MAJORWIDTH 0.02f
#define MEDIUMWIDTH 0.01f
#define MINORWIDTH 0.01f

- (void)drawRect:(NSRect)dirtyRect {
	[[NSColor blackColor] setFill];
	[NSBezierPath fillRect:dirtyRect];
	[NSBezierPath setDefaultLineWidth:MINORWIDTH];
	[[NSColor colorWithCalibratedRed:0 green:0.4f blue:0 alpha:1] setStroke];
	for (float sp = floor(dirtyRect.origin.x/MINORLINES)*MINORLINES; sp<=dirtyRect.origin.x+dirtyRect.size.width; sp+=MINORLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(sp, dirtyRect.origin.y) toPoint:NSMakePoint(sp, dirtyRect.origin.y+dirtyRect.size.height)];
	}
	for (float sp = floor(dirtyRect.origin.y/MINORLINES)*MINORLINES; sp<=dirtyRect.origin.y+dirtyRect.size.height; sp+=MINORLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(dirtyRect.origin.x,sp) toPoint:NSMakePoint(dirtyRect.origin.x+dirtyRect.size.width,sp)];
	}
	[NSBezierPath setDefaultLineWidth:MEDIUMWIDTH];
	[[NSColor colorWithCalibratedRed:0 green:0.7f blue:0 alpha:1] setStroke];
	for (float sp = floor(dirtyRect.origin.x/MEDIUMLINES)*MEDIUMLINES; sp<=dirtyRect.origin.x+dirtyRect.size.width; sp+=MEDIUMLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(sp, dirtyRect.origin.y) toPoint:NSMakePoint(sp, dirtyRect.origin.y+dirtyRect.size.height)];
	}
	for (float sp = floor(dirtyRect.origin.y/MEDIUMLINES)*MEDIUMLINES; sp<=dirtyRect.origin.y+dirtyRect.size.height; sp+=MEDIUMLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(dirtyRect.origin.x,sp) toPoint:NSMakePoint(dirtyRect.origin.x+dirtyRect.size.width,sp)];
	}
	[NSBezierPath setDefaultLineWidth:MAJORWIDTH];
	[[NSColor colorWithCalibratedRed:0 green:1.0f blue:0 alpha:1] setStroke];
	for (float sp = floor(dirtyRect.origin.x/MAJORLINES)*MAJORLINES; sp<=dirtyRect.origin.x+dirtyRect.size.width; sp+=MAJORLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(sp, dirtyRect.origin.y) toPoint:NSMakePoint(sp, dirtyRect.origin.y+dirtyRect.size.height)];
	}
	for (float sp = floor(dirtyRect.origin.y/MAJORLINES)*MAJORLINES; sp<=dirtyRect.origin.y+dirtyRect.size.height; sp+=MAJORLINES)
	{
		[NSBezierPath strokeLineFromPoint:NSMakePoint(dirtyRect.origin.x,sp) toPoint:NSMakePoint(dirtyRect.origin.x+dirtyRect.size.width,sp)];
	}
	[[NSColor redColor] setFill];
	[NSBezierPath fillRect:NSMakeRect(-0.05f, -0.05f, 0.1f, 0.1f)];
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}
@end
