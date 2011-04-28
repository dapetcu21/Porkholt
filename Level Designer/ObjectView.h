//
//  ObjectView.h
//  Level Designer
// 
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PHObject;
@interface ObjectView : NSView {
	PHObject * object;
	BOOL selected;
	double rotation;
}

@property(nonatomic,retain) PHObject * object;
@property(nonatomic,assign) BOOL selected;
@property(nonatomic,assign) double rotation;
@end
