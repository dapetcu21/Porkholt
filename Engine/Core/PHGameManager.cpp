/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHGameManager.h"
#include "PHRemote.h"
#include "PHView.h"
#include "PHFileManager.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHNavigationController.h"
#include "PHEventHandler.h"
#include "PHSoundManager.h"
#include "PHGLShaderProgram.h"
#include "PHGLUniformStates.h"
#include "PHGLVertexBufferObject.h"
#include "PHGLVertexArrayObject.h"
#include "PHEventQueue.h"
#include "PHTextView.h"
#include "PHFont.h"
#include "PHGLTexture.h"

//#define PH_FORCE_FAKE_VAO

PHGameManager::PHGameManager() : view(NULL), viewController(NULL), loaded(false), useRemote(false), remote(NULL), showFPS(false), fpsView(NULL), capped(false), openGLStates(0), openGLVertexAttribStates(0), parsedExtensions(false), openGLVersionMajor(0), openGLVersionMinor(0), spriteStates(NULL), _shader(NULL), _spriteShader(NULL), _coloredSpriteShader(NULL), _noTexSpriteShader(NULL), _coloredNoTexSpriteShader(NULL), _textShader(NULL), _missingNormalSpriteShader(NULL), rndMode(defaultRenderMode), _boundVAO(NULL), _solidSquareVAO(NULL), _solidSquareVBO(NULL), lgth(NULL), ambient(PHClearColor), aTMU(0)
{
    memset(boundVBOs, 0, sizeof(boundVBOs));
    memset(textures, 0, sizeof(textures));
}


PHGameManager::~PHGameManager()
{
    if (exitmsg)
    {
        exitmsg->broadcast(this, NULL);
    }
    if (view)
        view->release();
    if (fpsView)
        fpsView->release();
    if (viewController)
        viewController->release();
    if (_solidSquareVAO)
        _solidSquareVAO->release();
    if (_solidSquareVBO)
        _solidSquareVBO->release();
    if (evtHandler)
        evtHandler->release();
    if (evtQueue)
        evtQueue->release();
    if (sndManager)
        sndManager->release();
    if (spriteStates)
        spriteStates->release();
    if (lgth)
        lgth->release();
    if (remote)
        delete remote;
    if (hd)
        --globalHD;
}

void PHGameManager::setMainView(PHView * v)
{
    if (v)
    {
        v->setGameManager(this);
        v->retain();
    }
    if (view)
    {
        view->setGameManager(NULL);
        view->release();
    }
    view = v;
    setProjection(); 
    if (view)
        view->setFrame(PHRect(0,0,_screenWidth,_screenHeight));
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
    _defaultFBOf = params.defaultFBO;
    _defaultFBO = 0;
    lt = PHTime::getTime();
	setUserData(ud);
    
    hd = false;
    updateHD();
    
	PHThread::mainThread();

    if (useRemote)
    {
        try {
            remote = new PHRemote(this);
            remote->start();
        } catch (string ex) {
            PHLog("Remote Server: %s",ex.c_str());
            if (remote)
            {
                delete remote;
                remote = NULL;
            }
            useRemote = false;
        }
    }
	
    evtHandler = new PHEventHandler(this);
    evtQueue = new PHEventQueue();
    sndManager = new PHSoundManager(resPath + "/snd/fx");
    
    loadCapabilities();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT,GL_LINE); //wireframe
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
        _missingNormalSpriteShader = shaderProgramNamed("missingnormals_sprites");
        spriteStates = new PHGLUniformStates;
        spriteStates->insert("modelViewProjectionMatrix", modelViewSpriteUniform);
        spriteStates->insert("color", colorSpriteUniform);
        spriteStates->insert("texture",textureSpriteUniform) = 0;
    }
    
    view = new PHView(PHRect(0,0,_screenWidth,_screenHeight));
    view->setGameManager(this);
	view->setUserInput(true);
    view->setAutoresizesSubviews(true);
    
    viewController = new PHNavigationController();
	viewController->init(this);
	viewController->_viewWillAppear();
	view->addChild(viewController->getView());
    view->setGameManager(this);
    viewController->_viewDidAppear();
    
    entryPoint(this);
    
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
    if (useRemote)
    {
        if (!remote)
        {
            try {
                remote = new PHRemote(this);
                remote->start();
            } catch (string ex) {
                PHLog("Remote Server: %s",ex.c_str());
                if (remote)
                {
                    delete remote;
                    remote = NULL;
                }
                useRemote = false;
            }
        }
        if (remote)
        {
            remote->processPendingPackets();
        }
    }
}

void PHGameManager::globalFrame(ph_float timeElapsed)
{
    PHAnimatorPool::mainAnimatorPool()->advanceAnimation(timeElapsed);
    PHThread::mainThread()->processQueue();
}

void PHGameManager::renderFrame(ph_float timeElapsed)
{	
    lastElapsed = timeElapsed;
    if (_defaultFBOf==0)
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_defaultFBO);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef PH_MOBILE
    glClearDepthf(1.0);
#else
    glClearDepth(1.0);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
    setModelViewMatrix(PHIdentityMatrix);
    
	if (viewController)  
		viewController->_updateScene(timeElapsed);
    
	view->render();
    
    ph_float tm = PHTime::getTime();
    if (showFPS)
        renderFPS(tm - lt);
    lt = tm;
    
    evtQueue->processQueue();
    clearDeleteQueue();
}

void PHGameManager::renderFPS(ph_float timeElapsed)
{
    if (!fpsView)
    {
        fpsView = new PHTextView();
        fpsView->setGameManager(this);
        fpsView->setFont(fontNamed("Arial"));
        fpsView->setFontSize(30.0f);
        fpsView->setFontColor(PHColor(0.1f,0.9f,1.0f));
        fpsView->setAlignment(PHTextView::alignTop | PHTextView::justifyLeft);
        fpsLeft = 1.0f;
        frames = 0;
    }
    fpsView->setFrame(view->frame());
    frames++;
    fpsLeft -= timeElapsed;
    if (fpsLeft <=0)
    {
        stringstream s;
        s<<frames<<" FPS";
        fpsView->setText(s.str());
        frames = 0;
        fpsLeft += (int(-fpsLeft)+1);
    }
    fpsView->render();
}

void PHGameManager::clearDeleteQueue()
{
    if (!deleteVBOs.empty())
        glDeleteBuffers(deleteVBOs.size(), &(deleteVBOs[0]));
    if (!deleteVAOs.empty())
        PHGLDeleteVertexArrays(deleteVAOs.size(), &(deleteVAOs[0]));
    deleteVBOs.clear();
    deleteVAOs.clear();
}

void PHGameManager::setGLAttributeStates(uint32_t vertexAttrib)
{
    int xra = vertexAttrib^openGLVertexAttribStates;
    openGLVertexAttribStates = vertexAttrib;
    
    
    if (useShaders())
    {
        int max = min<int>(_maxVertexAttribs,32);
        for (int i=0; i<max; i++)
        {
            uint32_t flag = (1 << i);
            if (xra & flag)
            {
                if (vertexAttrib & flag)
                    glEnableVertexAttribArray(i);
                else
                    glDisableVertexAttribArray(i);
            }
        }
    } else {
        uint32_t mask;
        mask = (1 << PHIMAGEATTRIBUTE_POS);
        if (xra & mask )
        {
            if (vertexAttrib & mask)
                glEnableClientState(GL_VERTEX_ARRAY);
            else
                glDisableClientState(GL_VERTEX_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_CLR);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                glEnableClientState(GL_COLOR_ARRAY);
            else
                glDisableClientState(GL_COLOR_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_TXC);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            else
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_NRM);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                glEnableClientState(GL_NORMAL_ARRAY);
            else
                glDisableClientState(GL_NORMAL_ARRAY);
        }
    }
}

void PHGameManager::setGLStates(uint32_t states, uint32_t vertexAttrib)
{
    if (states & PHGLResetAllStates)
    {
        states = vertexAttrib = 0;
        openGLStates = 0xffffffff;
    }
    
    if (states & PHGLVertexArray)
    {
        vertexAttrib |= (1 << PHIMAGEATTRIBUTE_POS);
        states &= ~(uint32_t)PHGLVertexArray;
    }
    if (states & PHGLTextureCoordArray)
    {
        vertexAttrib |= (1 << PHIMAGEATTRIBUTE_TXC);
        states &= ~(uint32_t)PHGLTextureCoordArray;
    }
    if (states & PHGLColorArray)
    {
        vertexAttrib |= (1 << PHIMAGEATTRIBUTE_CLR);
        states &= ~(uint32_t)PHGLColorArray;
    }
    if (states & PHGLNormalArray)
    {
        vertexAttrib |= (1 << PHIMAGEATTRIBUTE_NRM);
        states &= ~(uint32_t)PHGLNormalArray;
    }
    
    int xr = states^openGLStates;
    openGLStates = states;

    if (!useShaders() && (xr & PHGLTexture0))
    {
        if (states & PHGLTexture0)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);
    }
    if (xr & PHGLZTesting)
    {
        if (states & PHGLZTesting)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }
    if (xr & PHGLBlending)
    {
        if (states & PHGLBlending)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
    
    if (vertexAttrib)
        setGLAttributeStates(vertexAttrib);
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
        const char * vers = ver;
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
            GLint n;
            glGetIntegerv(GL_NUM_EXTENSIONS, &n);
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
        
        glslVersion = 0;
        const char * glslvp = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        const char * glslv = glslvp;
        for (;(*glslv) && glslVersion<100; glslv++)
        {
            char c = *glslv;
            if (c>='0' && c<='9')
                glslVersion = glslVersion*10 + (c-'0');
        }
        while (glslVersion && glslVersion<100)
            glslVersion *= 10;
        
#ifdef PH_DEBUG
        string s;
        for (set<string>::iterator i = extensions.begin(); i!=extensions.end(); i++)
        {
            s+=*i;
            s+=" ";
        }
        
        PHLog("OpenGL Version: \"%s\" -> %s %d.%d",vers,es?"ES":"",openGLVersionMajor,openGLVersionMinor);
        PHLog("GLSL Version: \"%s\" -> %d",glslvp,glslVersion);
        PHLog("OpenGL Extensions: %s",s.c_str());
#endif
        
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
        GLint tmp;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
        _maxVertexAttribs = (int)tmp;
        
        stringstream sstr;
        sstr<<"#version "<<glslVersion<<"\n";
        sstr<<"#define PH_ENGINE\n";
        sstr<<"#define PH_GLSL_VERSION "<<glslVersion<<"\n";
        sstr<<"#define PH_GL_MINOR_VERSION "<<openGLVersionMinor<<"\n";
        sstr<<"#define PH_GL_MAJOR_VERSION "<<openGLVersionMajor<<"\n";
        if (!es)
        {
            sstr<<"#define highp \n";
            sstr<<"#define mediump \n";
            sstr<<"#define lowp \n";
            sstr<<"#define PH_GL_ES\n";
        }
        
        glslHeader = sstr.str();
        
        
#ifndef PH_FORCE_FAKE_VAO
#ifdef GL_VERSION_3_0
        if (openGLVersionMajor >= 3 && !es)
        {
            PHGLBindVertexArray = (GLvoid (*)(GLuint))&glBindVertexArray;
            PHGLDeleteVertexArrays = (GLvoid (*)(GLsizei,const GLuint*))&glDeleteVertexArrays;
            PHGLGenVertexArrays = (GLvoid (*)(GLsizei,GLuint*))&glGenVertexArrays;
        }
        else
#endif
        
#ifdef GL_OES_vertex_array_object
        if (extensions.count("GL_OES_vertex_array_object"))
        {
            PHGLBindVertexArray = (GLvoid (*)(GLuint))&glBindVertexArrayOES;
            PHGLDeleteVertexArrays = (GLvoid (*)(GLsizei,const GLuint*))&glDeleteVertexArraysOES;
            PHGLGenVertexArrays = (GLvoid (*)(GLsizei,GLuint*))&glGenVertexArraysOES;
        }
        else
#endif
        
//#ifdef GL_APPLE_vertex_array_object
//        if (extensions.count("GL_APPLE_vertex_array_object"))
//        {
//            PHGLBindVertexArray = (GLvoid (*)(GLuint))&glBindVertexArrayAPPLE;
//            PHGLDeleteVertexArrays = (GLvoid (*)(GLsizei,const GLuint*))&glDeleteVertexArraysAPPLE;
//            PHGLGenVertexArrays = (GLvoid (*)(GLsizei,GLuint*))&glGenVertexArraysAPPLE;
//        }
//        else
//#endif
        
#ifdef GL_ARB_vertex_array_object
        if (extensions.count("GL_ARB_vertex_array_object"))
        {
            PHGLBindVertexArray = (GLvoid (*)(GLuint))&glBindVertexArray;
            PHGLDeleteVertexArrays = (GLvoid (*)(GLsizei,const GLuint*))&glDeleteVertexArrays;
            PHGLGenVertexArrays = (GLvoid (*)(GLsizei,GLuint*))&glGenVertexArrays;
        }
        else
#endif
#endif
        {
            PHGLBindVertexArray = NULL;
            PHGLDeleteVertexArrays = NULL;
            PHGLGenVertexArrays = NULL;
        }
    }
}

bool PHGameManager::hasExtension(const string & ext)
{
    return extensions.count(ext);
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

void PHGameManager::applyShader(PHGLShaderProgram * shader)
{
    if (useShaders())
    {
        if (!shader) return;
        (*spriteStates)[modelViewSpriteUniform] = _projection * _modelView;
        (*spriteStates)[colorSpriteUniform] = _currentColor;
        useShader(shader);
        spriteStates->apply(shader);
    }
}

void PHGameManager::applySpriteShader()
{
    applyShader(spriteShader());
}

void PHGameManager::useShader(PHGLShaderProgram * prog)
{
    if (_shader==prog)
        return;
    _shader = prog;
    prog->use();
}

void PHGameManager::bindVBO(PHGLVertexBufferObject * vbo, int location)
{
    if (location==0) return;
    if ((location == PHGLVBO::elementArrayBuffer) && _boundVAO)
    {
        if (_boundVAO->elementVBO == vbo) return;
        if (_boundVAO->elementVBO)
            _boundVAO->elementVBO->bound = 0;
        if (vbo)
            vbo->bound = location;
        _boundVAO->elementVBO = vbo;
        glBindBuffer(PHGLVertexBufferObject::targets[location], vbo?(vbo->vbo):0);
        return;
    }
    if (boundVBOs[location]==vbo) return;
    if (boundVBOs[location])
        boundVBOs[location]->bound = 0;
    if (vbo)
        vbo->bound = location;
    boundVBOs[location] = vbo;
    glBindBuffer(PHGLVertexBufferObject::targets[location], vbo?(vbo->vbo):0);
}

void PHGameManager::bindVAO(PHGLVertexArrayObject * vao)
{
    if (_boundVAO == vao) return;
    if (_boundVAO)
    {
        _boundVAO->bound = false;
        if (_boundVAO->elementVBO)
            _boundVAO->elementVBO->bound = 0;
    } else
        if (boundVBOs[PHGLVBO::elementArrayBuffer])
            boundVBOs[PHGLVBO::elementArrayBuffer]->bound = 0;
    if (vao)
    {
        vao->bound = true;
        if (vao->elementVBO)
            vao->elementVBO->bound = PHGLVBO::elementArrayBuffer;
    } else 
        if (boundVBOs[PHGLVBO::elementArrayBuffer])
            boundVBOs[PHGLVBO::elementArrayBuffer]->bound = PHGLVBO::elementArrayBuffer;
    
    if (PHGLBindVertexArray)
        PHGLBindVertexArray(vao?(vao->vao):0);
    else
    {
        if (vao)
            vao->fakeBind();
        else
            _boundVAO->fakeUnbind();
    }
    _boundVAO = vao;
}

void PHGameManager::buildSolidSquareVAO()
{
    PHGLVertexArrayObject * vao = new PHGLVertexArrayObject(this);
    PHGLVertexBufferObject * vbo = new PHGLVertexBufferObject(this);
    
    vao->bind();
    vbo->bindTo(PHGLVBO::arrayBuffer);
    static const GLfloat a[] = {
        0,0,
        1,0,
        0,1,
        1,1
    };
    vbo->setData(a, sizeof(GLfloat)*8, PHGLVBO::staticDraw);
    vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, 0, vbo);
    vao->setDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    vao->unbind();
    vbo->unbind();
    _solidSquareVAO = vao;
    _solidSquareVBO = vbo;
}

void PHGameManager::setActiveTexture(int tmu)
{
    if (aTMU == tmu) return;
    aTMU = tmu;
    glActiveTexture(GL_TEXTURE0 + tmu);
}

void PHGameManager::bindTexture(PHGLTexture * tx)
{
    //if (textures[aTMU] == tx) return;
    if (textures[aTMU])
        textures[aTMU]->bound = false;
    textures[aTMU] = tx;
    if (tx)
    {
        tx->bound = true;
        tx->_bind();
    }
}

void PHGameManager::destroyTexture(PHGLTexture * tx)
{
    for (int i = 0; i < PHGameManager_maxTextures; i++)
        if (textures[aTMU] == tx)
            textures[aTMU] = NULL;
}
