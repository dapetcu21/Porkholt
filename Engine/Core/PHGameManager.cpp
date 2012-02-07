/*
 *  PHGameManager.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHGameManager.h"
#ifdef PH_SIMULATOR
#include "PHRemote.h"
#endif
#include "PHView.h"
#include "PHFileManager.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHNavigationController.h"
#include "PHEventHandler.h"
#include "PHSoundManager.h"
#include "PHGLShaderProgram.h"
#include "PHGLUniformStates.h"

PHGameManager::PHGameManager() : view(NULL), viewController(NULL), loaded(false), openGLStates(0), parsedExtensions(false), openGLVersionMajor(0), openGLVersionMinor(0), _shader(NULL), _spriteShader(NULL), _coloredSpriteShader(NULL), spriteStates(NULL), _noTexSpriteShader(NULL), _coloredNoTexSpriteShader(NULL), _textShader(NULL)
{}

PHGameManager::~PHGameManager()
{
    if (evtHandler)
        evtHandler->release();
    if (sndManager)
        sndManager->release();
    if (spriteStates)
        spriteStates->release();
    if (hd)
        --globalHD;
}

int PHGameManager::globalHD = 0;

void PHGameManager::updateHD()
{
    ph_float x = _screenWidth;
    ph_float y = _screenHeight;
    ph_float diagsq = x*x+y*y;
    bool newhd = (diagsq > 500000);
    if (newhd && !hd)
        ++globalHD;
    if (!newhd && hd)
        --globalHD;
    hd = newhd;
}

void PHGameManager::init(const PHGameManagerInitParameters & params)
{
	fps = params.fps;
	_screenWidth = params.screenWidth;
	_screenHeight = params.screenHeight;
    resPath = params.resourcePath;
    dpi = params.dpi;
	suspended = 0;
    loaded = true;
    entryPoint = params.entryPoint;
	setUserData(ud);
    
    hd = false;
    updateHD();
    
	PHThread::mainThread();

#ifdef PH_SIMULATOR
    remote = new PHRemote(this);
    remote->start();
#endif
	
    evtHandler = new PHEventHandler(this);
#ifdef PH_IPHONE_OS
    sndManager = new PHSoundManager(resPath + "/snd/fx");
#endif
    
    loadCapabilities();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!useShaders())
    {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    setProjection();
    
    if (useShaders())
    {
        pushSpriteShader(_spriteShader = shaderProgramNamed("sprites"));
        _coloredSpriteShader = shaderProgramNamed("color_sprites");
        _noTexSpriteShader = shaderProgramNamed("notex_sprites");
        _coloredNoTexSpriteShader = shaderProgramNamed("color_notex_sprites");
        _textShader = shaderProgramNamed("text");
        spriteStates = new PHGLUniformStates;
        spriteStates->insert("modelViewProjectionMatrix", modelViewSpriteUniform);
        spriteStates->insert("color", colorSpriteUniform);
        spriteStates->insert("texture",textureSpriteUniform) = 0;
    }
    
    view = new PHView(PHRect(0,0,_screenWidth,_screenHeight));
    view->setGameManager(this);
	view->setBackgroundColor(PHGrayColor);
	view->setUserInput(true);
    
    viewController = new PHNavigationController();
	viewController->init(this);
	viewController->_viewWillAppear();
	view->addSubview(viewController->getView());
	viewController->_viewDidAppear();

    entryPoint(this);
    
    view->setGameManager(this);
}

void PHGameManager::setModelViewMatrix(const PHMatrix & m)
{
    if (!useShaders())
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(m.floats());
    }
    _modelView = m;
}

void PHGameManager::setProjectionMatrix(const PHMatrix & m)
{
    if (!useShaders())
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(m.floats());
    }
    _projection = m;
}

void PHGameManager::setProjection()
{
    glViewport(0, 0, _screenWidth, _screenHeight);
    PHMatrix m;
    m.loadTranslation(-1,-1);
    m.scale(2.0f/(_screenWidth), 2.0f/(_screenHeight));
    setProjectionMatrix(m);
}

void PHGameManager::setScreenSize(ph_float w, ph_float h)
{
    _screenWidth = w;
    _screenHeight = h;
    updateHD();
    setProjection(); 
    if (view)
        view->setFrame(PHRect(0,0,_screenWidth,_screenHeight));
}

void PHGameManager::processInput()
{
#ifdef PH_SIMULATOR
    remote->processPendingPackets();
#endif
}

void PHGameManager::globalFrame(ph_float timeElapsed)
{
    PHAnimatorPool::mainAnimatorPool()->advanceAnimation(timeElapsed);
    PHThread::mainThread()->processQueue();
}

void PHGameManager::renderFrame(ph_float timeElapsed)
{	
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
    setModelViewMatrix(PHIdentityMatrix);
    
	if (viewController)  
		viewController->_updateScene(timeElapsed);
    
	view->render();
}

void PHGameManager::setOpenGLStates(uint32_t states)
{
    if (states & PHGLResetAllStates)
    {
        states = 0;
        openGLStates = 0xffffffff;
    }
    
    int xr = states^openGLStates;
    openGLStates = states;
    
    if (xr & PHGLVertexArray)
    {
        if (useShaders())
        {
            if (states & PHGLVertexArray)
                glEnableVertexAttribArray(PHIMAGEATTRIBUTE_POS);
            else
                glDisableVertexAttribArray(PHIMAGEATTRIBUTE_POS);            
        } else {
            if (states & PHGLVertexArray)
                glEnableClientState(GL_VERTEX_ARRAY);
            else
                glDisableClientState(GL_VERTEX_ARRAY);
        }
    }
    
    
    if (xr & PHGLColorArray)
    {
        if (useShaders())
        {
            if (states & PHGLVertexArray)
                glEnableVertexAttribArray(PHIMAGEATTRIBUTE_CLR);
            else
                glDisableVertexAttribArray(PHIMAGEATTRIBUTE_CLR);            
        } else {
            if (states & PHGLColorArray)
                glEnableClientState(GL_COLOR_ARRAY);
            else
                glDisableClientState(GL_COLOR_ARRAY);
        }
    }
    
    if (xr & PHGLTextureCoordArray)
    {
        if (useShaders())
        {
            if (states & PHGLVertexArray)
                glEnableVertexAttribArray(PHIMAGEATTRIBUTE_TXC);
            else
                glDisableVertexAttribArray(PHIMAGEATTRIBUTE_TXC);            
        } else {
            if (states & PHGLTextureCoordArray)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            else
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
    
    if (xr & PHGLTexture)
    {
        if (states & PHGLTexture)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);
    }
}

void PHGameManager::_appSuspended(PHObject * sender, void * ud)
{
	if (viewController)
	{
		viewController->_viewWillDisappear();
		viewController->_viewDidDisappear();
	}
}

void PHGameManager::_appResumed(PHObject * sender, void * ud)
{
    if (viewController)
	{
		viewController->_viewWillAppear();
		viewController->_viewDidAppear();
	}
}

void PHGameManager::appSuspended()
{
    if (!loaded) return;
    if (!loaded) return;
	if (suspended) return;
	suspended = true;
	PHMessage::messageWithName("appSuspended")->broadcast(this, NULL);
    PHLog("appSuspended");
#ifdef PH_SIMULATOR
    remote->stop();
#endif
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appSuspended, NULL), false);
}

void PHGameManager::appResumed()
{
    if (!loaded) return;
    if (!suspended) return;
	suspended = false;
	PHLog("appResumed");
	PHMessage::messageWithName("appResumed")->broadcast(this, NULL);
#ifdef PH_SIMULATOR
    try {
        remote->start();
    } catch (string err) {
        PHLog("URemote: %s",err.c_str());
    }
#endif
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appResumed, NULL), false);
}


void PHGameManager::appQuits()
{
    if (!loaded) return;
	//This isn't guaranteed to be called
	//Save all stuff in PHGameManager::appSuspended()
	PHLog("appQuits");
#ifdef PH_SIMULATOR
    delete remote;
#endif
}

void PHGameManager::memoryWarning()
{
	PHLog("memoryWarning");
}

int PHGameManager::interfaceType()
{
    if (isGloballyHD())        
        return interfaceHD;
    return interfaceSD;
}

const string PHGameManager::imageDirectory()
{
    return resourcePath() + "/img/";
}

const string PHGameManager::fontDirectory()
{
    return resourcePath() + "/fnt/";
}

const string PHGameManager::musicNamed(const string & name)
{
    return resourcePath() + "/snd/music/"+name+".mp3";
}

const string PHGameManager::shaderDirectory()
{
    return resourcePath() + "/shaders/";
}

void PHGameManager::loadCapabilities()
{
    if (!parsedExtensions)
    {
        const char * ver = (char*) glGetString(GL_VERSION);
        bool es = strstr(ver, "ES");
        openGLCaps[PHGLCapabilityOpenGLES] = es;
        
        int number = 0;
        bool major = true;
        while (*ver)
        {
            if ((*ver)=='.')
            {
                if (major)
                {
                    openGLVersionMajor = number;
                    major = false;
                    number = 0;
                }
                else 
                    break;
            } else
                if (('0'<=(*ver)) && ((*ver)<='9'))
                    number=number*10+*ver-'0';
                else
                    if (number || !major)
                        break;
            ver++;
        }
        openGLVersionMinor = number;
        
#ifdef GL_NUM_EXTENSIONS
        if (!es && openGLVersionMajor>=3)
        {
            int n = (int)glGetInteger(GL_NUM_EXTENSIONS);
            for (int i=0; i<n; i++)
                extensions.insert(string((const char*)glGetStringi(GL_EXTENSIONS, i)));
        } else
#endif
        {
            const char * ext = (char*) glGetString(GL_EXTENSIONS);
            while (ext && ext[0])
            {
                const char * pos = strpbrk(ext, " ");
                if (!pos)
                    pos = ext+strlen(ext);
                extensions.insert(string(ext,pos-ext));
                if (!pos[0])
                    ext = pos;
                else
                    ext = pos+1;
            }
        }
        parsedExtensions = true;
        
        string s;
        for (set<string>::iterator i = extensions.begin(); i!=extensions.end(); i++)
        {
            s+=*i;
            s+=" ";
        }
        PHLog("OpenGL Version: %s %d.%d",es?"ES":"",openGLVersionMajor,openGLVersionMinor);
        PHLog("OpenGL Extensions: %s",s.c_str());
        
        openGLCaps[PHGLCapabilityNPOT] = (openGLVersionMajor>=2 || extensions.count("OES_texture_npot") || extensions.count("GL_ARB_texture_non_power_of_two") || extensions.count("GL_IMG_texture_npot"));
        openGLCaps[PHGLCapabilityAppleLimitedNPOT] =
            (extensions.count("GL_APPLE_texture_2D_limited_npot") && !extensions.count("OES_texture_npot")) 
#ifdef PH_IPHONE_OS
            || (es && (openGLVersionMajor==2));
#endif
        ;
        openGLCaps[PHGLCapabilityGLES1] = es && (openGLVersionMajor == 1);
        openGLCaps[PHGLCapabilityGLES2] = es && (openGLVersionMajor >= 2);
        openGLCaps[PHGLCapabilityShaders] = (openGLVersionMajor>=2);
    }
}

void PHGameManager::pushSpriteShader(PHGLShaderProgram * p) 
{ 
    if (!p) return;
    spriteShaderStack.push_back(p);
    p->retain(); 
}
void PHGameManager::popSpriteShader() 
{
    if (!spriteShaderStack.empty())
    {
        spriteShaderStack.back()->release();
        spriteShaderStack.pop_back();
    }
}

void PHGameManager::reapplyMatrixUniform()
{
    if (!openGLCaps[PHGLCapabilityShaders]) return;
    PHGLShaderProgram * shader = spriteShader();
    if (!shader) return;
    (spriteStates->at(modelViewSpriteUniform) = _projection * _modelView).apply(shader);
}

void PHGameManager::reapplyColorUniform()
{
    if (!openGLCaps[PHGLCapabilityShaders]) return;
    PHGLShaderProgram * shader = spriteShader();
    if (!shader) return;
    (spriteStates->at(colorSpriteUniform) = _currentColor).apply(shader);
}

void PHGameManager::applySpriteShader()
{
    if (useShaders())
    {
        PHGLShaderProgram * shader = spriteShader();
        if (!shader) return;
        (*spriteStates)[modelViewSpriteUniform] = _projection * _modelView;
        (*spriteStates)[colorSpriteUniform] = _currentColor;
        useShader(shader);
        spriteStates->apply(shader);
    }
}

void PHGameManager::useShader(PHGLShaderProgram * prog)
{
    if (_shader==prog)
        return;
    _shader = prog;
    prog->use();
}