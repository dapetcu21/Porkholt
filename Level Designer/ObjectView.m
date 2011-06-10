//
//  ObjectView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ObjectView.h"
#import "PHObject.h"
#import "PHObjectProperty.h"
#import "ObjectController.h"
#import "WorldView.h"
#import "SubObjectView.h"
#import "SolidColorView.h"
#import "WorldController.h"

@implementation ObjectView
@synthesize ignoreModified;

-(BOOL)showMarkers
{
    return self.worldView.controller.showMarkers;
}

-(void)setShowMarkers:(BOOL)value
{
    self.worldView.controller.showMarkers = value;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        centerView = [[SolidColorView alloc] initWithFrame:NSMakeRect(-0.04,-0.04,0.08,0.08)];
        [centerView bind:@"hidden" toObject:self withKeyPath:@"object.controller.worldController.showMarkers" options:[NSDictionary dictionaryWithObject:NSNegateBooleanTransformerName forKey:NSValueTransformerNameBindingOption]];
        [self addSubview:centerView];
        extendX = extendY = 0.04;
    }
    return self;
}

-(void)dealloc
{
	object.view = nil;
	[object release];
    [centerView release];
	[super dealloc];
}

-(BOOL)selected
{
    return selected;
}

-(void)setSelected:(BOOL)sel
{
    [self setNeedsDisplay:YES];
    selected = sel;
    centerView.color = sel?[NSColor colorWithCalibratedRed:0.5f green:0.5f blue:1.0f alpha: 1.0f]:(object.readOnly?[NSColor grayColor]:[NSColor whiteColor]);
}

-(void)setObject:(PHObject *)obj
{
	[obj retain];
	object.view = nil;
	[object release];
	object = obj;
	object.view = self;
    centerView.color = object.readOnly?[NSColor grayColor]:[NSColor whiteColor];
    [self rebuildSubviews];
}

-(PHObject*)object
{
	return object;
}

-(WorldView*)worldView
{
    WorldView * view = (WorldView*)[self superview];
	if ([view isKindOfClass:[WorldView class]])
        return view;
    return nil;
}

-(void)mouseDown:(NSEvent *)theEvent
{
    if (object.controller.worldController.objectMode)
    {
        [super mouseDown:theEvent];
        return;
    }
    if (![self pointIntersectsSelf:[self convertPoint:[theEvent locationInWindow] fromView:nil]])
    {
        [super mouseDown:theEvent];
        return;
    }
    WorldView * view = (WorldView*)[self superview];
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
			if (!object.selected)
            {
				[[object.controller arrayController] setSelectedObjects:[NSArray arrayWithObject:object]];
                [self setSelected:YES];
            }
            [[self window] makeFirstResponder:view];
            if (!object.readOnly)
                [view beginDragging:theEvent];
		}
	} else {
		[super mouseDown:theEvent];
	}

}

- (NSArray*)imageViews
{
    PHObjectProperty * prp = object.imagesProperty;
    NSMutableArray * arr = [NSMutableArray array];
    NSArray * props = prp.value;
    if ([props isKindOfClass:[NSArray class]])
    {
        for (PHObjectProperty * prop in props)
        {
            SubObjectView * view  = prop.userData;
            if (!view)
            {
                view = [[SubObjectView alloc] init];
                view.objectView = self;
                view.type = kSOTImage;
                view.property = prop;
                [view bind:@"hidden" toObject:self withKeyPath:@"object.controller.worldController.showImages" options:[NSDictionary dictionaryWithObject:NSNegateBooleanTransformerName forKey:NSValueTransformerNameBindingOption]];
            } else {
                [view reloadData];
            }
            [arr addObject:view];
        }
    }
    return arr;
}

- (NSArray*)fixtureViews
{
    PHObjectProperty * prp = object.fixturesProperty;
    NSMutableArray * arr = [NSMutableArray array];
    NSArray * props = prp.value;
    if ([props isKindOfClass:[NSArray class]])
    {
        for (PHObjectProperty * prop in props)
        {
            SubObjectView * view  = prop.userData;
            if (!view)
            {
                view = [[SubObjectView alloc] init];
                view.objectView = self;
                view.type = kSOTFixture;
                view.property = prop;
                [view bind:@"hidden" toObject:self withKeyPath:@"object.controller.worldController.showFixtures" options:[NSDictionary dictionaryWithObject:NSNegateBooleanTransformerName forKey:NSValueTransformerNameBindingOption]];
            } else {
                [view reloadData];
            }
            [arr addObject:view];
        }
    }
    return arr;
}

-(NSPoint)farthestForView:(NSView*)v
{
    NSPoint pnt;
    pnt.x = pnt.y = 0;
    NSRect fr = [v bounds];
    for (int i=0; i<4; i++)
    {
        NSPoint pts;
        pts.x = fr.origin.x+((i&1)?fr.size.width:0);
        pts.y = fr.origin.y+((i&2)?fr.size.height:0);
        pts = [self convertPoint:pts fromView:v];
        if (pts.x>pnt.x)
            pnt.x=pts.x;
        if (pts.x<-pnt.x)
            pnt.x=-pts.x;
        if (pts.y>pnt.y)
            pnt.y=pts.y;
        if (pts.y<-pnt.y)
            pnt.y=-pts.y;
    }
    if (fr.origin.x<-pnt.x)
        pnt.x=-fr.origin.x;
    if (fr.origin.x+fr.size.width>pnt.x)
        pnt.x=fr.origin.x+fr.size.width;
    if (fr.origin.y<-pnt.y)
        pnt.y=-fr.origin.y;
    if (fr.origin.y+fr.size.height>pnt.y)
        pnt.y=fr.origin.y+fr.size.height;
    return pnt;
}

- (void)updatePosition
{
    [self setFrameCenterRotation:0];
    NSRect frame;
    frame.size.width = extendX*2;
    frame.size.height = extendY*2;
    frame.origin = [object position];
    frame.origin.x -= extendX;
    frame.origin.y -= extendY;
    [self setFrame:frame];
    frame.origin.x = -extendX;
    frame.origin.y = -extendY;
    [self setBounds:frame];
    [self setFrameCenterRotation:-object.rotation];
}

- (void)rebuildSubviews
{
    NSMutableArray * arr = [NSMutableArray array];
    [arr addObjectsFromArray:[self imageViews]];
    [arr addObjectsFromArray:[self fixtureViews]];
    [arr addObject:centerView];
    NSArray * sv = [self subviews];
    int n = [arr count];
    BOOL doIt = TRUE;
    if (n==[sv count])
    {
        for (int i=0; i<n; i++)
            if ([arr objectAtIndex:i]!=[sv objectAtIndex:i])
                break;
        doIt = FALSE;
    }
    extendX = extendY = 0.04;
    for (NSView * view in arr)
    {
        NSPoint fp = [self farthestForView:view];
        if (fp.x>extendX) extendX = fp.x;
        if (fp.y>extendY) extendY = fp.y;
    }
    [self updatePosition];
    if (doIt)
    {
        [self setSubviews:arr];
    }
}

-(void)modified
{
    if (ignoreModified) return;
    [self rebuildSubviews];
}

- (BOOL)pointIntersectsSelf:(NSPoint)pnt
{
    NSArray * sv = [self subviews];
    for (NSView * v in sv)
        if (![v isHidden] && ([v isKindOfClass:[SubObjectView class]]?[(SubObjectView*)v intersectsPoint:[v convertPoint:pnt fromView:self]]:NSPointInRect([v convertPoint:pnt fromView:self],v.bounds)))
            return YES;
    return NO;
}

- (BOOL)intersectsRect:(NSRect)rect
{
    NSArray * sv = [self subviews];
    for (NSView * v in sv)
        if (![v isHidden] && ([v isKindOfClass:[SubObjectView class]]?[(SubObjectView*)v intersectsRect:rect fromView:self.superview]:[SubObjectView rect:rect inView:self.superview intersectsRect:v.bounds inView:v]))
            return YES;
    return NO;
}

- (void)adaptForView:(NSView*)v
{
    NSPoint fp = [self farthestForView:v];
    if (fp.x>extendX) extendX = fp.x;
    if (fp.y>extendY) extendY = fp.y;
    [self updatePosition];
}

@end
