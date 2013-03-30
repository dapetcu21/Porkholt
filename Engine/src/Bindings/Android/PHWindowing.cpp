/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <EGL/egl.h>
#include <android/sensor.h>
#include "android_native_app_glue.h"
#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHLinkDirectory.h>
#include <Porkholt/Core/PHAccelInterface.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

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
    float fps; 
    float dpi;
    int orient;
    bool vsync;

    PHGameManager * gm;

    JNIEnv *jni;
    jobject myInstance;
    jclass myClass;
};

void PHWLoadFromJava(PHWState & state)
{
    JNIEnv *jni = state.jni;
    if (!state.jni)
    {
        state.app->activity->vm->AttachCurrentThread(&state.jni, NULL);
        jni = state.jni; 
        jclass activityClass = jni->FindClass("android/app/NativeActivity");
        jmethodID getClassLoader = jni->GetMethodID(activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = jni->CallObjectMethod(state.app->activity->clazz, getClassLoader);
        jclass classLoader = jni->FindClass("java/lang/ClassLoader");
        jmethodID findClass = jni->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring strClassName = jni->NewStringUTF("org/porkholt/jniloader/JNILoader");
        
        state.myClass = (jclass)jni->CallObjectMethod(cls, findClass, strClassName);
        jmethodID constructor = jni->GetMethodID(state.myClass, "<init>", "(Landroid/app/NativeActivity;)V");
        state.myInstance = jni->NewObject(state.myClass, constructor, state.app->activity->clazz);
    }

    jmethodID load = jni->GetMethodID(state.myClass, "load", "()V");
    jni->CallVoidMethod(state.myInstance, load);

    jfieldID rrid = jni->GetFieldID(state.myClass, "refreshRate", "F");
    jfieldID dpiid = jni->GetFieldID(state.myClass, "dpi", "F");
    jfieldID oriid = jni->GetFieldID(state.myClass, "orientation", "I");

    state.fps = jni->GetFloatField(state.myInstance, rrid);
    state.dpi = jni->GetFloatField(state.myInstance, dpiid);
    state.orient = jni->GetIntField(state.myInstance, oriid);
}


void PHWInitWindow(PHWState & state)
{
    EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT | ((PHWFlags & PHWGLES1) ? EGL_OPENGL_ES_BIT : 0),
            EGL_STENCIL_SIZE, (PHWFlags & PHWStencilBuffer) ? 1 : 0,
            EGL_DEPTH_SIZE, (PHWFlags & PHWDepthBuffer) ? 1 : 0,
            EGL_NONE
    };

    EGLint w, h, format;
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
        EGLint depth, stencil, gl, smax;
        EGLConfig & c = *(configs + i);
        eglGetConfigAttrib(display, c, EGL_DEPTH_SIZE, &depth);
        eglGetConfigAttrib(display, c, EGL_STENCIL_SIZE, &stencil);
        eglGetConfigAttrib(display, c, EGL_RENDERABLE_TYPE, &gl);
        eglGetConfigAttrib(display, c, EGL_MAX_SWAP_INTERVAL, &smax);

        if (!(PHWFlags & PHWDepthBuffer))
            depth = 1<<5-1-depth;
        if (!(PHWFlags & PHWStencilBuffer))
            stencil = 1<<5-1-stencil;
        int score = ((gl & EGL_OPENGL_ES2_BIT)?1<<11:0) + ((smax>=1)?(1<<10):0) + (depth<<5) + stencil;

        //PHLog("config: 0x%x", score);

        if (score > mscore)
        {
            mscore = score;
            config = configs + i;
        }
    }
    state.vsync = ((mscore & (1<<10)) != 0);

    EGLint gl, smin;
    eglGetConfigAttrib(display, *config, EGL_NATIVE_VISUAL_ID, &format);
    eglGetConfigAttrib(display, *config, EGL_RENDERABLE_TYPE, &gl);
    eglGetConfigAttrib(display, *config, EGL_MIN_SWAP_INTERVAL, &smin);

    bool gles2 = (gl & EGL_OPENGL_ES2_BIT) != 0;
    PHGL::linkedLibrary = gles2 ? PHGL::libOpenGLES2 : PHGL::libOpenGLES1;

    ANativeWindow_setBuffersGeometry(state.app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, *config, state.app->window, NULL);
    const EGLint contextAttribs [] = {EGL_CONTEXT_CLIENT_VERSION, gles2?2:1, EGL_NONE};
    context = eglCreateContext(display, *config, NULL, contextAttribs);
    eglSwapInterval(display, state.vsync?max(smin,1):0);

    delete [] configs;

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        PHLog("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    state.display = display;
    state.context = context;
    state.surface = surface;
    state.width = w;
    state.height = h;

    PHAutoreleasePool ap;
    PHGameManager * gameManager = state.gm = new PHGameManager;
    PHGameManagerInitParameters params;
    params.screenWidth = w; 
    params.screenHeight = h; 
    params.fps = state.fps;
    params.dpi = state.dpi; 
    PHDirectory * dir = NULL;
    try { 
        dir = new PHLinkDirectory(PHInode::directoryAtFSPath("/sdcard/porkholt/rsrc")); // FUCK YOU GOOGLE
        dir->autorelease();
    } 
    catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    params.setResourceDirectory(dir);
    params.entryPoint = PHWEntryPoint;
    PHWApplyFlags(gameManager, PHWFlags); 
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    gameManager->setUserData(PHWUD);


#ifdef PH_DEBUG
    PHLog("Loaded window %dx%d@%.1f, vsync: %s, orientation: %d degrees", w, h, state.fps, state.vsync?"on":"off", (int)state.orient * 90);
#endif
    gameManager->init(params);
}

void PHWDestroyWindow(PHWState & state)
{
    if (state.gm)
    {
        state.gm->release();
        state.gm = NULL;
    }

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
    state.animating = false;
    state.display = EGL_NO_DISPLAY;
    state.context = EGL_NO_CONTEXT;
    state.surface = EGL_NO_SURFACE;
}

void PHWRender(PHWState & state)
{
    if (state.display == EGL_NO_DISPLAY) return;
    if (!state.gm) return;
    
    eglMakeCurrent(state.display, state.surface, state.surface, state.context);

    state.gm->renderFrame();
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
                ASensorEventQueue_setEventRate(state.sensorEventQueue, state.accelerometerSensor, 1000000L / state.fps);
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

void PHWOnTouchEvent(PHWState & state, int type, int pointer, float x, float y)
{
    if (!state.gm) return;
    void * ud = (void*)pointer;
    PHPoint p(x/state.width  *  2 - 1, 
              y/state.height * -2 + 1);
    switch(type)
    {
        case AMOTION_EVENT_ACTION_DOWN:
            state.gm->eventHandler()->touchDown(p, ud);
            break;
        case AMOTION_EVENT_ACTION_UP:
            state.gm->eventHandler()->touchUp(p, ud);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            state.gm->eventHandler()->touchMoved(p, ud);
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            state.gm->eventHandler()->touchCancelled(p, ud);
            break;
    }
}

int32_t PHWOnInputEvent(struct android_app* app, AInputEvent * evt)
{
    PHWState & state = *(PHWState*)(app->userData);
    if (AInputEvent_getType(evt) == AINPUT_EVENT_TYPE_MOTION) {
    	int type;
        int action = AMotionEvent_getAction(evt);
        int pointer = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int pc;
        
        switch(action & AMOTION_EVENT_ACTION_MASK)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                type = AMOTION_EVENT_ACTION_DOWN;
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_OUTSIDE:
                type = AMOTION_EVENT_ACTION_UP;
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                type = AMOTION_EVENT_ACTION_MOVE;
                pc = AMotionEvent_getPointerCount(evt);
                for (int p=0; p<pc; p++)
                    PHWOnTouchEvent(state, type, AMotionEvent_getPointerId(evt, p), AMotionEvent_getX(evt, p), AMotionEvent_getY(evt, p));
                return true;
            case AMOTION_EVENT_ACTION_CANCEL:
                type = AMOTION_EVENT_ACTION_CANCEL;
                break;
            default:
                return 0;
        }
        PHWOnTouchEvent(state, type, AMotionEvent_getPointerId(evt, pointer), AMotionEvent_getX(evt, pointer), AMotionEvent_getY(evt, pointer));
		return 1;
    }
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

    PHWLoadFromJava(state);

    while(1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(state.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) {
                source->process(app, source);
            }

            if (ident == LOOPER_ID_USER) 
            {
                if (state.accelerometerSensor != NULL) 
                {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(state.sensorEventQueue,
                            &event, 1) > 0) 
                    {
                        PHAcceleration accel;
                        accel.x = event.acceleration.x;
                        accel.y = event.acceleration.y;
                        accel.z = event.acceleration.z;
                        float x=accel.x, y=accel.y;
                        switch(state.orient)
                        {
                            case 0:
                                accel.x = -x;
                                accel.y = -y;
                            case 1:
                                accel.x = y;
                                accel.y = -x;
                                break;
                            case 2:
                                accel.x = x;
                                accel.y = -y;
                                break;
                            case 3:
                                accel.x = -y;
                                accel.y = x;
                                break;
                        }
                        PHAccelInterface::setAcceleration(accel);
                    }
                }
            }
        }

        if (state.animating)
            PHWRender(state);
    }
}
