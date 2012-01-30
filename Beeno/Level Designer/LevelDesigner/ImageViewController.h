//
//  ImageViewController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/6/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PLImage;
@interface ImageViewController : NSObject
{
    IBOutlet NSView * view;
    PLImage * model;
    
    IBOutlet NSPopUpButton * className;
    IBOutlet NSTextField * frame_x;
    IBOutlet NSTextField * frame_y;
    IBOutlet NSTextField * frame_w;
    IBOutlet NSTextField * frame_h;
    IBOutlet NSTextField * portion_x;
    IBOutlet NSTextField * portion_y;
    IBOutlet NSTextField * repeat_x;
    IBOutlet NSTextField * repeat_y;
    IBOutlet NSTextField * portion_w;
    IBOutlet NSTextField * portion_h;
    IBOutlet NSTextField * rotation;
    IBOutlet NSTextField * fileName;
    IBOutlet NSButton * hFlipped;
    IBOutlet NSButton * vFlipped;
    IBOutlet NSButton * curve;
    IBOutlet NSButton * constrain;
    IBOutlet NSTextField * tag;
    IBOutlet NSColorWell * tint;
    IBOutlet NSSlider * alphaSlider;
    
    BOOL modifyFromInside;
}

@property(nonatomic,retain) IBOutlet PLImage * model;
-(IBAction)valueChanged:(id)sender;
-(void)imageChanged;

@end
