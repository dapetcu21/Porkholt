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
        // Initialization code here.
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
    [img drawInRect:[self bounds] fromRect:NSZeroRect operation:NSCompositeSourceAtop fraction:1.0f];
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
    if (posX&&posY&&posW&&posH)
    {
        frame.origin.x = posX.doubleValue;
        frame.origin.y = posY.doubleValue;
        frame.size.width = posW.doubleValue;
        frame.size.height = posH.doubleValue;
    } else {
        fail = YES;
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
    [self reloadData];
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

#define reb(x,y,z) if (!y) x = nil; else if (!x || ![x.key isEqualToString:z]) x = [y propertyForKey:z]

-(void)weakRebuildCachedProperties
{
    reb(pos,property,@"pos");
    reb(posX,pos,@"x");
    reb(posY,pos,@"y");
    reb(posW,pos,@"width");
    reb(posH,pos,@"height");
    reb(path,property,@"filename");
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
			[super mouseDown:theEvent];
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
			[view beginDragging:theEvent];
		}
	} else {
		[super mouseDown:theEvent];
	}
    
}

@end
