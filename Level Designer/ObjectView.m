//
//  ObjectView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ObjectView.h"


@implementation ObjectView
@synthesize color;
- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        color = [[NSColor redColor] retain];
    }
    return self;
}

-(void)dealloc
{
	[color release];
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
	[color setFill];
    NSRectFill(dirtyRect);
}

@end
