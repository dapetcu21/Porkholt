//
//  SubObjectView.h
//  Level Designer
//
//  Created by Marius Petcu on 6/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

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

-(BOOL)intersectsRect:(NSRect)rect;
-(BOOL)intersectsPoint:(NSPoint)pnt;

@end
