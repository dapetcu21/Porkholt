//
//  WorldClipView.h
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class WorldController;
@interface WorldClipView : NSClipView {
	WorldController * controller;
	BOOL insideScroll;
}

@property(nonatomic,assign) WorldController * controller;

-(IBAction)expandLeft:(id)sender;
-(IBAction)expandRight:(id)sender;
-(IBAction)expandUp:(id)sender;
-(IBAction)expandDown:(id)sender;
-(IBAction)scrollToOrigin:(id)sender;

@end
