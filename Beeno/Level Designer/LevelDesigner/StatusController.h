//
//  StatusController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class ObjectController;

@interface StatusController : NSObject
{
    IBOutlet ObjectController * objectController;
    IBOutlet NSButton * showMarkers;
    IBOutlet NSButton * showImages;
    IBOutlet NSButton * showJoints;
    IBOutlet NSButton * showFixtures;
    IBOutlet NSMatrix * mode;
}

-(void)flagsChanged;
-(IBAction)stuffChanged:(id)sender;

@end
