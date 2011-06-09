//
//  WorldController.h
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ObjectController;
@class WorldView;
@class PHObject;

@interface WorldController : NSObject {
	IBOutlet ObjectController * objectController;
	NSArrayController * controller;
	IBOutlet WorldView * worldView;
	double scalingFactor;
	NSArray * objects;
	NSArray * selection;
    BOOL showImages;
    BOOL showFixtures;
    BOOL showMarkers;
    BOOL showJoints;
    BOOL objectMode;
    PHObject * currentObject;
}

@property(nonatomic,assign) double scalingFactor;

@property(nonatomic,assign) BOOL showImages;
@property(nonatomic,assign) BOOL showFixtures;
@property(nonatomic,assign) BOOL showMarkers;
@property(nonatomic,assign) BOOL showJoints;
@property(nonatomic,assign) BOOL objectMode;
@property(nonatomic,readonly) PHObject * currentObject;
@property(nonatomic,readonly) ObjectController * objectController;

-(void)updateSubviews;

@end
