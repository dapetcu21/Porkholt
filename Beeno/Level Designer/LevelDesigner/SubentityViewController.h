/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "MultipleListController.h"

@class PLTableView;
@class ImageViewController;
@class FixtureViewController;
@class OverlayController;
@class OverlayView;

@interface SubentityViewController : MultipleListController
{
    IBOutlet ImageViewController * imageController;
    IBOutlet FixtureViewController * fixtureController;
    IBOutlet OverlayView * imageDetailView;
    IBOutlet OverlayView * fixtureDetailView;
    IBOutlet OverlayController * overlayController;
}

@property(nonatomic,retain) IBOutlet PLTableView * imagesView;
@property(nonatomic,retain) IBOutlet PLTableView * fixturesView;
@end
