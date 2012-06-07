/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLBezier.h"
#include "PHBezierPath.h"
#import "EntityController.h"
#include "PLBezierView.h"


@interface PLCopyBezier : NSObject
{
@public
    set<PHRange> curves;
    vector<PHBezierPath::anchorPoint> anchors;
}
@end
@implementation PLCopyBezier
@end

@interface PHPointC : NSObject<NSCoding>
{
    PHBezierPath::anchorPoint anchor;
}
-(id)initWithAnchor:(PHBezierPath::anchorPoint)an;
@property(assign,nonatomic) PHBezierPath::anchorPoint anchor;
@end

@implementation PHPointC
@synthesize anchor;
-(id)initWithAnchor:(PHBezierPath::anchorPoint)an
{
    self = [super init];
    if (self) {
        anchor = an;
    }
    return self;
}
-(void)encodeWithCoder:(NSCoder *)aCoder
{
    [aCoder encodeInt32:anchor.tag forKey:@"tag"];
    [aCoder encodeDouble:anchor.point.x forKey:@"x"];
    [aCoder encodeDouble:anchor.point.y forKey:@"y"];
}
-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super init];
    if (self)
    {
        anchor.tag = [aDecoder decodeInt32ForKey:@"tag"];
        anchor.point.x = [aDecoder decodeDoubleForKey:@"x"];
        anchor.point.y = [aDecoder decodeDoubleForKey:@"y"];
    }
    return self;
}
@end

@interface PHRangeC : NSObject<NSCoding>
{
    PHRange range;
}
-(id)initWithRange:(PHRange)r;
@property(nonatomic,assign) PHRange range;
@end

@implementation PHRangeC
@synthesize range;
-(id)initWithRange:(PHRange)r
{
    self = [super init];
    if (self) {
        range = r;
    }
    return self;
}
-(void)encodeWithCoder:(NSCoder *)aCoder
{
    [aCoder encodeInt32:range.start forKey:@"start"];
    [aCoder encodeInt32:range.length forKey:@"length"];
}
-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super init];
    if (self)
    {
        range.start = [aDecoder decodeInt32ForKey:@"start"];
        range.length = [aDecoder decodeInt32ForKey:@"length"];
    }
    return self;
}
@end

class PLBezierDelegate : public PHObject
{
    PLBezier * d;
public:
    PLBezierDelegate(PLBezier * bezier) : d(bezier) {}
    void callback(PHBezierPath * sender, void * ud)
    {
        [d modelChanged];
    }
};

@implementation PLBezier
@synthesize undoManager;

- (id)initFromLua:(lua_State *)L
{
    self = [super initFromLua:L];
    if (self) {
        delegate = new PLBezierDelegate(self);
        if (L)
            curve = PHBezierPath::nonUniqueFromLua(L);
        else {
            curve = new PHBezierPath;
            PHBezierPath::anchorPoint v[3] = { PHBezierPath::anchorPoint(PHPoint(0,0),1), PHBezierPath::anchorPoint(PHPoint(1,0),2), PHBezierPath::anchorPoint(PHPoint(0,1),3) };
            curve->insertAnchorPoints(v, 3, 0);
        }
        curve->addCallback(PHInv(delegate,PLBezierDelegate::callback,NULL));
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone
{
    PLBezier * c = [[PLBezier allocWithZone:zone] initFromLua:NULL];
    if (c)
    {
        c->curve->setAnchorPoints(curve->anchorPoints());
        c->curve->setBezierCurves(curve->bezierCurves());
    }
    return c;
}

-(void)encodeWithCoder:(NSCoder *)aCoder
{
    const vector<PHBezierPath::anchorPoint> & ap = curve->anchorPoints();
    NSMutableArray * anchors = [[[NSMutableArray alloc] initWithCapacity:ap.size()] autorelease];
    for (vector<PHBezierPath::anchorPoint>::const_iterator i = ap.begin(); i!=ap.end(); i++)
        [anchors addObject:[[[PHPointC alloc] initWithAnchor:(*i)] autorelease]];
    [aCoder encodeObject:anchors forKey:@"anchors"];
    
    const set<PHRange> & bp = curve->bezierCurves();
    NSMutableArray * curves = [[[NSMutableArray alloc] initWithCapacity:bp.size()] autorelease];
    for (set<PHRange>::const_iterator i = bp.begin(); i!=bp.end(); i++)
        [curves addObject:[[[PHRangeC alloc] initWithRange:(*i)] autorelease]];
    [aCoder encodeObject:curves forKey:@"curves"];
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initFromLua:NULL];
    if (self)
    {
        delegate = new PLBezierDelegate(self);
        curve = new PHBezierPath();
        NSArray * anchors = [aDecoder decodeObjectForKey:@"anchors"];
        vector<PHBezierPath::anchorPoint> ap;
        for (PHPointC * c in anchors)
            ap.push_back(c.anchor);
        curve->insertAnchorPoints(ap.begin(), ap.end(), 0);
        
        NSArray * curves = [aDecoder decodeObjectForKey:@"curves"];
        for(PHRangeC * c in curves)
            curve->addCurve(c.range);
        
        curve->addCallback(PHInv(delegate,PLBezierDelegate::callback,NULL));
    }
    return self;
}

-(BOOL)isEqual:(PLBezier*)object
{
    if (![object isKindOfClass:[PLBezier class]]) return NO;
    return (*[object bezierPath])==(*curve);
}

-(void)writeToFile:(NSMutableString*)file
{
    const vector<PHBezierPath::anchorPoint> & v = curve->anchorPoints();
    [file appendFormat:@"{\n\tpoints = { n=%u",(unsigned int)v.size()];
    int idx = 0;
    for (vector<PHBezierPath::anchorPoint>::const_iterator i = v.begin(); i!=v.end(); i++, idx++)
    {
        [file appendFormat:@",\n\t\t[%d] = { point = point(%lf,%lf)",idx,i->point.x,i->point.y];
        if (i->tag)
            [file appendFormat:@", tag = %d",i->tag];
        [file appendString:@" }"];
    }
    [file appendFormat:@"},\n\tcurves = { n=%u",(unsigned int)curve->bezierCurves().size()];
    const set<PHRange> & c = curve->bezierCurves();
    idx = 0;
    for (set<PHRange>::const_iterator i = c.begin(); i!=c.end(); i++, idx++)
        [file appendFormat:@",\n\t\t[%d] = range(%d,%d)",idx,i->start,i->length];
    [file appendString:@"}\n\t}"];
}

-(PHBezierPath*)bezierPath
{
    return curve;
}

-(void)modelChanged
{
    [(EntityController*)owner entityChanged:self];
    for (set<PLBezierView*>::iterator i = actors.begin(); i!= actors.end(); i++)
        (*i)->modelChanged();
}

-(void)addActor:(PLBezierView*)a
{
    if (!fromWithin)
        actors.insert(a);
}

-(void)removeActor:(PLBezierView*)a
{
    if (!fromWithin)
        actors.erase(a);
}

-(void)applyUndoState:(PLCopyBezier*)cb
{
    [self saveUndoState];
    curve->beginCommitGrouping();
    curve->setBezierCurves(cb->curves);
    curve->setAnchorPoints(cb->anchors);
    curve->endCommitGrouping();
}

-(void)saveUndoState
{
    PLCopyBezier * cb = [[[PLCopyBezier alloc] init] autorelease];
    cb->curves = curve->bezierCurves();
    cb->anchors = curve->anchorPoints();
    [(PLBezier*)[undoManager prepareWithInvocationTarget:self] applyUndoState:cb];
}

-(void)dealloc
{
    fromWithin = YES;
    for (set<PLBezierView*>::iterator i = actors.begin(); i!= actors.end(); i++)
        (*i)->setModel(NULL);
    fromWithin = NO;
    if (curve)
        curve->release();
    if (delegate)
        delegate->release();
    [undoManager release];
    [super dealloc];
}

@end
