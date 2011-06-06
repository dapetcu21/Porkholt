//
//  ObjectView.h
//  Level Designer
// 
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PHObject;
@class WorldView;
@class SolidColorView;

@interface ObjectView : NSView {
	PHObject * object;
	BOOL selected;
    BOOL ignoreModified;
    SolidColorView * centerView;
}

@property(nonatomic,retain) PHObject * object;
@property(nonatomic,assign) BOOL selected;
@property(nonatomic,assign) BOOL ignoreModified;
@property(nonatomic,readonly) WorldView * worldView;
@property(nonatomic,assign) BOOL showMarkers;

-(void)modified;

- (void)rebuildSubviews;

- (BOOL)intersectsRect:(NSRect)rect;
- (BOOL)intersectsRectInObjectCoords:(NSRect)rect;
- (BOOL)pointIntersectsSelf:(NSPoint)pnt;

- (void)adaptForView:(NSView*)v;

@end
