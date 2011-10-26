//
//  PLImage.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLSubEntity.h"

@class PLProperty;
@class PLObject;

struct PLColor
{
    double r,g,b,a;
};

#ifdef __cplusplus
class PLImageView;
#else
#define PLImageView void
#endif
@class PLBezier;

@class ImageViewController;
@interface PLImage : PLEntity<PLSubEntity>
{
    NSString * imageClass;
    NSRect portion;
    NSRect frame;
    int tag;
    double rotation;
    BOOL horizontallyFlipped;
    BOOL verticallyFlipped;
    BOOL constrainToFrame;
    struct PLColor tint;
    double alpha;
    NSString * fileName;
    PLBezier * bezierCurve;
    
    ImageViewController * viewController;
}

@property(nonatomic,retain) NSString * imageClass;
@property(nonatomic,assign) NSRect portion;
@property(nonatomic,assign) NSRect frame;
@property(nonatomic,assign) int tag;
@property(nonatomic,assign) double rotation;
@property(nonatomic,assign) BOOL horizontallyFlipped;
@property(nonatomic,assign) BOOL verticallyFlipped;
@property(nonatomic,assign) BOOL constrainToFrame;
@property(nonatomic,assign) struct PLColor tint;
@property(nonatomic,retain) NSString * fileName;
@property(nonatomic,assign) double alpha;
@property(nonatomic,assign) PLBezier * bezierCurve;

@property(nonatomic,assign) PLImageView * actor;

-(id)initFromProperty:(PLProperty*)prop;
-(void)writeToFile:(NSMutableString*)file;
+(NSArray*)imagesFromProperty:(PLProperty*)prop;

@property(nonatomic,readonly) PLObject * object;
@property(nonatomic,assign) ImageViewController * viewController;

@end
