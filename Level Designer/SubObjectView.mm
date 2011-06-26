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

-(BOOL)showMarkers
{
    return showMarkers;
}

-(void)setShowMarkers:(BOOL)val
{
    showMarkers = val;
    [self setNeedsDisplay:YES];
}

#define HANDLEWIDTH 0.01f
#define RESIZEHANDLE 0.02f

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    if (failed)
    {
        [[NSImage imageNamed:NSImageNameCaution] drawInRect:bounds fromRect:NSZeroRect operation:NSCompositeSourceAtop fraction:1.0f];
        return;
    }
    if (type==kSOTImage)
        [img drawInRect:bounds fromRect:NSZeroRect operation:NSCompositeSourceAtop fraction:1.0f];
    else
    {
        NSBezierPath * bpath;
        NSRect rect = bounds;
        rect.size.width-=HANDLEWIDTH;
        rect.size.height-=HANDLEWIDTH;
        rect.origin.x+=HANDLEWIDTH/2;
        rect.origin.y+=HANDLEWIDTH/2;
        if (shape==kSOSRect)
            bpath = [NSBezierPath bezierPathWithRect:rect];
        if (shape==kSOSCircle)
            bpath = [NSBezierPath bezierPathWithOvalInRect:rect];
        [bpath setLineWidth:HANDLEWIDTH];
        [[NSColor colorWithCalibratedRed:0.06 green:0.3 blue:0.66 alpha:0.7] setFill];
        [[NSColor greenColor] setStroke];
        [bpath fill];
        [bpath stroke];
    }
    if (shape==kSOSRect && trackingArea && showMarkers)
    {
        if (property.selected)
            [[NSColor colorWithCalibratedRed:0.5f green:0.5f blue:1.0f alpha: 1.0f] setFill];
        else
            [[NSColor redColor] setFill];
        for (int i=0; i<4; i++)
        {
            NSBezierPath * bpath = [NSBezierPath bezierPathWithRect:
                     NSMakeRect(bounds.origin.x+(i&1?(bounds.size.width-RESIZEHANDLE*2):0),
                                bounds.origin.y+(i&2?(bounds.size.height-RESIZEHANDLE*2):0),RESIZEHANDLE*2,RESIZEHANDLE*2)];
            [bpath fill];
        }
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
    BOOL fail = NO;
    NSRect frame = NSMakeRect(-0.2,-0.2,0.4,0.4);
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
            if (failed || ![imagePath isEqualToString:nimg])
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
    if (x>=r.origin.x && x<=r.origin.x+r.size.width && x<=maxX && x>=maxX && y<=maxY && y>=minY) return YES;
    y = r.origin.y+r.size.height;
    x = p1.x-(p1.x-p2.x)*(p1.y-y)/(p1.y-p2.y);
    if (x>=r.origin.x && x<=r.origin.x+r.size.width && x<=maxX && x>=maxX && y<=maxY && y>=minY) return YES;
    x = r.origin.x;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.origin.y && y<=r.origin.y+r.size.height && x<=maxX && x>=maxX && y<=maxY && y>=minY) return YES;
    x = r.origin.x+r.size.width;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.origin.y && y<=r.origin.y+r.size.height && x<=maxX && x>=maxX && y<=maxY && y>=minY) return YES;
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
    PHObject * obj = objectView.object;
    WorldController * cntr = obj.controller.worldController;
    if (!cntr.objectMode || cntr.currentObject != obj)
    {
        [super mouseDown:theEvent];
        return;
    }
    NSPoint pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    if (![self intersectsPoint:pnt])
    {
        [super mouseDown:theEvent];
        return;
    }
    int tag;
    if (tag=[self resizeAreaHit:pnt])
    {
        [self beginDrag:theEvent withResizeArea:tag];
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
			if (obj.readOnly)
            {
				[super mouseDown:theEvent];
                return;
            }
			if (!property.selected)
            {
                ObjectController * controller = obj.controller;
                [controller selectObject:obj];
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

-(void)mouseDragged:(NSEvent*)theEvent
{
    if (dragTag)
        [self moveDrag:theEvent];
    else
        [super mouseDragged:theEvent];
}

-(void)mouseUp:(NSEvent*)theEvent
{
    if (dragTag)
        [self endDrag:theEvent];
    else
        [super mouseUp:theEvent];
}

-(void)move:(NSPoint)delta
{
    if (posX&&posY)
    {
        posX.doubleValue = posX.doubleValue+delta.x;
        posY.doubleValue = posY.doubleValue+delta.y;
        NSPoint origin = [self frame].origin;
        origin.x+=delta.x;
        origin.y+=delta.y;
        [self setFrameOrigin:origin];
        [objectView adaptForView:self];
        objectView.ignoreModified = YES;
        [property.parentObject modified];
        objectView.ignoreModified = NO;
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
    }
}

-(double)rotation
{
    if (!rot || rot.type != kPHObjectPropertyNumber)
        return 0;
    return rot.doubleValue;
}
-(void)setRotation:(double)val
{
    if (!rot || rot.type != kPHObjectPropertyNumber) return;
    while (val<0)
        val+=360;
    while (val>=360)
        val-=360;
    rot.doubleValue = val;
    [self setFrameCenterRotation:-val];
    [objectView adaptForView:self];
    objectView.ignoreModified = YES;
    [property.parentObject modified];
    objectView.ignoreModified = NO;
}

-(double)radius
{
    return rad.doubleValue;
}

-(NSSize)size
{
    return NSMakeSize(posW.doubleValue,posH.doubleValue);
}

-(NSPoint)position
{
    return NSMakePoint(posX.doubleValue,posY.doubleValue);
}

-(BOOL)supportsRotation
{
    return (rot && rot.type == kPHObjectPropertyNumber);
}

-(void)undoable:(NSUndoManager*)man setRotation:(double)val
{
    [[man prepareWithInvocationTarget:self] setRotation:self.rotation];
    [self setRotation:val];
}

-(void)setEditMode:(BOOL)val
{
     if (val)
     {
         trackingArea = [[NSTrackingArea alloc] initWithRect:NSZeroRect 
                                                     options:NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect
                                                       owner:self
                                                    userInfo:nil];
         [self addTrackingArea:trackingArea];
     } else {
         [self removeTrackingArea:trackingArea];
         [trackingArea release];
         trackingArea = nil;
     }
    [self setNeedsDisplay:YES];
}

#pragma mark -
#pragma mark Resizing

-(double)squareDistanceForPoint:(NSPoint)pnt
{
    NSRect bounds = [self bounds];
    pnt.x-=bounds.origin.x + bounds.size.width/2;
    pnt.y-=bounds.origin.y + bounds.size.height/2;
    double sqdist = pnt.x*pnt.x+pnt.y*pnt.y;
    return sqdist;
}

-(void)updateCursorWithEvent:(NSEvent*)theEvent
{
    if (dragTag)
        [[NSCursor closedHandCursor] set];
    else
        if ([self resizeAreaHit:[self convertPoint:[theEvent locationInWindow] fromView:nil]])
        {
            [[NSCursor openHandCursor] set];
        } else 
            [[NSCursor arrowCursor] set];
}

-(void)beginDrag:(NSEvent*)theEvent withResizeArea:(int)tag
{
    [self saveState:initialState];
    dragTag = tag;
    dragPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    if (shape==kSOSCircle)
        dragRadius = sqrt([self squareDistanceForPoint:dragPoint]);
    [self updateCursorWithEvent:theEvent];
}

-(void)moveDrag:(NSEvent*)theEvent
{
    NSPoint pnt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    if (shape==kSOSCircle && dragTag==1)
    {
        double radius = sqrt([self squareDistanceForPoint:pnt]);
        rad.doubleValue = rad.doubleValue + radius - dragRadius;
        dragRadius = radius;
        [self reloadData];
        [objectView adaptForView:self];
        dragPoint = pnt;
    }
    if (shape==kSOSRect && dragTag!=0)
    {
        NSPoint p = pnt;
        p.x-=dragPoint.x;
        p.y-=dragPoint.y;
        NSRect extend = NSZeroRect;
        if (dragTag&1)
        {
            extend.origin.x+=p.x;
            extend.size.width-=p.x;
        }
        if (dragTag&4)
        {
            extend.origin.y+=p.y;
            extend.size.height-=p.y;
        }
        if (dragTag&2)
            extend.size.width+=p.x;
        if (dragTag&8)
            extend.size.height+=p.y;
        if (self.rotation!=0)
        {
            extend.origin.x = -extend.size.width;
            extend.origin.y = -extend.size.height;
            extend.size.width*=2;
            extend.size.height*=2;
        }
        
        positionState st;
        [self saveState:st];
        st.x+=extend.origin.x;
        st.y+=extend.origin.y;
        st.w+=extend.size.width;
        st.h+=extend.size.height;
        if ((st.w>0)&&(st.h>0))
        {
            [self loadState:st];
            dragPoint = pnt;
            dragPoint.x-=extend.origin.x;
            dragPoint.y-=extend.origin.y;
        }
    }
    [self updateCursorWithEvent:theEvent];
}

-(void)endDrag:(NSEvent*)theEvent
{
    [self undoable:property.parentObject.controller.undoManager intoState:initialState];
    dragTag = 0;
    [self updateCursorWithEvent:theEvent];
}

-(int)resizeAreaHit:(NSPoint)pnt
{
    if (failed)
        return 0;
    if (shape == kSOSRect)
    {
        NSRect bounds = [self bounds];
        if (!NSPointInRect(pnt,bounds))
            return 0;
        if ((pnt.x<=RESIZEHANDLE*2)&&(pnt.y<=RESIZEHANDLE*2)) return 1|4;
        if ((pnt.x>=bounds.size.width-RESIZEHANDLE*2)&&(pnt.y<=RESIZEHANDLE*2)) return 2|4;
        if ((pnt.x<=RESIZEHANDLE*2)&&(pnt.y>=bounds.size.height-RESIZEHANDLE*2)) return 1|8;
        if ((pnt.x>=bounds.size.width-RESIZEHANDLE*2)&&(pnt.y>=bounds.size.height-RESIZEHANDLE*2)) return 2|8;
        if (pnt.x<=RESIZEHANDLE) return 1;
        if (pnt.x>=bounds.size.width-RESIZEHANDLE) return 2;
        if (pnt.y<=RESIZEHANDLE) return 4;
        if (pnt.y>=bounds.size.height-RESIZEHANDLE) return 8;
    }
    if (shape == kSOSCircle)
    {
        double sqdist = [self squareDistanceForPoint:pnt];
        double r = rad.doubleValue;
        if (sqdist<=(r+RESIZEHANDLE/2)*(r+RESIZEHANDLE/2) && sqdist>=(r-RESIZEHANDLE)*(r-RESIZEHANDLE))
            return 1;
    }
    return 0;
}

- (void)mouseEntered:(NSEvent *)theEvent
{
    [self updateCursorWithEvent:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
    [self updateCursorWithEvent:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    [self updateCursorWithEvent:theEvent];
}

-(void)saveState:(positionState&)st
{
    st.x = posX.doubleValue;
    st.y = posY.doubleValue;
    st.w = posW.doubleValue;
    st.h = posH.doubleValue;
    st.r = rad.doubleValue;
    st.rot = rot.doubleValue;
}


-(void)loadState:(const positionState&)st
{
    [self weakRebuildCachedProperties];
    if (posX)
        posX.doubleValue = st.x;
    if (posY)
        posY.doubleValue = st.y;
    if (posW)
        posW.doubleValue = st.w;
    if (posH)
        posH.doubleValue = st.h;
    if (rad)
        rad.doubleValue = st.r;
    if (rot)
        rot.doubleValue = st.rot;
    [self reloadData];
    [objectView adaptForView:self];
}

-(void)undoable:(NSUndoManager*)man loadState:(positionState)st
{
    positionState current;
    [self saveState:current];
    [[man prepareWithInvocationTarget:self] undoable:man loadState:current];
    [self loadState:st];
}

-(void)undoable:(NSUndoManager*)man intoState:(positionState) state
{
    [[man prepareWithInvocationTarget:self] undoable:man loadState:state];
}

-(BOOL)resetAspectRatioUndoable:(NSUndoManager*)um
{
    if (type != kSOTImage) return NO;
    [self weakRebuildCachedProperties];
    if (!img) return NO;
    NSSize sz = [img size];
    if (!sz.width || !sz.height) return NO;
    if (!posX || !posY || !posW || !posH) return NO;
    positionState crr;
    [self saveState:crr];
    double nh;
    posH.doubleValue = nh = sz.height/sz.width*posW.doubleValue;
    if (crr.rot)
        posY.doubleValue = crr.y-(nh-crr.h)/2;
    [self reloadData];
    [objectView adaptForView:self];
    [self undoable:um intoState:crr];
    return YES;
}

-(BOOL)undoable:(NSUndoManager*)man matchWith:(SubObjectView*)source
{
    positionState crr;
    [self saveState:crr];
    posX.doubleValue = source.position.x;
    posY.doubleValue = source.position.y;
    if (shape==kSOSRect && source.shape==kSOSRect)
    {
        posW.doubleValue = source.size.width;
        posH.doubleValue = source.size.height;
        rot.doubleValue = source.rotation;
    }   
    if (shape==kSOSCircle && source.shape==kSOSCircle)
    {
        rad.doubleValue = source.radius;
    }
    [self undoable:man intoState:crr];
    [self reloadData];
    [objectView adaptForView:self];
    return YES;
}

@end
