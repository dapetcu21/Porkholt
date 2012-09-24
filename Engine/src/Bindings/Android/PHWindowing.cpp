/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <EGL/egl.h>
#include <android/sensor.h>
#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>
#include "android_native_app_glue.h"

class PHWVideoMode PHW_VideoMode;
int PHWFlags;
void (*PHWEntryPoint)(PHGameManager*);
void * PHWUD;

int PHWMain(PHMAIN_PROTO, const PHWVideoMode & vmode, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHW_VideoMode = vmode;
    PHWFlags = flags;
    PHWEntryPoint = entryPoint;
    PHWUD = ud;
    android_ANativeActivity_onCreate((ANativeActivity*)activity, savedState, savedStateSize);
    return 0;
}

struct PHWState {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    int32_t width;
    int32_t height;
    ph_float fps; 

    PHGameManager * gm;
};

void PHWInitWindow(PHWState & state)
{
    EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT | ((PHWFlags & PHWGLES1) ? EGL_OPENGL_ES_BIT : 0),
            EGL_STENCIL_SIZE, (PHWFlags & PHWStencilBuffer) ? 1 : 0,
            EGL_DEPTH_SIZE, (PHWFlags & PHWDepthBuffer) ? 1 : 0,
            EGL_NONE
    };

    EGLint w, h, fps, format;
    EGLint numConfigs;
    EGLConfig * config = NULL;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, NULL, 64, &numConfigs);
    if (!numConfigs)
    {
        PHLog("Unable to get a matching OpenGL configuration");
        return;
    }
    EGLConfig * configs = new EGLConfig[numConfigs];
    eglChooseConfig(display, attribs, configs, numConfigs, &numConfigs);
    int mscore = 0;
    for (EGLint i = 0; i < numConfigs; i++)
    {
        EGLint depth, stencil, gl;
        EGLConfig & c = *(configs + i);
        eglGetConfigAttrib(display, c, EGL_DEPTH_SIZE, &depth);
        eglGetConfigAttrib(display, c, EGL_STENCIL_SIZE, &stencil);
        eglGetConfigAttrib(display, c, EGL_RENDERABLE_TYPE, &gl);
        if (!(PHWFlags & PHWDepthBuffer))
            depth = -depth;
        if (!(PHWFlags & PHWStencilBuffer))
            stencil = -stencil;
        int score = ((gl & EGL_OPENGL_ES2_BIT)?10000:0) + depth*100 + stencil;
        PHLog("config: %d", score);
        if (score > mscore)
        {
            mscore = score;
            config = configs + i;
        }
    }
    PHLog("score %d %d", mscore, (int)(config - configs));

    EGLint gl;
    eglGetConfigAttrib(display, *config, EGL_NATIVE_VISUAL_ID, &format);
    eglGetConfigAttrib(display, *config, EGL_RENDERABLE_TYPE, &gl);
    bool gles2 = (gl & EGL_OPENGL_ES2_BIT) != 0;
    PHGL::linkedLibrary = gles2 ? PHGL::libOpenGLES2 : PHGL::libOpenGLES1;
    PHLog("meow %d", PHGL::linkedLibrary);

    ANativeWindow_setBuffersGeometry(state.app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, *config, state.app->window, NULL);
    const EGLint attrib_list [] = {EGL_CONTEXT_CLIENT_VERSION, gles2?2:1, EGL_NONE};
    context = eglCreateContext(display, *config, NULL, attrib_list);

    delete [] configs;

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        PHLog("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    fps = 60.0f; //FUCK YOU GOOGLE

    state.display = display;
    state.context = context;
    state.surface = surface;
    state.width = w;
    state.height = h;
    state.fps = fps;

    PHGameManager * gameManager = state.gm = new PHGameManager;
    PHGameManagerInitParameters params;
    params.screenHeight = w; 
    params.screenWidth = h; 
    params.fps = fps;
    params.dpi = 160; //FUCK YOU GOOGLE
    PHDirectory * dir = PHInode::directoryAtFSPath("/sdcard/porkholt/rsrc"); //FUCK YOU GOOGLE
    params.setResourceDirectory(dir);
    dir->release();
    params.entryPoint = PHWEntryPoint;
    if (PHWFlags & PHWShowFPS)
        gameManager->setShowsFPS(true);
    if (PHWFlags & PHWRemote)
        gameManager->setUsesRemote(true);
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    gameManager->setFpsCapped(true);
    gameManager->setUserData(PHWUD);
    gameManager->init(params);
}

void PHWDestroyWindow(PHWState & state)
{
    if (state.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (state.context != EGL_NO_CONTEXT) {
            eglDestroyContext(state.display, state.context);
        }
        if (state.surface != EGL_NO_SURFACE) {
            eglDestroySurface(state.display, state.surface);
        }
        eglTerminate(state.display);
    }
    state.animating = true;
    state.display = EGL_NO_DISPLAY;
    state.context = EGL_NO_CONTEXT;
    state.surface = EGL_NO_SURFACE;
}

void PHWRender(PHWState & state)
{
    if (state.display == EGL_NO_DISPLAY) return;
    if (!state.gm) return;

    eglMakeCurrent(state.display, state.surface, state.surface, state.context);
    state.gm->renderFrame(1.0f/state.fps); 
    eglSwapBuffers(state.display, state.surface);
}

void PHWOnCmd(struct android_app* app, int32_t cmd)
{
    PHWState & state = *(PHWState*)(app->userData);
    switch(cmd)
    {
        case APP_CMD_INIT_WINDOW:
            PHWInitWindow(state);
            PHWRender(state);
            break;
        case APP_CMD_TERM_WINDOW:
            PHWDestroyWindow(state);
            break;
        case APP_CMD_GAINED_FOCUS:
            if (state.accelerometerSensor)
            {
                ASensorEventQueue_enableSensor(state.sensorEventQueue, state.accelerometerSensor);
                if (state.gm)
                    ASensorEventQueue_setEventRate(state.sensorEventQueue, state.accelerometerSensor, state.fps * 1000000L);
            }
            state.animating = true;
            if (state.gm)
                state.gm->appResumed();
            break;
        case APP_CMD_LOST_FOCUS:
            if (state.accelerometerSensor)
                ASensorEventQueue_disableSensor(state.sensorEventQueue, state.accelerometerSensor);
            PHWRender(state);
            state.animating = false;
            if (state.gm)
                state.gm->appSuspended();
            break;
    }
}

int32_t PHWOnInputEvent(struct android_app* app, AInputEvent * evt)
{
    PHWState & state = *(PHWState*)(app->userData);
    return 0;
}

void android_main(android_app * app)
{
    app_dummy();

    PHWState state;
    memset(&state, 0, sizeof(PHWState));


    state.app = app;
    app->userData = &state;
    app->onAppCmd = PHWOnCmd;
    app->onInputEvent = PHWOnInputEvent;

    state.sensorManager = ASensorManager_getInstance();
    state.accelerometerSensor = ASensorManager_getDefaultSensor(state.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    state.sensorEventQueue = ASensorManager_createEventQueue(state.sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);

    while(1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(state.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) {
                source->process(app, source);
            }

            if (ident == LOOPER_ID_USER) {
                if (state.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(state.sensorEventQueue,
                            &event, 1) > 0) {
                        PHLog("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                    }
                }
            }
        }

        if (state.animating)
            PHWRender(state);
    }
}
