//
//  SolidColorView.m
//  Level Designer
//
//  Created by Marius Petcu on 6/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "SolidColorView.h"


@implementation SolidColorView

@synthesize color;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        color = [[NSColor whiteColor] retain];
    }
    
    return self;
}

- (void)dealloc
{
    [color release];
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [[self color] setFill];
    NSRectFill(dirtyRect);
}

@end
