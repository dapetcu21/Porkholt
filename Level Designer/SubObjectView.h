//
//  SubObjectView.h
//  Level Designer
//
//  Created by Marius Petcu on 6/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef struct
{
    double x,y,w,h,r,rot;
} positionState;

@class ObjectView;
@class PHObjectProperty;
@interface SubObjectView : NSView {
@private
    ObjectView * objectView;
    PHObjectProperty * property;
    NSImage * img;
    NSString * imagePath;
    int type,shape;
    BOOL failed;
    
    PHObjectProperty * pos;
    PHObjectProperty * posX;
    PHObjectProperty * posY;
    PHObjectProperty * posW;
    PHObjectProperty * posH;
    PHObjectProperty * path;
    PHObjectProperty * rad;
    PHObjectProperty * sh;
    PHObjectProperty * rot;
    
    NSTrackingArea * trackingArea;
    
    int dragTag;
    NSPoint dragPoint;
    positionState initialState;
    
    BOOL showMarkers;
}

enum kSubObjectViewType
{
    kSOTImage = 0,
    kSOTFixture
};

enum kSubObjectViewShape
{
    kSOSRect = 0,
    kSOSCircle
};

@property(nonatomic,assign) ObjectView * objectView;
@property(nonatomic,retain) PHObjectProperty * property;
@property(nonatomic,assign) int type;
@property(nonatomic,assign) int shape;

-(void)reloadData;
-(void)rebuildCachedProperties;
-(void)weakRebuildCachedProperties;

-(void)undoable:(NSUndoManager*)man move:(NSPoint)delta;
-(void)move:(NSPoint)delta;
-(double)rotation;
-(void)setRotation:(double)val;
-(BOOL)supportsRotation;
-(void)undoable:(NSUndoManager*)man setRotation:(double)val;

+(BOOL)rect:(NSRect)r1 inView:(NSView*)v1 intersectsRect:(NSRect)r2 inView:(NSView*)v2;
-(BOOL)intersectsRect:(NSRect)rect fromView:(NSView*)v;
-(BOOL)intersectsPoint:(NSPoint)pnt;

-(int)resizeAreaHit:(NSPoint)pnt;
-(void)beginDrag:(NSEvent*)theEvent withResizeArea:(int)tag;
-(void)moveDrag:(NSEvent*)theEvent;
-(void)endDrag:(NSEvent*)theEvent;

-(void)setEditMode:(BOOL)val;

-(BOOL)resetAspectRatioUndoable:(NSUndoManager*)um ;

@property(nonatomic,assign)BOOL showMarkers;

@end