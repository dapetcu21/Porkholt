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
#import "WorldClipView.h"
#import "WorldView.h"

@implementation WorldController

+(void)initialize
{
	[WorldController exposeBinding:@"objects"];
}

-(id)init
{
	if (self=[super init])
	{
		scalingFactor = 150.0f;
	}
	return self;
}

-(void) dealloc
{
	[super dealloc];
}
-(void)awakeFromNib
{
	worldView = [[WorldView alloc] init];
	NSRect frm = [scrollView contentView].bounds;
	worldView.frame = frm;
	worldView.bounds = NSMakeRect(-frm.size.width/2/scalingFactor, -frm.size.height/2/scalingFactor, frm.size.width/scalingFactor, frm.size.height/scalingFactor);
	worldView.scale = NSMakeSize(scalingFactor, scalingFactor);
	NSRect frame = [scrollView contentView].frame;
	WorldClipView * clipView = [[WorldClipView alloc] initWithFrame:frame];
	[clipView setDrawsBackground:NO];
	clipView.controller = self;
	[scrollView setContentView:clipView];
	[scrollView setDocumentView:worldView];
	controller = [objectController arrayController];
	[self bind:@"objects" toObject:controller withKeyPath:@"content" options:nil];
	
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
	NSLog(@"updateSubviews objects:%@",objects);
	NSArray * views = [[[worldView subviews] copy] autorelease];
	for (NSView * view in views)
	{
		if ([view isKindOfClass:[ObjectView class]])
			[view removeFromSuperview];
	}
	for (PHObject * obj in objects)
	{
		ObjectView * view = [[ObjectView alloc] init];
		view.object = obj;
		[worldView addSubview:view];
	}
}

-(double)scalingFactor
{
	return scalingFactor;
}


-(void)setScalingFactor:(double)sf
{
	NSRect frame = worldView.frame;
	NSRect bounds = worldView.bounds;
	frame.size.width*= sf/scalingFactor;
	frame.size.height*= sf/scalingFactor;
	NSRect visible = [scrollView documentVisibleRect];
	worldView.scale = NSMakeSize(sf, sf);
	worldView.frame = frame;
	worldView.bounds = bounds;
	visible.origin.x+=visible.size.width/2;
	visible.origin.y+=visible.size.height/2;
	visible.size.width/= sf/scalingFactor;
	visible.size.height/= sf/scalingFactor;
	visible.origin.x-=visible.size.width/2;
	visible.origin.y-=visible.size.height/2;
	//[worldView scrollRectToVisible:visible];
	scalingFactor = sf;
	[self adjustForVisibleRect:visible];
}

-(void)adjustForVisibleRect:(NSRect)visibleRect
{
	NSRect bounds = worldView.bounds;
	NSRect oldBounds = bounds;
	NSRect newBounds = bounds;
	BOOL touched = YES;
	BOOL rt = NO;
	while (touched)
	{
		touched = NO;
		if (bounds.origin.x>=visibleRect.origin.x-visibleRect.size.width*0.1)
		{
			newBounds.origin.x-=visibleRect.size.width*4;
			newBounds.size.width+=visibleRect.size.width*4;
			touched = YES;
		}
		if (bounds.origin.y>=visibleRect.origin.y-visibleRect.size.height*0.1)
		{
			newBounds.origin.y-=visibleRect.size.height*4;
			newBounds.size.height+=visibleRect.size.height*4;
			touched = YES;
		}
		if (bounds.origin.x+bounds.size.width<=visibleRect.origin.x+visibleRect.size.width*1.1f)
		{
			newBounds.size.width+=visibleRect.size.width*4;
			touched=YES;
		}
		if (bounds.origin.y+bounds.size.height<=visibleRect.origin.y+visibleRect.size.height*1.1f)
		{
			newBounds.size.height+=visibleRect.size.height*4;
			touched=YES;
		}
		bounds = newBounds;
		if (touched)
			rt = YES;
	}
	worldView.frame = NSMakeRect(0, 0, bounds.size.width*scalingFactor, bounds.size.height*scalingFactor);
	worldView.bounds = bounds;
	if (rt)
	{
		NSLog(@"ob:%f %f %f %f  nb:%f %f %f %f visible:%f %f %f %f",
			  oldBounds.origin.x,oldBounds.origin.y,oldBounds.size.width,oldBounds.size.height,
			  bounds.origin.x,bounds.origin.y,bounds.size.width,bounds.size.height,
			  visibleRect.origin.x,visibleRect.origin.y,visibleRect.size.width,visibleRect.size.height);
		//[scrollView scrollClipView:[scrollView contentView] toPoint:visibleRect.origin];
		[worldView scrollRectToVisible:visibleRect];
		[scrollView reflectScrolledClipView:[scrollView contentView]];
	}
 }

-(void)expandLeft:(id)sender
{
	NSRect visibleRect = [scrollView documentVisibleRect];
	NSRect bounds = worldView.bounds;
	
	bounds.origin.x-=visibleRect.size.width*2;
	bounds.size.width+=visibleRect.size.width*2;
	
	worldView.frame = NSMakeRect(0, 0, bounds.size.width*scalingFactor, bounds.size.height*scalingFactor);
	worldView.bounds = bounds;
	[worldView scrollRectToVisible:visibleRect];
	[scrollView reflectScrolledClipView:[scrollView contentView]];
}
-(void)expandDown:(id)sender
{
	NSRect visibleRect = [scrollView documentVisibleRect];
	NSRect bounds = worldView.bounds;
	
	bounds.origin.y-=visibleRect.size.height*2;
	bounds.size.height+=visibleRect.size.height*2;
	
	worldView.frame = NSMakeRect(0, 0, bounds.size.width*scalingFactor, bounds.size.height*scalingFactor);
	worldView.bounds = bounds;
	[worldView scrollRectToVisible:visibleRect];
	[scrollView reflectScrolledClipView:[scrollView contentView]];
}

-(void)expandRight:(id)sender
{
	NSRect visibleRect = [scrollView documentVisibleRect];
	NSRect bounds = worldView.bounds;
	
	bounds.size.width+=visibleRect.size.width*2;
	
	worldView.frame = NSMakeRect(0, 0, bounds.size.width*scalingFactor, bounds.size.height*scalingFactor);
	worldView.bounds = bounds;
	
	[worldView scrollRectToVisible:visibleRect];
	[scrollView reflectScrolledClipView:[scrollView contentView]];
}
-(void)expandUp:(id)sender
{
	NSRect visibleRect = [scrollView documentVisibleRect];
	NSRect bounds = worldView.bounds;
	
	bounds.size.height+=visibleRect.size.height*2;
	
	worldView.frame = NSMakeRect(0, 0, bounds.size.width*scalingFactor, bounds.size.height*scalingFactor);
	worldView.bounds = bounds;
	[worldView scrollRectToVisible:visibleRect];
	[scrollView reflectScrolledClipView:[scrollView contentView]];
}

-(IBAction)scrollToOrigin:(id)sender
{
	NSRect visibleRect = [scrollView documentVisibleRect];
	visibleRect.origin.x = -visibleRect.size.width/2;
	visibleRect.origin.y = -visibleRect.size.height/2;
	[worldView scrollRectToVisible:visibleRect];
	[scrollView reflectScrolledClipView:[scrollView contentView]];
}

-(void)scrollViewHasScrolled
{
//	NSLog(@"scrollViewHasScrolled");
//	[self adjustForVisibleRect:[scrollView documentVisibleRect]];
}

@end
