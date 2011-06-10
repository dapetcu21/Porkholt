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

#import <float.h>

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
    [self unbind:@"hidden"];
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
    double rota = 0;
    if (shape==kSOSRect)
    {
        if (posX&&posY&&posW&&posH)
        {
            frame.origin.x = posX.doubleValue;
            frame.origin.y = posY.doubleValue;
            frame.size.width = posW.doubleValue;
            frame.size.height = posH.doubleValue;
            rota = rot.doubleValue;
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
    [self setFrameCenterRotation:0];
    [self setFrame:frame];
    [self setFrameCenterRotation:-rota];
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
    reb(rot,property,@"rotation");
}

inline BOOL lineIntersectsRect(NSPoint & p1, NSPoint & p2, NSRect & r)
{
    double x;
    double y;
    double maxX = p1.x;
    double minX = p2.x;
    double maxY = p1.y;
    double minY = p2.y;
    if (maxY<minX)
    {
        maxX = p2.x;
        minX = p1.x;
    }
    if (maxY<minY)
    {
        maxY = p2.y;
        minY = p1.y;
    }
    y = r.origin.y;
    x = p1.x-(p1.x-p2.x)*(p1.y-y)/(p1.y-p2.y);
    if (x>=r.origin.x && x<=r.origin.x+r.size.width && x<=maxX && x>=maxY && y<=maxY && y>=minY) return YES;
    y = r.origin.y+r.size.height;
    x = p1.x-(p1.x-p2.x)*(p1.y-y)/(p1.y-p2.y);
    if (x>=r.origin.x && x<=r.origin.x+r.size.width && x<=maxX && x>=maxY && y<=maxY && y>=minY) return YES;
    x = r.origin.x;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.origin.y && y<=r.origin.y+r.size.height && x<=maxX && x>=maxY && y<=maxY && y>=minY) return YES;
    x = r.origin.x+r.size.width;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.origin.y && y<=r.origin.y+r.size.height && x<=maxX && x>=maxY && y<=maxY && y>=minY) return YES;
    return NO;
}

+(BOOL)rect:(NSRect)r1 inView:(NSView*)v1 intersectsRect:(NSRect)r2 inView:(NSView*)v2
{
    NSPoint p1[4],p2[4];
    double minX = DBL_MAX, minY = DBL_MAX, maxX = -DBL_MAX, maxY = -DBL_MAX;
    for (int i=0; i<4; i++)
    {
        p1[i].x = r1.origin.x+((i&1)?r1.size.width:0);
        p1[i].y = r1.origin.y+((i&2)?r1.size.height:0);
        p1[i] = [v2 convertPoint:p1[i] fromView:v1];
        if (p1[i].x<minX) minX = p1[i].x;
        if (p1[i].x>maxX) maxX = p1[i].x;
        if (p1[i].y<minY) minY = p1[i].y;
        if (p1[i].y>maxY) maxY = p1[i].y;
        if (NSPointInRect(p1[i],r2)) return YES;
    }
    if ((minX>r2.origin.x+r2.size.width)||
        (maxX<r2.origin.x)||
        (minY>r2.origin.y+r2.size.height)||
        (maxY<r2.origin.y))
        return NO;
    for (int i=0; i<4; i++)
    {
        p2[i].x = r2.origin.x+((i&1)?r2.size.width:0);
        p2[i].y = r2.origin.y+((i&2)?r2.size.height:0);
        p2[i] = [v1 convertPoint:p2[i] fromView:v2];
        if (NSPointInRect(p2[i],r1)) return YES;
    }
    if (lineIntersectsRect(p1[0],p1[1],r2)) return YES;
    if (lineIntersectsRect(p1[1],p1[3],r2)) return YES;
    if (lineIntersectsRect(p1[0],p1[2],r2)) return YES;
    if (lineIntersectsRect(p1[2],p1[3],r2)) return YES;
    return NO;
}

-(BOOL)intersectsRect:(NSRect)rect fromView:(NSView*)v
{
    return [SubObjectView rect:rect inView:v intersectsRect:self.bounds inView:self];
}
-(BOOL)intersectsPoint:(NSPoint)pnt
{
    return NSPointInRect(pnt,self.bounds);
}

-(void)mouseDown:(NSEvent *)theEvent
{
    if (!objectView.object.controller.worldController.objectMode)
    {
        [super mouseDown:theEvent];
        return;
    }
    if (![self intersectsPoint:[self convertPoint:[theEvent locationInWindow] fromView:nil]])
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

-(double)rotation
{
    if (rot || rot.type != kPHObjectPropertyNumber)
        return rot.doubleValue;
    return 0;
}
-(void)setRotation:(double)val
{
    if (!rot || rot.type != kPHObjectPropertyNumber) return;
    while (val<0)
        val+=360;
    while (val>=360)
        val-=360;
    rot.doubleValue = val;
    [self setFrameCenterRotation:0];
    [self setFrameCenterRotation:-val];
    [objectView adaptForView:self];
    [property.parentObject modified];
}

-(void)undoable:(NSUndoManager*)man setRotation:(double)val
{
    [[man prepareWithInvocationTarget:self] setRotation:self.rotation];
    [self setRotation:val];
}

@end
