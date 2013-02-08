#ifndef LPPHVIEWCONTROLLER_H
#define LPPHVIEWCONTROLLER_H

#import <GLKit/GLKit.h>

class PHGameManager;

@interface LPPHViewController : GLKViewController
{
    EAGLContext * _context;
    NSBundle * _bundle;
    BOOL snap;

    NSTimer * idleTimer;
    unsigned int fpsStage;
    BOOL _showing;
    NSDictionary * _init;
    
    PHGameManager * _gm;
    void (*_entryPoint)(PHGameManager*);
    void * _ud;
    int _flags;
}
@property (nonatomic, readonly) EAGLContext * context;
@property (nonatomic, retain) NSBundle * bundle;
@property (nonatomic, readonly) NSTimeInterval adjustedTimeSinceLastUpdate;


//to override
-(void)setupGL;
-(void)tearDownGL;
-(void)update;
-(void)render;
-(void)reshape;

-(id)initWithFlags:(int)flags entryPoint:(void(*)(PHGameManager*))ep userData:(void*)ud initDictionary:(NSDictionary*)init;

-(void)setCurrentContext;
-(void)resetIdleTimer;

@end

#endif
