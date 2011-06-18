//
//  PorkholtViewController.m
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import "PorkholtViewController.h"
#import "PHMainEvents.h"
#import "PHTouchInterface.h"
#import "PHMain.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#import "EAGLView.h"

@interface PorkholtViewController ()
@property (nonatomic, retain) EAGLContext *context;
/*- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;*/
@end 

@implementation PorkholtViewController

@synthesize animating, context;

class PHRenderThread : public PHObject
{
public:
	PHMutex * pauseMutex;
	EAGLView * view;
	volatile bool running;
	int fps;
	void renderingThread()
	{
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        [view initMain];
		[view setFramebuffer];
		PHMainEvents * mainClass = PHMainEvents::sharedInstance();
		
		mainClass->init([UIScreen mainScreen].bounds.size.height, [UIScreen mainScreen].bounds.size.width,fps);
		
		double frameInterval = 1.0f/fps;
		
		double lastTime = 0;
		double time = 0;
		pauseMutex->lock();
		double targetTime = PHTime::getTime();
		while (running)
		{
			pauseMutex->unlock();
			targetTime+= frameInterval;
			
			[PHTouchInterfaceSingleton processQueue];
            mainClass->processInput();
			[view setFramebuffer];
			
			lastTime = time;
			time = PHTime::getTime();
			double elapsedTime = time-lastTime;
		//	PHLog("elapsedTime: %f",elapsedTime/frameInterval);
			if (elapsedTime>1.5*frameInterval)
				elapsedTime = 1.5*frameInterval;
			mainClass->renderFrame(elapsedTime);
			
			if (![view presentFramebuffer])
				PHLog("ERROR: Couldn't swap buffers");
			if (!mainClass->independentTiming())
			{
				double currentTime = PHTime::getTime();
				double sleepTime = targetTime-currentTime;
				if (sleepTime>0)
					PHTime::sleep(sleepTime);
				else
					targetTime = currentTime;
			}
            [pool release];
            pool = [[NSAutoreleasePool alloc] init];
			pauseMutex->lock();
		}
		pauseMutex->unlock();	
        [pool release];
	}
} renderThread;

- (NSString *) platform
{
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
	free(machine);
	return platform;
}

- (bool) dumbDevice
{
	NSString * platform = [self platform];
	PHLog("Running on %s",[platform UTF8String]);
	if ([platform isEqual:@"iPhone1,1"]|| //iPhone 1G
		[platform isEqual:@"iPhone1,2"]|| //iPhone 3G
		[platform isEqual:@"iPod1,1"]||   //iPod Touch 1G
		[platform isEqual:@"iPod2,1"])    //iPod Touch 2G
		return true;
	return false;
}

- (void)loadView
{
	EAGLView * view = [[EAGLView alloc] initWithFrame:[UIScreen mainScreen].bounds];
	
    PHTouchInterface * touchView = [[PHTouchInterface alloc] initWithFrame:view.bounds];
	touchView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
	[view addSubview:touchView];
	[touchView release];
	
	self.view = view;
	
	animating = FALSE;
	
	PHThread::mainThread();
	
	pausemutex = new PHMutex;
	pausemutex->lock();
	thread = new PHThread;
	thread->setFunction(&renderThread, (PHCallback)&PHRenderThread::renderingThread, NULL);
	renderThread.view = view;
	renderThread.pauseMutex = pausemutex;
	renderThread.running = true;
	renderThread.fps = 60;
	if ([self dumbDevice])
		renderThread.fps = 40;
	thread->start();
}

- (void)dealloc
{
	renderThread.running = false;
	thread->join();
	pausemutex->release();
	thread->release();
	[super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    [super viewWillDisappear:animated];
}

- (void)startAnimation
{
    if (!animating)
    {
        animating = TRUE;
		pausemutex->unlock();
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        animating = FALSE;
		pausemutex->lock();
    }
}

- (void)didReceiveMemoryWarning
{
	PHMainEvents::sharedInstance()->memoryWarning();
    [super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

/*

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Attach vertex shader to program.
    glAttachShader(program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_COLOR, "color");
    
    // Link program.
    if (![self linkProgram:program])
    {
        NSLog(@"Failed to link program: %d", program);
        
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }
        
        return FALSE;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}
*/

@end
