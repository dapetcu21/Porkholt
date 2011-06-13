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
	dragPoint = [(controller.objectMode)?(NSView*)(controller.currentObject.view):(NSView*)self convertPoint:[theEvent locationInWindow] fromView:nil];
    initialPoint = dragPoint;
}

-(void)endDragging:(NSEvent*)theEvent
{
    if (theEvent)
        dragPoint = [(controller.objectMode)?(NSView*)(controller.currentObject.view):(NSView*)self convertPoint:[theEvent locationInWindow] fromView:nil];
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
        ObjectView * view;
        NSPoint pnt;
        if (controller.objectMode)
        {
            view = controller.currentObject.view;
            pnt = [view convertPoint:[theEvent locationInWindow] fromView:nil];
        } else {
            pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        }
		NSPoint dif = pnt;
		dif.x-= dragPoint.x;
		dif.y-= dragPoint.y;
		dragPoint = pnt;
        if (controller.objectMode)
        {
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
            NSArray * sv = [view subviews];
            for (SubObjectView * view in sv)
            {
                if (![view isKindOfClass:[SubObjectView class]]) continue;
                if (selectModifier==smNone)
                    view.property.selected = [view intersectsRect:dragRect fromView:self];
                if (selectModifier==smAdd)
                    view.property.selected = view.property.initialSelected || [view intersectsRect:dragRect fromView:self];
                if (selectModifier==smSubstract)
                    view.property.selected = view.property.initialSelected && ![view intersectsRect:dragRect fromView:self];
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
    [self endRotate];
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
    if (rotating) return;
    [self zoomAtPoint:[self convertPoint:[event locationInWindow] fromView:nil] byScale:[event magnification]];
    isZooming = YES;
}

-(IBAction)scrollToOrigin:(id)sender
{
	[self setPosition:NSMakePoint(0,0)];
}

- (void)endRotate
{
    if (rotated==0) return;
    rotating = NO;
    BOOL items = NO;
    NSUndoManager * undoMan = controller.undoManager;
    if (controller.objectMode)
    {
        ObjectView * view = controller.currentObject.view;
        NSArray * sv = [view subviews];
        for (SubObjectView * view in sv)
        {
            if (![view isKindOfClass:[SubObjectView class]]) continue;
            PHObjectProperty * property = view.property;
            if (!property.selected) continue;
            if (![view supportsRotation]) continue;
            if (!items)
            {
                items = YES;
                [undoMan beginUndoGrouping];
            }
            double rot = view.rotation;
            view.rotation = rot - rotated;
            [view undoable:undoMan setRotation:rot];
        }
    } else {
        NSArray * sv = [self subviews];
        for (ObjectView * view in sv)
        {
            if (![view isKindOfClass:[ObjectView class]]) continue;
            PHObject * obj = view.object;
            if (obj.readOnly) continue;
            if (!obj.selected) continue;
            if (!items)
            {
                items = YES;
                [undoMan beginUndoGrouping];
            }
            double rot = obj.rotation;
            obj.rotation = rot - rotated;
            [obj undoableSetRotation:rot];
        }
    }
    if (items)
        [undoMan endUndoGrouping];
    rotated = 0;
}

- (void)rotate:(double)amount
{
    
    rotated+=amount;
    if (controller.objectMode)
    {
        ObjectView * view = controller.currentObject.view;
        NSArray * sv = [view subviews];
        for (SubObjectView * view in sv)
        {
            if (![view isKindOfClass:[SubObjectView class]]) continue;
            PHObjectProperty * property = view.property;
            if (!property.selected) continue;
            view.rotation = view.rotation + amount;
        }
    } else {
        NSArray * sv = [self subviews];
        for (ObjectView * view in sv)
        {
            if (![view isKindOfClass:[ObjectView class]]) continue;
            PHObject * obj = view.object;
            if (obj.readOnly) continue;
            if (!obj.selected) continue;
            obj.rotation = obj.rotation + amount;
        }
    }
}

- (void)beginGestureWithEvent:(NSEvent *)event
{
}

- (void)endGestureWithEvent:(NSEvent *)event
{
    if (!isZooming)
        [self endRotate];
    isZooming = NO;
}

- (void)rotateWithEvent:(NSEvent *)event
{
    if (isZooming)
        return;
    rotating = YES;
    [self rotate:-[event rotation]];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    [self cancelInertialScrolling];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    [self rotate:[theEvent deltaY]];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    [self endRotate];
}

- (void)setObjectEdit:(BOOL)val
{
    ObjectView *  view = controller.currentObject.view;
    NSArray * sv = view.subviews;
    for (SubObjectView * so in sv)
        if ([so isKindOfClass:[SubObjectView class]])
            [so setEditMode:val];
}

#pragma mark -
#pragma mark Cut, copy, paste

-(IBAction)new:(id)_sender
{
    if (controller.objectMode)
        [controller.objectController newImage:_sender];
    else
        [controller.objectController new:_sender];
}

-(IBAction)delete:(id)_sender
{
    if (controller.objectMode)
        [controller.objectController deleteProp:_sender];
    else
        [controller.objectController delete:_sender];
}

-(IBAction)duplicate:(id)_sender
{
    if (controller.objectMode)
        [controller.objectController duplicateProp:_sender];
    else
        [controller.objectController duplicate:_sender];
}

-(IBAction)copy:(id)_sender
{
    if (controller.objectMode)
        [controller.objectController copyProp:_sender];
    else
        [controller.objectController copy:_sender];    
}

-(IBAction)paste:(id)_sender
{
    if (controller.objectMode)
        [controller.objectController pasteSubObject:_sender];
    else
        [controller.objectController paste:_sender];    
}

-(BOOL)validateMenuItem:(NSMenuItem*)_sender
{
    if (controller.objectMode)
        return [controller.objectController validateMenuItemProp:_sender];
    else
        return [controller.objectController validateMenuItem:_sender];
}

-(IBAction)selectAll:(id)_sender
{
    if (controller.objectMode)
    {
        [controller.objectController selectAllSubObjects:_sender];
    } else {
        [controller.objectController selectAll:_sender];
    }
}

#pragma mark -
#pragma mark Misc Features

-(IBAction)resetAspectRatio:(id)sender
{
    ObjectView *  view = controller.objectController.selectedObject.view;
    NSArray * sv = view.subviews;
    BOOL smth = NO;
    NSUndoManager * man = controller.undoManager;
    [man beginUndoGrouping];
    for (SubObjectView * so in sv)
        if ([so isKindOfClass:[SubObjectView class]])
            if (so.property.selected)
                smth = smth || [so resetAspectRatioUndoable:man];
    [man endUndoGrouping];
    if (!smth)
    {
        [man undo];
        NSBeep();
    }
}

@end
