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
#import "WorldController.h"
#import "ObjectController.h"
#import "SubObjectView.h"
#import "PHObjectProperty.h"

@interface NSEvent()
-(double)deviceDeltaX;
-(double)deviceDeltaY;
-(double)deviceDeltaZ;
@end

@implementation WorldView
@synthesize controller;
@synthesize sender;

enum dragStates
{
	dsNone = 0,
	dsSelect,
	dsMove,
	NUMDRAGSTATES
};

enum selectModifier
{
    smNone = 0,
    smAdd,
    smSubstract
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

-(void)cancelInertialScrolling
{
    if (inscrolltimer)
    {
        [inscrolltimer invalidate];
        inscrolltimer = nil;
    }
    ins = 0;
}

-(void)beginDragging:(NSEvent *)theEvent
{
    [self cancelInertialScrolling];
	dragState = dsMove;
	dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    initialPoint = dragPoint;
}

-(void)endDragging:(NSEvent*)theEvent
{
    if (theEvent)
        dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSPoint dif,invDif;
    dif.x = dragPoint.x-initialPoint.x;
    dif.y = dragPoint.y-initialPoint.y;
    invDif.x = -dif.x;
    invDif.y = -dif.y;
    if (!dif.x && !dif.y) return;
    if (controller.objectMode)
    {
        ObjectView * view = controller.currentObject.view;
        NSArray * sv = [view subviews];
        NSUndoManager * um = nil;
        for (SubObjectView * view in sv)
        {
            if (![view isKindOfClass:[SubObjectView class]]) continue;
            PHObjectProperty * prop = view.property;
            if (!prop.selected) continue;
            if (!um)
            {
                um = [prop.parentObject.controller undoManager];
                [um beginUndoGrouping];
            }
            [view move:invDif];
            [view undoable:um move:dif];
        }
        [um endUndoGrouping];
    } else {
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
}

- (BOOL)resignFirstResponder
{
	dragState = dsNone;
	return [super resignFirstResponder];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    [self cancelInertialScrolling];
    if (dragState != dsNone) return;
    if ([theEvent buttonNumber] != 2) return;
    opnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    vel.x = vel.y = 0;
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
    if (dragState != dsNone) return;
    if ([theEvent buttonNumber] != 2) return;
    NSPoint pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSPoint loc = self.position;
    vel.x = -theEvent.deltaX/controller.scalingFactor;
    vel.y = theEvent.deltaY/controller.scalingFactor;
    loc.x = (vel.x + loc.x);
    loc.y = (vel.y + loc.y);
    self.position = loc;
    opnt = pnt;
}

double min(double a, double b) { return a<b?a:b; }

-(void)inertialScroll
{
    if (dragState != dsNone) 
    {
        [inscrolltimer invalidate];
        inscrolltimer = nil;
    }
    NSPoint loc = self.position;
    loc.x = (vel.x*(ins/(double)insn) + loc.x);
    loc.y = (vel.y*(ins/(double)insn) + loc.y);
    self.position = loc;
    ins--;
    if (ins<=0)
    {
        [inscrolltimer invalidate];
        inscrolltimer = nil;
        ins = 0;
    }
}

#define DECAY_RATE 0.04

- (void)otherMouseUp:(NSEvent *)theEvent
{
    if (dragState != dsNone) return;
    if ([theEvent buttonNumber] != 2) return;
    insn = sqrtf(vel.x*vel.x+vel.y*vel.y)/DECAY_RATE;
    if (insn>30)
        insn=30;
    if (insn)
    {
        inscrolltimer = [[NSTimer scheduledTimerWithTimeInterval:1/30.0f target:self selector:@selector(inertialScroll) userInfo:nil repeats:YES] retain];
        ins = insn;
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    [self cancelInertialScrolling];
    [[self window] makeFirstResponder:self];
	if ([theEvent modifierFlags] & NSAlternateKeyMask)
    {
		[self beginDragging:theEvent];
        return;
    }
    dragState = dsSelect;
    selectModifier = smNone;
    if ([theEvent modifierFlags] & NSCommandKeyMask)
    {
        selectModifier = smAdd;
        if ([theEvent modifierFlags] & NSShiftKeyMask)
            selectModifier = smSubstract;
    }
    
    if (selectModifier==smNone)
    {
        if (controller.objectMode)
        {
            [controller.objectController clearPropertySelection];
        } else {
            NSArray * sv = [self subviews];
            for (ObjectView * view in sv)
            {
                if (![view isKindOfClass:[ObjectView class]]) continue;
                view.object.selected = NO;
            }
        }
    } else {
        if (controller.objectMode)
        {
            ObjectView * view = controller.currentObject.view;
            NSArray * sv = [view subviews];
            for (SubObjectView * view in sv)
            {
                if (![view isKindOfClass:[SubObjectView class]]) continue;
                PHObjectProperty * property = view.property;
                property.initialSelected = property.selected;
            }
            if (sender && [sender isKindOfClass:[SubObjectView class]])
            {
                if (selectModifier == smAdd)
                    ((SubObjectView*)sender).property.selected = YES;
                if (selectModifier == smSubstract)
                    ((SubObjectView*)sender).property.selected = NO;
            }
        } else {
            NSArray * sv = [self subviews];
            for (ObjectView * view in sv)
            {
                if (![view isKindOfClass:[ObjectView class]]) continue;
                view.object.initialSelected = view.object.selected;
            }
            if (sender && [sender isKindOfClass:[ObjectView class]])
            {
                if (selectModifier == smAdd)
                    ((ObjectView*)sender).object.selected = YES;
                if (selectModifier == smSubstract)
                    ((ObjectView*)sender).object.selected = NO;
            }
        }
    }
    dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
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
        if (controller.objectMode)
        {
            ObjectView * view = controller.currentObject.view;
            NSArray * sv = [view subviews];
            for (SubObjectView * view in sv)
            {
                if (![view isKindOfClass:[SubObjectView class]]) continue;
                PHObjectProperty * property = view.property;
                if (!property.selected) continue;
                [view move:dif];
            }
        } else {
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
        if (controller.objectMode)
        {
            ObjectView * view = controller.currentObject.view;
            NSRect rect = dragRect;
            NSRect frame = [view frame];
            rect.origin.x -= frame.origin.x+frame.size.width/2;
            rect.origin.y -= frame.origin.y+frame.size.height/2;
            NSArray * sv = [view subviews];
            for (SubObjectView * view in sv)
            {
                if (![view isKindOfClass:[SubObjectView class]]) continue;
                if (selectModifier==smNone)
                    view.property.selected = [view intersectsRect:rect];
                if (selectModifier==smAdd)
                    view.property.selected = view.property.initialSelected || [view intersectsRect:rect];
                if (selectModifier==smSubstract)
                    view.property.selected = view.property.initialSelected && ![view intersectsRect:rect];
            }
        } else {
            NSArray * sv = [self subviews];
            for (ObjectView * view in sv)
            {
                if (![view isKindOfClass:[ObjectView class]]) continue;
                if (selectModifier==smNone)
                    view.object.selected = [view intersectsRect:dragRect];
                if (selectModifier==smAdd)
                    view.object.selected = view.object.initialSelected || [view intersectsRect:dragRect];
                if (selectModifier==smSubstract)
                    view.object.selected = view.object.initialSelected && ![view intersectsRect:dragRect];
            }
        }
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if (dragState==dsMove)
        [self endDragging:theEvent];
	dragState = dsNone;
    selectModifier = smNone;
    dragRect = NSZeroRect;
	[self setNeedsDisplay:YES];
}


-(void)cancelAllDrags
{
    [self cancelInertialScrolling];
    if (dragState!=dsNone)
        [self mouseUp:nil];
}

-(NSPoint)position
{
    NSRect bounds = self.bounds;
    NSPoint pnt;
    pnt.x = bounds.origin.x+bounds.size.width/2;
    pnt.y = bounds.origin.y+bounds.size.height/2;
    return pnt;
}

-(void)setPosition:(NSPoint)pos
{
    NSRect bounds = self.bounds;
    bounds.origin.x = pos.x-bounds.size.width/2;
    bounds.origin.y = pos.y-bounds.size.height/2;
    self.bounds = bounds;
}

-(void)zoomAtPoint:(NSPoint)loc byScale:(double)mg
{
    NSPoint pos = self.position;
    self.position = loc;
    double mag = controller.scalingFactor*(1+mg*0.75);
    double sf = controller.scalingFactor;
    controller.scalingFactor = mag;
    pos.x = loc.x + (pos.x - loc.x)/mag*sf;
    pos.y = loc.y + (pos.y - loc.y)/mag*sf;
    self.position = pos;
}

- (void)scrollWheel:(NSEvent*)theEvent
{
    double dx = theEvent.deltaX * 10;
    double dy = theEvent.deltaY * 10;
    if (CGEventGetIntegerValueField([theEvent CGEvent ],kCGScrollWheelEventIsContinuous))
    {
        dx = theEvent.deviceDeltaX;
        dy = theEvent.deviceDeltaY;
    }
    if ([theEvent modifierFlags] & NSCommandKeyMask)
        [self zoomAtPoint:[self convertPoint:[theEvent locationInWindow] fromView:nil] byScale:dy/1000];
    else
    {
        NSRect bounds = self.bounds;
        bounds.origin.x -= dx/(controller.scalingFactor);
        bounds.origin.y += dy/(controller.scalingFactor);
        self.bounds = bounds;
    }
}

-(void)magnifyWithEvent:(NSEvent *)event
{
    [self zoomAtPoint:[self convertPoint:[event locationInWindow] fromView:nil] byScale:[event magnification]];
}

-(IBAction)scrollToOrigin:(id)sender
{
	[self setPosition:NSMakePoint(0,0)];
}

@end
