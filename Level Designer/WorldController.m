//
//  WorldController.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "WorldController.h"
#import "ObjectController.h"
#import "ObjectView.h"
#import "ViewScaling.h"
#import "WorldView.h"
#import "PHObject.h"

@implementation WorldController
@synthesize showImages;
@synthesize showFixtures;
@synthesize showMarkers;
@synthesize showJoints;
@synthesize objectMode;
@synthesize currentObject;
@synthesize objectController;
@synthesize document;

-(BOOL)objectMode
{
    return objectMode;
}

-(void)setObjectMode:(BOOL)val
{
    if (val)
    {
        if (![objectController selectedObject]||[objectController selectedObject].readOnly)
        {
            NSBeep();
            return;
        }
        objectMode = YES;
        currentObject = [objectController selectedObject];
    }
    else
    {
        objectMode = NO;
        currentObject = NO;
    }
    [worldView cancelAllDrags];
}

+(void)initialize
{
	[WorldController exposeBinding:@"objects"];
	[WorldController exposeBinding:@"selection"];
}

-(id)init
{
	if (self=[super init])
	{
		scalingFactor = 100.0f;
        showFixtures = YES;
        showImages = YES;
        showJoints = YES;
        showMarkers = YES;
	}
	return self;
}

-(void) dealloc
{
	[objects release];
	[selection release];
	[super dealloc];
}
-(void)awakeFromNib
{
	objects = [NSArray new];
	selection = [NSArray new];
	worldView.controller = self;
	NSRect frm = [worldView frame];
	worldView.bounds = NSMakeRect(-frm.size.width/2/scalingFactor, -frm.size.height/2/scalingFactor, frm.size.width/scalingFactor, frm.size.height/scalingFactor);
	worldView.scale = NSMakeSize(scalingFactor, scalingFactor);
	controller = [objectController arrayController];
	[self bind:@"objects" toObject:controller withKeyPath:@"content" options:nil];
	[self bind:@"selection" toObject:controller withKeyPath:@"selectedObjects" options:nil];	
}

-(NSArray*)selection
{
	return selection;
}

-(void)setSelection:(NSArray*)obj
{
	[selection release];
	selection = [[NSArray alloc] initWithArray:obj];
	for (PHObject * obj in objects)
		[obj updateSelected:NO];
	for (PHObject * obj in selection)
		[obj updateSelected:YES];
    PHObject * sel = [objectController selectedObject];
	if (objectMode)
    {
        if ((!sel||sel.readOnly))
            [self setObjectMode:NO];
        else
        {
            [worldView cancelAllDrags];
            currentObject = sel;    
        }
    }
    if (sel)
        [objectController expandStuff];
}


-(NSArray*)objects
{
	return objects;
}

-(void)setObjects:(NSArray*)obj
{
	[objects release];
	objects = [[NSArray alloc] initWithArray:obj];
	[self updateSubviews];
}

-(void)updateSubviews
{
	NSArray * views = [[[worldView subviews] copy] autorelease];
	for (NSView * view in views)
	{
		[view retain];
		if ([view isKindOfClass:[ObjectView class]])
			[view removeFromSuperview];
	}
	for (PHObject * obj in objects)
	{
		[obj rebuildView];
		ObjectView * view = obj.view;
		[worldView addSubview:view];
	}
	for (NSView * view in views)
		[view release];
}

-(double)scalingFactor
{
	return scalingFactor;
}

-(void)setScalingFactor:(double)sf
{
	NSRect bounds = worldView.bounds;
    NSPoint pnt;
    pnt.x = bounds.origin.x+bounds.size.width/2;
    pnt.y = bounds.origin.y+bounds.size.height/2;
	bounds.size.width/= sf/scalingFactor;
	bounds.size.height/= sf/scalingFactor;
    bounds.origin.x = pnt.x-bounds.size.width/2;
    bounds.origin.y = pnt.y-bounds.size.height/2;
    worldView.scale = NSMakeSize(sf, sf);
	worldView.bounds = bounds;
	scalingFactor = sf;
}

-(NSUndoManager*)undoManager
{
    return [document undoManager];
}

@end
