//
//  WorldController.h
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ObjectController;
@interface WorldController : NSObject {
	IBOutlet ObjectController * objectController;
	NSArrayController * controller;
	IBOutlet NSScrollView * scrollView;
	NSView * worldView;
	double scalingFactor;
	NSArray * objects;
	NSArray * selection;
}

@property(nonatomic,assign) double scalingFactor;

-(void)scrollViewHasScrolled;
-(void)adjustForVisibleRect:(NSRect)visibleRect;

-(IBAction)expandLeft:(id)sender;
-(IBAction)expandRight:(id)sender;
-(IBAction)expandUp:(id)sender;
-(IBAction)expandDown:(id)sender;
-(IBAction)scrollToOrigin:(id)sender;

-(void)updateSubviews;

@end
