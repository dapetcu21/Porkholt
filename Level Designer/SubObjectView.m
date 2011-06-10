//
//  SubObjectView.m
//  Level Designer
//
//  Created by Marius Petcu on 6/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "SubObjectView.h"
#import "PHObject.h"
#import "PHObjectProperty.h"
#import "ObjectView.h"
#import "MyDocument.h"
#import "ObjectController.h"
#import "WorldView.h"
#import "WorldController.h"

@implementation SubObjectView

@synthesize objectView;
@synthesize property;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        type = kSOTImage;
    }
    
    return self;
}

- (void)dealloc
{
    property.userData = nil;
    [property release];
    [img release];
    [imagePath release];
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
    if (failed)
    {
        [[NSImage imageNamed:NSImageNameCaution] drawInRect:[self bounds] fromRect:NSZeroRect operation:NSCompositeSourceAtop fraction:1.0f];
        return;
    }
    if (type==kSOTImage)
        [img drawInRect:[self bounds] fromRect:NSZeroRect operation:NSCompositeSourceAtop fraction:1.0f];
    else
    {
        NSBezierPath * bpath;
        if (shape==kSOSRect)
            bpath = [NSBezierPath bezierPathWithRect:[self bounds]];
        if (shape==kSOSCircle)
            bpath = [NSBezierPath bezierPathWithOvalInRect:[self bounds]];
        [bpath setLineWidth:0.01f];
        [[NSColor colorWithCalibratedRed:0.06 green:0.3 blue:0.66 alpha:0.7] setFill];
        [[NSColor greenColor] setStroke];
        [bpath fill];
        [bpath stroke];
    }
}

-(void)setProperty:(PHObjectProperty*)prop
{
    [property setUserData:nil];
    [prop setUserData:self];
    [prop retain];
    [property release];
    property = prop;
    [self rebuildCachedProperties];
    [self reloadData];
}

-(void)reloadData
{
    [self weakRebuildCachedProperties];
    NSRect frame = NSMakeRect(-0.2,-0.2,0.4,0.4);
    BOOL fail = NO;
    if (shape==kSOSRect)
    {
        if (posX&&posY&&posW&&posH)
        {
            frame.origin.x = posX.doubleValue;
            frame.origin.y = posY.doubleValue;
            frame.size.width = posW.doubleValue;
            frame.size.height = posH.doubleValue;
        } else {
            fail = YES;
        }
    }
    if (shape==kSOSCircle)
    {
        if (rad)
        {
            double rd = rad.doubleValue;
            if (posX&&posY)
            {
                frame.origin.x = posX.doubleValue-rd;
                frame.origin.y = posY.doubleValue-rd;
            } else
            {
                frame.origin.x = -rd;
                frame.origin.y = -rd;
            }
            frame.size.width = rd*2;
            frame.size.height = rd*2;
        } else {
            fail = YES;
        }
    }
    [self setFrame:frame];
    if (type == kSOTImage)
    {
        if (path)
        {
            NSString * nimg = path.stringValue;
            if (![imagePath isEqualToString:nimg] || failed)
            {
                [nimg retain];
                [imagePath release];
                imagePath = nimg;
                [img release];
                img = [[NSImage alloc] initWithContentsOfURL:[self.objectView.object.controller.document resourceURLNamed:imagePath]];
                if (!img)
                    fail = YES;
            }
        } else
        {
            fail = YES;
        }
    }
    failed = fail;
    [self setNeedsDisplay:YES];
}

- (int)type
{
    return type;
    
}

-(void)setType:(int)t
{
    type = t;
//    [self rebuildCachedProperties];
//    [self reloadData];
}
        
- (int)shape
{
    return shape;
}

-(void)setShape:(int)t
{
    shape = t;
    //[self rebuildCachedProperties];
    //[self reloadData];
}

-(void)rebuildCachedProperties
{
    pos = nil;
    posX = nil;
    posY = nil;
    posW = nil;
    posH = nil;
    path = nil;
    [self weakRebuildCachedProperties];
}

#define reb(x,y,z) do {if (!y) x = nil; else if (!x || ![x.key isEqualToString:z]) x = [y propertyForKey:z]; }while(0)

-(void)weakRebuildCachedProperties
{
    if (type==kSOTFixture)
    {
        reb(sh,property,@"shape");
        if (sh)
        {
            if ([sh.stringValue isEqualToString:@"box"] && shape!=kSOSRect)
            {
                shape=kSOSRect;
                pos = nil;
            }
            if ([sh.stringValue isEqualToString:@"circle"] && shape!=kSOSCircle)
            {
                shape=kSOSCircle;
                pos = nil;
            }
        }
    }
    if (type==kSOTImage)
        reb(pos,property,@"pos");
    else
    {
        if (shape==kSOSRect)
            reb(pos,property,@"box");
        if (shape==kSOSCircle)
            reb(pos,property,@"pos");
    }
    reb(posX,pos,@"x");
    reb(posY,pos,@"y");
    reb(posW,pos,@"width");
    reb(posH,pos,@"height");
    reb(path,property,@"filename");
    reb(rad,property,@"circleR");
}

-(BOOL)intersectsRect:(NSRect)rect
{
    return NSIntersectsRect(self.frame,rect);
}
-(BOOL)intersectsPoint:(NSPoint)pnt
{
    return NSPointInRect(pnt,self.frame);
}

-(void)mouseDown:(NSEvent *)theEvent
{
    if (!objectView.object.controller.worldController.objectMode)
    {
        [super mouseDown:theEvent];
        return;
    }
    if (![self intersectsPoint:[objectView convertPoint:[theEvent locationInWindow] fromView:nil]])
    {
        [super mouseDown:theEvent];
        return;
    }
    WorldView * view = (WorldView*)[[self superview] superview];
	if ([view isKindOfClass:[WorldView class]])
	{
		if ([theEvent modifierFlags] & (NSShiftKeyMask | NSAlternateKeyMask | NSCommandKeyMask) )
        {
            if (!view.sender)
                view.sender = self;
			[super mouseDown:theEvent];
            view.sender = nil;
        }
		else
		{
			if (objectView.object.readOnly)
            {
				[super mouseDown:theEvent];
                return;
            }
			if (!property.selected)
            {
                ObjectController * controller = objectView.object.controller;
                [controller selectObject:objectView.object];
                [controller clearPropertySelection];
                property.selected = YES;
            }
            [[self window] makeFirstResponder:view];
			[view beginDragging:theEvent];
		}
	} else {
		[super mouseDown:theEvent];
	}
    
}

-(void)move:(NSPoint)delta
{
    if (posX&&posY)
    {
        posX.doubleValue = posX.doubleValue+delta.x;
        posY.doubleValue = posY.doubleValue+delta.y;
        NSPoint origin = [self frame].origin;
        origin.x = posX.doubleValue;
        origin.y = posY.doubleValue;
        [self setFrameOrigin:origin];
        [objectView adaptForView:self];
        [property.parentObject modified];
    }
}

-(void)undoable:(NSUndoManager*)man move:(NSPoint)delta
{
    if (posX&&posY)
    {
        NSPoint invdelta;
        invdelta.x = -delta.x;
        invdelta.y = -delta.y;
        [[man prepareWithInvocationTarget:self] undoable:man move:invdelta];
        [self move:delta];
        [property.parentObject modified];
    }
}

@end
