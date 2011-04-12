//
//  ViewScaling.h
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface NSView (ScaleUtilities) 
	@property (assign) NSSize scale; 
	- (void)resetScaling; 
@end


