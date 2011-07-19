//
//  ImageView.m
//  PHFontCreator
//
//  Created by Marius Petcu on 7/19/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ImageView.h"


@implementation ImageView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    
    return self;
}

- (void)dealloc
{
    CGContextRelease(bitmapContext);
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    [[NSColor clearColor] setFill];
    [NSBezierPath fillRect:dirtyRect];
    CGContextRef c = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGImageRef i = CGBitmapContextCreateImage(bitmapContext);
    double w = CGImageGetWidth(i);
    double h = CGImageGetHeight(i);
    double rw = bounds.size.width;
    double rh = bounds.size.height;
    if (rw/w*h<=rh)
    {
        h = rw/w*h;
        w = rw;
    } else {
        w = rh/h*w;
        h = rh;
    }
    CGRect r;
    r.origin.x = bounds.origin.x+(rw-w)/2;
    r.origin.y = bounds.origin.y+(rh-h)/2;
    r.size.width = w;
    r.size.height = h;
    CGContextDrawImage(c, r, i);
    CGImageRelease(i);
}

-(CGContextRef) bitmapContext
{
    return bitmapContext;
}

-(void) setBitmapContext:(CGContextRef)bc
{
    CGContextRetain(bc);
    CGContextRelease(bitmapContext);
    bitmapContext = bc;
    [self setNeedsDisplay:YES];
}

@end
