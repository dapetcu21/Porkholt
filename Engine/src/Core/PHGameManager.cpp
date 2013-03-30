/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLUniformStates.h> //this must go before PHGameManager.h for uniformtrick to work
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHRemote.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHNavigationController.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHFont.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGLFramebuffer.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Core/PHThreading.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHViewControllerHolder.h>
#include <Porkholt/Core/PHProfilerCollection.h>
#include <Porkholt/IO/PHEmbeddedDirectory.h>
#include <Porkholt/IO/PHUnionDirectory.h>

PHGameManager::PHGameManager() : drawable(NULL), viewController(NULL), loaded(false), useRemote(false), remote(NULL), showFPS(false), fpsView(NULL), fpsCamera(NULL), capped(false), frameAnim(false), openGLStates(0), openGLVertexAttribStates(0), parsedExtensions(false), openGLVersionMajor(0), openGLVersionMinor(0), spriteStates(NULL), _shader(NULL), _spriteShader(NULL), rndMode(defaultRenderMode), _boundVAO(NULL), _solidSquareVAO(NULL), _fullScreenVAO(NULL), _boundFBO(NULL), lgth(NULL), ambient(PHClearColor), _windowClearColor(PHBlackColor), _windowDepthClearValue(1.0f), aTMU(0), clat(0), ccolor(PHInvalidColor), cdepth(1.0f), cstencil(0), _currentColor(0, 0, 0, 0), stencilF(stencilAlways), stencilRef(0), stencilMask((unsigned int)-1), stencilOpSF(stencilKeep), stencilOpDF(stencilKeep), stencilOpDP(stencilKeep)
{
    memset(boundVBOs, 0, sizeof(boundVBOs));
    memset(textures, 0, sizeof(textures));
}


PHGameManager::~PHGameManager()
{
    PHAutoreleasePool pool;
    PHLog("Dealocating game manager");
    if (exitmsg)
        exitmsg->broadcast(this, NULL);
    if (drawable)
        drawable->release();
    if (fpsView)
        fpsView->release();
    if (fpsCamera)
        fpsCamera->release();
    if (viewController)
        viewController->release();
    if (_fullScreenVAO)
        _fullScreenVAO->release();
    if (_solidSquareVAO)
        _solidSquareVAO->release();
    if (_spriteShader)
        _spriteShader->release();
    for (list<PHGLShaderProgram*>::iterator i = spriteShaderStack.begin(); i != spriteShaderStack.end(); i++)
        (*i)->release();
    if (evtHandler)
        evtHandler->release();
    if (animPool)
        animPool->release();
    if (spriteStates)
        spriteStates->release();
    if (lgth)
        lgth->release();
    if (rsrcDir)
        rsrcDir->release();
    if (fntDir)
        fntDir->release();
    if (shdDir)
        shdDir->release();
    if (matDir)
        matDir->release();
    if (imgDir)
        imgDir->release();
    if (sndMan)
        sndMan->release();
    if (remote)
        delete remote;
    clearDeleteQueue();
    if (animPoolMutex)
        animPoolMutex->release();
} 
void PHGameManager::setMainDrawable(PHDrawable * v)
{
    if (v)
    {
        v->setGameManager(this);
        v->retain();
    }
    if (drawable)
    {
        drawable->setGameManager(NULL);
        drawable->release();
    }
    drawable = v;
}

void PHGameManagerInitParameters::setResourceDirectory(PHDirectory * r)
{
    if (resourceDir)
        resourceDir->release();
    if (r)
        r->retain();
    resourceDir = r;
}


PHGameManagerInitParameters::~PHGameManagerInitParameters()
{
    if (resourceDir)
        resourceDir->release();
}

void PHGameManager::init(const PHGameManagerInitParameters & params)
{
    PHAutoreleasePool ap;
    initPHGL();
    
    exitmsg = messageWithName("appQuits");

	fps = params.fps;
	_screenWidth = params.screenWidth;
	_screenHeight = params.screenHeight;
    _oldBounds = screenBounds();
    dpi = params.dpi;
	suspended = 0;
    loaded = true;
    entryPoint = params.entryPoint;
    _defaultFBO = params.defaultFBO;
    if (_defaultFBO == 0)
        PHGL::glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_defaultFBO);
    lt = PHTime::getTime();
	setUserData(ud);
    
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
    animPool = new PHAnimatorPool();
    animPoolMutex = new PHMutex();

    PHUnionDirectory * rdir = new PHUnionDirectory();
    PHDirectory * realRsrc = params.resourceDirectory();
    if (!realRsrc)
        try { realRsrc = PHInode::directoryAtFSPath("./rsrc"); }
        catch(...) {}
    rdir->addDirectory(realRsrc);
    PHEmbeddedDirectory * embedRsrc = NULL;
    try { embedRsrc = new PHEmbeddedDirectory("rsrc"); } 
    catch (...) {}
    if (embedRsrc)
    {
        rdir->addDirectory(embedRsrc);
        embedRsrc->release();
    }
    rsrcDir = rdir;

    try {
        shdDir = rsrcDir->directoryAtPath("shaders");
        shdDir->retain();
    } catch(...) {
        shdDir = NULL;
    }
    try {
        matDir = rsrcDir->directoryAtPath("materials");
        matDir->retain();
    } catch(...) {
        matDir = NULL;
    }
    try {
        imgDir = rsrcDir->directoryAtPath("img");
        imgDir->retain();
    } catch(...) {
        imgDir = NULL; 
    }
    try {
        fntDir = rsrcDir->directoryAtPath("fnt");
        fntDir->retain();
    } catch (...) {
        fntDir = NULL;
    }
     
    sndMan = NULL;
#ifndef PH_LIVEPAPERS
    try {
        sndMan = new PHSoundManager(rsrcDir->directoryAtPath("snd"), this);
        animPool->addAnimator(sndMan);
    } catch (...) {}
#endif
    
    PHGL::glDepthFunc(GL_LEQUAL);
	PHGL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    PHGL::glCullFace(GL_BACK);
    //PHGL::glPolygonMode(GL_FRONT,GL_LINE); //wireframe
    PHGL::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (!useShaders())
        PHGL::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    setGLStates(PHGLResetAllStates);
    setModelViewMatrix(PHIdentityMatrix);
    setProjectionMatrix(PHIdentityMatrix);
    setViewport(0, 0, _screenWidth, _screenHeight);
    
    if (useShaders())
    {
        pushSpriteShader(_spriteShader = shaderProgramNamed("sprites"));
        _spriteShader->retain();
        spriteStates = new PHGLUniformStates;
        modelViewSpriteUniform = &spriteStates->at("modelViewProjectionMatrix");
        colorSpriteUniform = &spriteStates->at("color");
        textureSpriteUniform = &spriteStates->at("texture");
    }
    
    entryPoint(this);
}

void PHGameManager::setModelViewMatrix(const PHMatrix & m)
{
    if (!useShaders())
    {
        PHGL::glMatrixMode(GL_MODELVIEW);
        PHGL::glLoadMatrixf(m.floats());
    }
    _modelView = m;
}

void PHGameManager::setProjectionMatrix(const PHMatrix & m)
{
    if (!useShaders())
    {
        PHGL::glMatrixMode(GL_PROJECTION);
        PHGL::glLoadMatrixf(m.floats());
    }
    _projection = m;
}

void PHGameManager::setViewport(int x, int y, int w, int h)
{
    PHGL::glViewport(x, y, w, h);
}

void PHGameManager::viewport(int & x, int & y, int & width, int & height)
{
    GLint v[4];
    PHGL::glGetIntegerv(GL_VIEWPORT, v);
    x = v[0]; y = v[1];
    width = v[2]; height = v[3];
}

void PHGameManager::setScreenSize(float w, float h)
{
    _oldBounds = screenBounds();
    _screenWidth = w;
    _screenHeight = h;
    setViewport(0, 0, _screenWidth, _screenHeight);
    messageWithName("reshapeWindow")->broadcast(this);
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

void PHGameManager::renderFrame()
{	
    PHAutoreleasePool pool;
    
    float tm = PHTime::getTime();
    if (lt == 0)
        lt = tm - frameInterval();
    float realTimeElapsed = tm - lt;
    lt = tm;

    float timeElapsed;
    if (frameAnim)
        timeElapsed = frameInterval();
    else
    {
        if (capped)
            timeElapsed = roundf(realTimeElapsed * framesPerSecond()) * frameInterval();
        else
            timeElapsed = realTimeElapsed;
    }

    evtHandler->processQueue();
    
    lastElapsed = timeElapsed;
    
    setClearColor(_windowClearColor);
    setDepthClearValue(_windowDepthClearValue);
    clearBuffers(PHGameManager::colorBuffers | PHGameManager::depthBuffer);
    setModelViewMatrix(PHIdentityMatrix);
    
    animPool->advanceAnimation(timeElapsed);
    
    if (drawable)
	      drawable->render();
    
    if (showFPS)
        renderFPS(realTimeElapsed);
    
    clearDeleteQueue();

#ifdef PH_PROFILING
    PHMainProfilers->collect(frameInterval());
#endif
}

void PHGameManager::renderFPS(float timeElapsed)
{
    if (!fpsView)
    {
        fpsCamera = new PH2DCamera();
        fpsCamera->setGameManager(this);
        fpsView = new PHTextView();
        fpsView->setGameManager(this);
        fpsView->setFont(defaultFont());
        fpsView->setFontSize(20.0f);
        fpsView->setFontColor(PHColor(0.1f,0.9f,1.0f));
        fpsView->setAlignment(PHTextView::alignTop | PHTextView::justifyLeft);
        fpsView->setFrame(screenBounds());
        fpsView->setAutoresizeMask(PHView::ResizeAll);
        fpsCamera->addChild(fpsView);
        fpsLeft = 1.0f;
        frames = 0;
    }
    frames++;
    fpsLeft -= timeElapsed;
    if (fpsLeft <=0)
    {
        stringstream s;
        s.precision(1);
        s.setf(ios::fixed, ios::floatfield);
        s<<float(frames)/(1.0f-fpsLeft)<<" FPS";
        fpsView->setText(s.str());
        frames = 0;
        fpsLeft = 1.0f;
    }
    fpsCamera->render();
}

void PHGameManager::clearDeleteQueue()
{
    if (!deleteVBOs.empty())
        PHGL::glDeleteBuffers(deleteVBOs.size(), &(deleteVBOs[0]));
    if (!deleteVAOs.empty())
        PHGL::glDeleteVertexArrays(deleteVAOs.size(), &(deleteVAOs[0]));
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
                    PHGL::glEnableVertexAttribArray(i);
                else
                    PHGL::glDisableVertexAttribArray(i);
            }
        }
    } else {
        uint32_t mask;
        mask = (1 << PHIMAGEATTRIBUTE_POS);
        if (xra & mask )
        {
            if (vertexAttrib & mask)
                PHGL::glEnableClientState(GL_VERTEX_ARRAY);
            else
                PHGL::glDisableClientState(GL_VERTEX_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_CLR);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                PHGL::glEnableClientState(GL_COLOR_ARRAY);
            else
                PHGL::glDisableClientState(GL_COLOR_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_TXC);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                PHGL::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            else
                PHGL::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        mask = (1 << PHIMAGEATTRIBUTE_NRM);
        if (xra & mask)
        {
            if (vertexAttrib & mask)
                PHGL::glEnableClientState(GL_NORMAL_ARRAY);
            else
                PHGL::glDisableClientState(GL_NORMAL_ARRAY);
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
            PHGL::glEnable(GL_TEXTURE_2D);
        else
            PHGL::glDisable(GL_TEXTURE_2D);
    }
    if (xr & PHGLZTesting)
    {
        if (states & PHGLZTesting)
            PHGL::glEnable(GL_DEPTH_TEST);
        else
            PHGL::glDisable(GL_DEPTH_TEST);
    }
    if (xr & PHGLZWriting)
    {
        if (states & PHGLZWriting)
            PHGL::glDepthMask(GL_TRUE);
        else
            PHGL::glDepthMask(GL_FALSE);
    }
    if (xr & PHGLBackFaceCulling)
    {
        if (states & PHGLBackFaceCulling)
            PHGL::glEnable(GL_CULL_FACE);
        else
            PHGL::glDisable(GL_CULL_FACE);
    }
    if (xr & PHGLBlending)
    {
        if (states & PHGLBlending)
            PHGL::glEnable(GL_BLEND);
        else
            PHGL::glDisable(GL_BLEND);
    }
    if (xr & PHGLStencilTesting)
    {
        if (states & PHGLStencilTesting)
            PHGL::glEnable(GL_STENCIL_TEST);
        else
            PHGL::glDisable(GL_STENCIL_TEST);
    }
    
    if (vertexAttrib)
        setGLAttributeStates(vertexAttrib);
    PHGLCheckError();
}

void PHGameManager::appSuspended()
{
    if (!loaded) return;
    if (!loaded) return;
	if (suspended) return;
	suspended = true;
	messageWithName("appSuspended")->broadcast(this, NULL);
    PHLog("appSuspended");
#ifdef PH_SIMULATOR
    remote->stop();
#endif
    
}

void PHGameManager::appResumed()
{
    if (!loaded) return;
    if (!suspended) return;
	suspended = false;
	PHLog("appResumed");
	messageWithName("appResumed")->broadcast(this, NULL);
#ifdef PH_SIMULATOR
    try {
        remote->start();
    } catch (string err) {
        PHLog("URemote: %s",err.c_str());
    }
#endif
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

void PHGameManager::useShader(PHGLShaderProgram * prog)
{
    if (_shader==prog) return;
    _shader = prog;
    prog->_use();
}

void PHGameManager::updateMatrixUniform()
{
    modelViewSpriteUniform->setValue(_projection * _modelView);
}

void PHGameManager::updateColorUniform()
{
    colorSpriteUniform->setValue(_currentColor);
}

void PHGameManager::updateSpriteUniforms()
{
    colorSpriteUniform->setValue(_currentColor);
    modelViewSpriteUniform->setValue(_projection * _modelView);
}

void PHGameManager::setTextureUniform(PHGLTexture * tex)
{
    textureSpriteUniform->setValue(tex);
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
        _boundVAO->setElementArrayVBO(vbo);
        return;
    }
    if (boundVBOs[location]==vbo) return;
    if (boundVBOs[location])
        boundVBOs[location]->bound = 0;
    if (vbo)
        vbo->bound = location;
    boundVBOs[location] = vbo;
    PHGL::glBindBuffer(PHGLVertexBufferObject::targets[location], vbo?(vbo->vbo):0);
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
    
    if (PHGL::glBindVertexArray)
        PHGL::glBindVertexArray(vao?(vao->vao):0);
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
    vbo->release();
    _solidSquareVAO = vao;
}

void PHGameManager::buildFullScreenVAO()
{
    PHGLVertexArrayObject * vao = new PHGLVertexArrayObject(this);
    PHGLVertexBufferObject * vbo = new PHGLVertexBufferObject(this);
    
    vao->bind();
    vbo->bindTo(PHGLVBO::arrayBuffer);
    static const GLfloat a[] = {
        -1,-1,   0,0,
         1,-1,   1,0,
        -1, 1,   0,1,
         1, 1,   1,1
    };
    vbo->setData(a, sizeof(GLfloat)*16, PHGLVBO::staticDraw);
    vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, 0, vbo);
    vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, sizeof(GLfloat)*2, vbo);
    vao->setDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    vao->unbind();
    vbo->unbind();
    vbo->release();
    _fullScreenVAO = vao;
}

void PHGameManager::setActiveTexture(int tmu)
{
    if (aTMU == tmu) return;
    aTMU = tmu;
    PHGL::glActiveTexture(GL_TEXTURE0 + tmu);
}

void PHGameManager::bindTexture(PHGLTexture * tx)
{
    if (textures[aTMU] == tx) return;
    if (textures[aTMU])
        textures[aTMU]->bound = false;
    textures[aTMU] = tx;
    if (tx)
    {
        tx->bound = true;
        tx->_bind();
    }
}

void PHGameManager::bindFramebuffer(PHGLFramebuffer * fbo)
{
    if (_boundFBO == fbo) return;
   _boundFBO = fbo;
   if (fbo)
       PHGL::glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
   else
       PHGL::glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
}

void PHGameManager::destroyTexture(PHGLTexture * tx)
{
    for (int i = 0; i < PHGameManager_maxTextures; i++)
        if (textures[aTMU] == tx)
            textures[aTMU] = NULL;
}

PHAnimatorPool * PHGameManager::animatorPool()
{
    animPoolMutex->lock();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = animPoolStacks.find(PHThread::currentThread());
    if ((it!=animPoolStacks.end()) && (!it->second.empty()))
    {
        PHAnimatorPool * ap = it->second.back();
        animPoolMutex->unlock();
        return ap;
    }
    animPoolMutex->unlock();
    return animPool;
}

void PHGameManager::pushAnimatorPool(PHAnimatorPool * pool)
{
    animPoolMutex->lock();
    PHThread * ct = PHThread::currentThread();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = animPoolStacks.find(ct);
    pool->retain();
    if (it == animPoolStacks.end())
    {
        list<PHAnimatorPool*> l;
        l.push_back(pool);
        animPoolStacks.insert(make_pair(ct, l));
    } else {
        it->second.push_back(pool);
    }
    animPoolMutex->unlock();
}

void PHGameManager::popAnimatorPool()
{
    animPoolMutex->lock();
    PHThread * ct = PHThread::currentThread();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = animPoolStacks.find(ct);
    if (it != animPoolStacks.end())
    {
        list<PHAnimatorPool*> & l = it->second;
        if (!l.empty())
        {
            l.back()->release();
            l.pop_back();
        }
        if (l.empty())
            animPoolStacks.erase(it);
    }
    animPoolMutex->unlock();
}

PHNavigationController * PHGameManager::setUpNavigationController()
{
    PH2DCamera * camera = new PH2DCamera();
    setMainDrawable(camera);
    PHRect bounds = screenBounds();
    PHViewControllerHolder * holder = new PHViewControllerHolder(bounds);
    camera->addChild(holder);
    
    PHNavigationController * nav = new PHNavigationController(this);
    holder->setViewController(nav);
    
    camera->release();
    holder->release();
    nav->release();
    return nav;
}

void PHGameManager::collectGarbageResources()
{
    collectGarbageImages();
    collectGarbageMaterials();
    collectGarbageShaderPrograms();
    collectGarbageFonts();
    if (sndMan)
        sndMan->collectGarbageSounds();
}
