//
//  FixtureViewController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/7/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "FixtureViewController.h"
#import "OverlayController.h"
#import "PLFixture.h"
#import "PLObject.h"
#import "OverlayView.h"

@implementation FixtureViewController

-(PLFixture*)model
{
    return model;
}

#define setNumberField(f,n) [(f) setStringValue:[NSString stringWithFormat:@"%.4lf",(double)(n)]]
#define setIntField(f,n) [(f) setStringValue:[NSString stringWithFormat:@"%d",(int)(n)]]

-(void)populateView
{
    BOOL enabled = (model && !model.readOnly && !model.object.readOnly);
    
    [type setEnabled:enabled];
    [pos_x setEnabled:enabled];
    [pos_y setEnabled:enabled];
    [box_x setEnabled:enabled];
    [box_y setEnabled:enabled];
    [box_w setEnabled:enabled];
    [box_h setEnabled:enabled];
    [rotation setEnabled:enabled];
    [radius setEnabled:enabled];
    [friction setEditable:enabled];
    [density setEditable:enabled];
    [restitution setEditable:enabled];
    [groupIndex setEditable:enabled];
    [categoryBits setEditable:enabled];
    [maskBits setEditable:enabled];
    
    if (!model) return;
    
    [type selectItemWithTag:model.shape];
    NSPoint p = model.position;
    setNumberField(pos_x, p.x);
    setNumberField(pos_y, p.y);
    NSRect r = model.box;
    setNumberField(box_x, r.origin.x);
    setNumberField(box_y, r.origin.y);
    setNumberField(box_w, r.size.width);
    setNumberField(box_h, r.size.height);
    setNumberField(rotation, model.rotation);
    setNumberField(radius, model.radius);
    setNumberField(friction, model.friction);
    setNumberField(density, model.density);
    setNumberField(restitution, model.restitution);
    setIntField(groupIndex, model.groupIndex);
    setIntField(categoryBits, model.categoryBits);
    setIntField(maskBits, model.maskBits);
    
    [circleView removeFromSuperview];
    [boxView removeFromSuperview];
    [freestyleView removeFromSuperview];
    NSView * v = nil;
    switch (model.shape) {
        case PLFixtureCircle:
            v = circleView;
            break;
        case PLFixtureRect:
            v = boxView;
            break;
        case PLFixtureFreestyle:
            v = freestyleView;
            break;
    }
    double delta = v.frame.size.height-dummyView.frame.size.height;
    r = [dummyView frame];
    r.size.height+=delta;
    r.origin.y-=delta;
    [dummyView setFrame:r];
    r = [view frame];
    r.size.height+=delta;
    r.origin.y-=delta;
    [view setFrame:r];
    [view reshape];
    [v setFrame:[dummyView bounds]];
    [dummyView addSubview:v];
    
}

-(IBAction)valueChanged:(id)sender
{
    modifyFromInside = YES;
    if (sender == type)
    {
        modifyFromInside = NO;
        model.shape = (int)[[type selectedItem] tag];
    }
    if (sender == pos_x || sender == pos_y)
        model.position = NSMakePoint([pos_x floatValue], [pos_y floatValue]);
    if (sender == radius)
        model.radius = [radius floatValue];
    if (sender == rotation)
        model.rotation = [rotation floatValue];
    if (sender == box_x || sender == box_y || sender == box_w || sender == box_h)
        model.box = NSMakeRect([box_x floatValue], [box_y floatValue], [box_w floatValue], [box_h floatValue]);
    if (sender == friction)
        model.friction = [friction floatValue];
    if (sender == density)
        model.density = [density floatValue];
    if (sender == restitution)
        model.restitution = [restitution floatValue];
    if (sender == groupIndex)
        model.groupIndex = [groupIndex intValue];
    if (sender == categoryBits)
        model.categoryBits = [categoryBits intValue];
    if (sender == maskBits)
        model.maskBits = [maskBits intValue];
    modifyFromInside = NO;
}

-(void)fixtureChanged
{
    if (modifyFromInside) return;
    [self populateView];
}

-(void)setModel:(PLFixture*)_model
{
    model.viewController = nil;
    [model release];
    [_model retain];
    _model.viewController = self;
    model = _model;
    [self populateView];
}


@end
