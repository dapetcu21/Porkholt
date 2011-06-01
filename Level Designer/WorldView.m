//
//  WorldView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/11/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "WorldView.h"
#import "ObjectView.h"
#import "PHObject.h"

@implementation WorldView

enum dragStates
{
	dsNone = 0,
	dsSelect,
	dsMove,
	NUMDRAGSTATES
};

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
	dirtyRect.origin.x-=MAJORWIDTH;
	dirtyRect.origin.y-=MAJORWIDTH;
	dirtyRect.size.width+=2*MAJORWIDTH;
	dirtyRect.size.height+=2*MAJORWIDTH;
	
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
	
	if (dragState == dsSelect)
	{
		[[NSColor colorWithCalibratedRed:0.5 green:0.5 blue:0.5 alpha:0.5] setFill];
		[NSBezierPath fillRect:dragRect];
	}
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

-(void)beginDragging:(NSEvent *)theEvent
{
	dragState = dsMove;
	dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    initialPoint = dragPoint;
}

-(void)endDragging:(NSEvent*)theEvent
{
    dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSPoint dif,invDif;
    dif.x = dragPoint.x-initialPoint.x;
    dif.y = dragPoint.y-initialPoint.y;
    invDif.x = -dif.x;
    invDif.y = -dif.y;
    NSArray * sv = [self subviews];
    NSUndoManager * um = nil;
    for (ObjectView * view in sv)
    {
        if (![view isKindOfClass:[ObjectView class]]) continue;
        PHObject * obj = view.object;
        if (obj.readOnly) continue;
        if (!obj.selected) continue;
        if (!um)
        {
            um = [obj.controller undoManager];
            [um beginUndoGrouping];
        }
        [obj move:invDif];
        [obj undoableMove:dif];
    }
    [um endUndoGrouping];
}

- (BOOL)resignFirstResponder
{
	dragState = dsNone;
	return [super resignFirstResponder];
}

- (void)mouseDown:(NSEvent *)theEvent
{
	NSLog(@"%x",[theEvent modifierFlags]);
	if ([theEvent modifierFlags] & NSAlternateKeyMask)
		[self beginDragging:theEvent];
	else 
	{
		dragState = dsSelect;
		NSArray * sv = [self subviews];
		for (ObjectView * view in sv)
		{
			if (![view isKindOfClass:[ObjectView class]]) continue;
			view.object.selected = NO;
		}
        dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	}
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	if (dragState == dsMove)
	{
		NSPoint pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
		NSPoint dif = pnt;
		dif.x-= dragPoint.x;
		dif.y-= dragPoint.y;
		dragPoint = pnt;
		NSArray * sv = [self subviews];
		for (ObjectView * view in sv)
		{
			if (![view isKindOfClass:[ObjectView class]]) continue;
			PHObject * obj = view.object;
			if (obj.readOnly) continue;
			if (!obj.selected) continue;
			[obj move:dif];
		}
	}
	if (dragState == dsSelect)
	{
		NSPoint pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
		dragRect.size.width = pnt.x-dragPoint.x;
		dragRect.size.height = pnt.y-dragPoint.y;
		dragRect.origin = dragPoint;
		if (dragRect.size.width<0)
		{
			dragRect.origin.x += dragRect.size.width;
			dragRect.size.width = -dragRect.size.width;
		}
		if (dragRect.size.height<0)
		{
			dragRect.origin.y += dragRect.size.height;
			dragRect.size.height = -dragRect.size.height;
		}
		NSArray * sv = [self subviews];
		for (ObjectView * view in sv)
		{
			if (![view isKindOfClass:[ObjectView class]]) continue;
			view.object.selected = NSPointInRect(view.frame.origin, dragRect);
		}
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if (dragState==dsMove)
        [self endDragging:theEvent];
	dragState = dsNone;
	[self setNeedsDisplay:YES];
}


@end
