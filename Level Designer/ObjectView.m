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
    NSLog(@"%@ %@ %d %d",self.worldView,self.worldView.controller,self.worldView.controller.showMarkers,value);
    self.worldView.controller.showMarkers = value;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        centerView = [[SolidColorView alloc] initWithFrame:NSMakeRect(-0.04,-0.04,0.08,0.08)];
        [centerView bind:@"hidden" toObject:self withKeyPath:@"object.controller.worldController.showMarkers" options:[NSDictionary dictionaryWithObject:NSNegateBooleanTransformerName forKey:NSValueTransformerNameBindingOption]];
        [self addSubview:centerView];
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
			[super mouseDown:theEvent];
		else
		{
			if (object.readOnly)
            {
				[super mouseDown:theEvent];
                return;
            }
			if (!object.selected)
            {
				[[object.controller arrayController] setSelectedObjects:[NSArray arrayWithObject:object]];
                [self setSelected:YES];
            }
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
    return [NSArray array];
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
    double x = 0.04;
    double y = 0.04;
    for (NSView * view in arr)
    {
        NSRect fr = [view frame];
        if (fr.origin.x<-x)
            x=-fr.origin.x;
        if (fr.origin.x+fr.size.width>x)
            x=fr.origin.x+fr.size.width;
        if (fr.origin.y<-y)
            y=-fr.origin.y;
        if (fr.origin.y+fr.size.height>y)
            y=fr.origin.y+fr.size.height;
    }
    NSRect fr = [self frame];
    fr.origin.x +=fr.size.width/2-x;
    fr.origin.y +=fr.size.height/2-y;
    fr.size.width = x*2;
    fr.size.height = y*2;
    [self setFrame:fr];
    fr.origin.x = -x;
    fr.origin.y = -y;
    [self setBounds:fr];
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
        if (![v isHidden] && ([sv isKindOfClass:[SubObjectView class]]?[(SubObjectView*)sv intersectsPoint:pnt]:NSPointInRect(pnt,v.frame)))
            return YES;
    return NO;
}

- (BOOL)intersectsRectInObjectCoords:(NSRect)rect
{
    NSArray * sv = [self subviews];
    for (NSView * v in sv)
        if (![v isHidden] && ([sv isKindOfClass:[SubObjectView class]]?[(SubObjectView*)sv intersectsRect:rect]:NSIntersectsRect(v.frame,rect)))
            return YES;
    return NO;
}

- (BOOL)intersectsRect:(NSRect)rect
{
    NSRect frame,bounds;
    frame = [self frame];
    if (!NSIntersectsRect(frame,rect))
        return NO;
    bounds = [self bounds];
    rect.origin.x+=bounds.origin.x-frame.origin.x;
    rect.origin.y+=bounds.origin.y-frame.origin.y;
    return [self intersectsRectInObjectCoords:rect];
}

- (void)adaptForView:(NSView*)v
{
    NSRect frame = [self frame];
    double x = frame.size.width/2;
    double y = frame.size.height/2;
    NSRect fr = [v frame];
    if (fr.origin.x<-x)
        x=-fr.origin.x;
    if (fr.origin.x+fr.size.width>x)
        x=fr.origin.x+fr.size.width;
    if (fr.origin.y<-y)
        y=-fr.origin.y;
    if (fr.origin.y+fr.size.height>y)
        y=fr.origin.y+fr.size.height;
    fr = frame;
    fr.origin.x +=fr.size.width/2-x;
    fr.origin.y +=fr.size.height/2-y;
    fr.size.width = x*2;
    fr.size.height = y*2;
    [self setFrame:fr];
    fr.origin.x = -x;
    fr.origin.y = -y;
    [self setBounds:fr];
}

@end
