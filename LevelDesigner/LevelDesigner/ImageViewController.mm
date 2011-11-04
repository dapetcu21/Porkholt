//
//  ImageViewController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/6/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "ImageViewController.h"
#import "PLImage.h"
#import "PLObject.h"
#import "PHGeometry.h"
#import "PLBezier.h"


@implementation ImageViewController

-(PLImage*)model
{
    return model;
}

#define setNumberField(f,n) [(f) setStringValue:[NSString stringWithFormat:@"%.4lf",(double)(n)]]

static PHColor PHColorFromPLColor(const struct PLColor & c)
{
    return PHColor(c.r,c.g,c.b,c.a);
}

-(void)populateView
{
    BOOL enabled = (model && !model.readOnly && !model.object.readOnly);

    [className setEnabled:enabled];
    [frame_x setEnabled:enabled];
    [frame_y setEnabled:enabled];
    [frame_w setEnabled:enabled];
    [frame_h setEnabled:enabled];
    [portion_x setEnabled:enabled];
    [portion_y setEnabled:enabled];
    [portion_w setEnabled:enabled];
    [portion_h setEnabled:enabled];
    [repeat_x setEnabled:enabled];
    [repeat_y setEnabled:enabled];
    [rotation setEnabled:enabled];
    [fileName setEnabled:enabled];
    [hFlipped setEnabled:enabled];
    [vFlipped setEnabled:enabled];
    [tag setEnabled:enabled];
    [tint setEnabled:enabled];
    [alphaSlider setEnabled:enabled];
    [curve setEnabled:enabled];
    [constrain setEnabled:enabled];
    
    if (!model) return;
    
    [className selectItemWithTitle:model.imageClass];
    NSRect frame = model.frame;
    setNumberField(frame_x, frame.origin.x);
    setNumberField(frame_y, frame.origin.y);
    setNumberField(frame_w, frame.size.width);
    setNumberField(frame_h, frame.size.height);
    NSRect portion = model.portion;
    setNumberField(portion_x, portion.origin.x);
    setNumberField(portion_y, portion.origin.y);
    setNumberField(portion_w, portion.size.width);
    setNumberField(portion_h, portion.size.height);
    setNumberField(repeat_x, model.repeatX);
    setNumberField(repeat_y, model.repeatY);
    setNumberField(rotation, model.rotation);
    [tag setStringValue:[NSString stringWithFormat:@"%d",(int)model.tag]];
    [alphaSlider setFloatValue:model.alpha];
    [hFlipped setState:model.horizontallyFlipped?NSOnState:NSOffState];
    [vFlipped setState:model.verticallyFlipped?NSOnState:NSOffState];
    [constrain setState:model.constrainToFrame?NSOnState:NSOffState];
    [curve setState:model.bezierCurve?NSOnState:NSOffState];
    PHColor color = PHColorFromPLColor(model.tint);
    NSColor * clr = [NSColor colorWithCalibratedRed:1 green:1 blue:1 alpha:1];
    if (color.isValid())
        clr = [NSColor colorWithCalibratedRed:color.r green:color.g blue:color.b alpha:color.a];
    [tint setColor:clr];
    NSString * fn = model.fileName;
    if (!fn) 
        fn = @"";
    [fileName setStringValue:fn];
}

-(IBAction)valueChanged:(id)sender
{
    modifyFromInside = YES;
    if (sender == className)
        model.imageClass = [[className selectedItem] title];
    if (sender == fileName)
        model.fileName = [fileName stringValue];
    if (sender == tag)
        model.tag = [tag intValue];
    if (sender == alphaSlider)
        model.alpha = [alphaSlider doubleValue];
    if (sender == repeat_x)
        model.repeatX = [repeat_x doubleValue];
    if (sender == repeat_y)
        model.repeatY = [repeat_y doubleValue];
    if (sender == hFlipped) 
        model.horizontallyFlipped = ([hFlipped state] == NSOnState);
    if (sender == vFlipped)
        model.verticallyFlipped = ([vFlipped state] == NSOnState);
    if (sender == constrain)
        model.constrainToFrame = ([constrain state] == NSOnState);
    if (sender == rotation)
        model.rotation = [rotation doubleValue];
    if (sender == frame_x || sender == frame_y || sender == frame_w || sender == frame_h)
        model.frame = NSMakeRect([frame_x doubleValue], [frame_y doubleValue], [frame_w doubleValue], [frame_h doubleValue]);
    if (sender == portion_x || sender == portion_y || sender == portion_w || sender == portion_h)
        model.portion = NSMakeRect([portion_x doubleValue], [portion_y doubleValue], [portion_w doubleValue], [portion_h doubleValue]);
    if (sender == tint)
    {
        PLColor c;
        NSColor * clr = [[tint color] colorUsingColorSpaceName:@"NSCalibratedRGBColorSpace"];
        c.r = [clr redComponent];
        c.g = [clr greenComponent];
        c.b = [clr blueComponent];
        c.a = [clr alphaComponent];
        if (c.a == 1 && c.r == 1 && c.g == 1 && c.b == 1)
            c.a=c.r=c.g=c.b=-1;
        model.tint = c;
    }
    if (sender == curve)
    {
        BOOL old = (model.bezierCurve!=nil);
        BOOL nw = [sender state]==NSOnState;
        if (old&&!nw)
            [model setBezierCurve:nil];
        if (!old&&nw)
            [model setBezierCurve:[[[PLBezier alloc] init] autorelease]];
    }
    modifyFromInside = NO;
}

-(void)imageChanged
{
    if (modifyFromInside) return;
    [self populateView];
}

-(void)setModel:(PLImage *)_model
{
    model.viewController = nil;
    [model release];
    [_model retain];
    _model.viewController = self;
    model = _model;
    [self populateView];
}



@end
