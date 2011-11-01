//
//  JointController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PLJoint;
@interface JointController : NSObject
{
    PLJoint * model;
    IBOutlet NSView * view;
}

-(void)jointChanged;
-(IBAction)controlsChanged:(id)sender;

@property(nonatomic,retain) PLJoint * model;
@end
