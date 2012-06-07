/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
