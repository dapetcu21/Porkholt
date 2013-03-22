#ifndef LPPHVIEWCONTROLLER_H
#define LPPHVIEWCONTROLLER_H

#import <GLKit/GLKit.h>

class PHGameManager;
#ifndef PH_TOKENPASTE
#define _PH_TOKENPASTE(x, y) x ## y
#define PH_TOKENPASTE(x, y) _PH_TOKENPASTE(x, y)
#endif

#define LPPHViewController PH_TOKENPASTE(LPPHViewController, PH_APP_TARGET)

class PHNormalImage;
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
    
    CGRect _wallpaperRect;
    PHNormalImage * _wallpaperImage;

    int _variant;
}
@property (nonatomic, readonly) EAGLContext * context;
@property (nonatomic, retain) NSBundle * bundle;
@property (nonatomic, readonly) NSTimeInterval adjustedTimeSinceLastUpdate;
@property (nonatomic, assign) CGRect wallpaperRect;
@property (nonatomic, assign) int variant;


//to override
-(void)setupGL;
-(void)tearDownGL;
-(void)update;
-(void)render;
-(void)reshape;

-(id)initWithFlags:(int)flags entryPoint:(void(*)(PHGameManager*))ep userData:(void*)ud initDictionary:(NSDictionary*)init;

-(void)setCurrentContext;
-(void)resetIdleTimer;

-(void)setWallpaperImage:(UIImage*)img;
-(PHNormalImage*)wallpaperImage;

@end

#endif
