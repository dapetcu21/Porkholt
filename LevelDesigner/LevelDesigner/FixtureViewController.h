//
//  FixtureViewController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/7/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PLFixture;
@interface FixtureViewController : NSObject
{
    IBOutlet NSView * view;
    PLFixture * model;
    
    BOOL modifyFromInside;
}

@property(nonatomic,retain) IBOutlet PLFixture * model;
-(IBAction)valueChanged:(id)sender;
-(void)fixtureChanged;

@end
