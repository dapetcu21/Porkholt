//
//  StatusController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "StatusController.h"
#import "ObjectController.h"

@implementation StatusController

-(void)awakeFromNib
{
    [self flagsChanged];
}

-(void)flagsChanged
{
    [showMarkers setState:objectController.showMarkers];
    [showJoints setState:objectController.showJoints];
    [showFixtures setState:objectController.showFixtures];
    [showImages setState:objectController.showImages];
    [mode selectCellWithTag:objectController.objectMode?1:0];
    [mode setEnabled:objectController.objectModeEnabled];
}


-(IBAction)stuffChanged:(id)sender
{
    if (sender == showJoints)
        [objectController setShowJoints:[showJoints state]==NSOnState];
    if (sender == showImages)
        [objectController setShowImages:[showImages state]==NSOnState];
    if (sender == showFixtures)
        [objectController setShowFixtures:[showFixtures state]==NSOnState];
    if (sender == showMarkers)
        [objectController setShowMarkers:[showMarkers state]==NSOnState];
    if (sender == mode)
        [objectController setObjectMode:[mode selectedTag]==1];
}

@end
