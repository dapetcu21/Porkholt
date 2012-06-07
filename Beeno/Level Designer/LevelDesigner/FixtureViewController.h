/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@class PLFixture;
@class OverlayView;
@interface FixtureViewController : NSObject
{
    IBOutlet OverlayView * view;
    IBOutlet NSView * boxView;
    IBOutlet NSView * circleView;
    IBOutlet NSView * freestyleView;
    IBOutlet NSView * dummyView;
    PLFixture * model;
    
    IBOutlet NSPopUpButton * type;
    IBOutlet NSTextField * pos_x;
    IBOutlet NSTextField * pos_y;
    IBOutlet NSTextField * box_x;
    IBOutlet NSTextField * box_y;
    IBOutlet NSTextField * box_w;
    IBOutlet NSTextField * box_h;
    IBOutlet NSTextField * rotation;
    IBOutlet NSTextField * radius;
    
    IBOutlet NSTextField * friction;
    IBOutlet NSTextField * density;
    IBOutlet NSTextField * restitution;
    
    IBOutlet NSTextField * groupIndex;
    IBOutlet NSTextField * categoryBits;
    IBOutlet NSTextField * maskBits;
    
    BOOL modifyFromInside;
}

@property(nonatomic,retain) IBOutlet PLFixture * model;
-(IBAction)valueChanged:(id)sender;
-(void)fixtureChanged;

@end
