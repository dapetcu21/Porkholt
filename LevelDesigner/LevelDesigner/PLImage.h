//
//  PLImage.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

@class PLProperty;

struct PLColor
{
    double r,g,b,a;
};

@interface PLImage : PLEntity
{
    NSString * imageClass;
    NSRect portion;
    NSRect frame;
    int tag;
    double rotation;
    BOOL horizontallyFlipped;
    BOOL verticallyFlipped;
    struct PLColor tint;
    double alpha;
    NSString * fileName;
}

@property(nonatomic,retain) NSString * imageClass;
@property(nonatomic,assign) NSRect portion;
@property(nonatomic,assign) NSRect frame;
@property(nonatomic,assign) int tag;
@property(nonatomic,assign) double rotation;
@property(nonatomic,assign) BOOL horizontallyFlipped;
@property(nonatomic,assign) BOOL verticallyFlipped;
@property(nonatomic,assign) struct PLColor tint;
@property(nonatomic,retain) NSString * fileName;
@property(nonatomic,assign) double alpha;

-(id)initFromProperty:(PLProperty*)prop;
-(void)writeToFile:(NSMutableString*)file;
+(NSArray*)imagesFromProperty:(PLProperty*)prop;

@end
