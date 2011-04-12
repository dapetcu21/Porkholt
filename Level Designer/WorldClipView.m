//
//  WorldClipView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "WorldClipView.h"
#import "WorldController.h"

@implementation WorldClipView
@synthesize controller;

- (void)scrollToPoint:(NSPoint)newOrigin
{
	[super scrollToPoint:newOrigin];
	if (!insideScroll)
	{
		insideScroll = YES;
		[controller scrollViewHasScrolled];
		insideScroll = NO;
	}
}

-(IBAction)expandLeft:(id)sender
{
	[controller expandLeft:sender];
}
-(IBAction)expandRight:(id)sender
{
	[controller expandRight:sender];
}
-(IBAction)expandUp:(id)sender
{
	[controller expandUp:sender];
}
-(IBAction)expandDown:(id)sender
{
	[controller expandDown:sender];
}

-(IBAction)scrollToOrigin:(id)sender
{
	[controller scrollToOrigin:sender];
}

@end
