//
//  PLPorkholtView.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLPorkholtView.h"
#import "PHGameManager.h"

#import "PHTestViewController.h"
#import "PHNavigationController.h"
#import "OpenGLTimer.h"

@implementation PLPorkholtView

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(PHGameManager*)gameManager
{
    return gameManager;
}

-(void)setNeedsDisplayYes
{
    [self setNeedsDisplay:YES];
}

-(void)awakeFromNib
{
    [[self openGLContext] makeCurrentContext];

    NSRect frame = [self bounds];
    gameManager = new PHGameManager;
    gameManager->setUserData(self);
    gameManager->init(frame.size.width, frame.size.height, 60);
    
    [[OpenGLTimer retainedInstance] addView:self];
}

-(void)dealloc
{
    gameManager->release();
    [[OpenGLTimer sharedInstance] removeView:self];
    [[OpenGLTimer sharedInstance] release];
    [super dealloc];
}

-(void)drawRect:(NSRect)dirtyRect
{
    gameManager->processInput();
    
    static double time = 0;
    static double lastTime = 0;
    
    double frameInterval = 1.0f/gameManager->framesPerSecond();
    lastTime = time;
    time = PHTime::getTime();
    double elapsedTime = time-lastTime;
    if (elapsedTime>1.5*frameInterval)
        elapsedTime = 1.5*frameInterval;
    gameManager->renderFrame(elapsedTime);
    glFlush();
}

-(void)reshape
{
    [super reshape];
    NSRect frame = [self bounds];
    if (gameManager)
        gameManager->setScreenSize(frame.size.width, frame.size.height);
}

-(void)entryPoint
{
    PHViewController * vc = new PHTestViewController();
    vc->init(gameManager);
    gameManager->navigationController()->pushViewController(vc, PHNavigationController::SlideLeft, false);
    vc->release();
}

void PHGameManager::entryPoint()
{
    [(PLPorkholtView*)(this->userData()) entryPoint];
}

@end
