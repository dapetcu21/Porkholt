/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHRemote.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHNavigationController.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLUniformStates.h>
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

//#define PH_FORCE_FAKE_VAO

PHGameManager::PHGameManager() : view(NULL), viewController(NULL), loaded(false), useRemote(false), remote(NULL), showFPS(false), fpsView(NULL), capped(false), openGLStates(0), openGLVertexAttribStates(0), parsedExtensions(false), openGLVersionMajor(0), openGLVersionMinor(0), spriteStates(NULL), _shader(NULL), _spriteShader(NULL), rndMode(defaultRenderMode), _boundVAO(NULL), _solidSquareVAO(NULL), _solidSquareVBO(NULL), _boundFBO(NULL), lgth(NULL), ambient(PHClearColor), aTMU(0), clat(0), ccolor(PHClearColor), cdepth(1.0f), cstencil(0)
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
    if (imgDir)
        imgDir->release();
    if (sndMan)
        sndMan->release();
    if (remote)
        delete remote;
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

void PHGameManagerInitParameters::setResourceDirectory(PHDirectory * r)
{
    if (resourceDir)
        resourceDir->release();
    if (r)
        r->retain();
    resourceDir = r;
}


void PHGameManager::init(const PHGameManagerInitParameters & params)
{
	fps = params.fps;
	_screenWidth = params.screenWidth;
	_screenHeight = params.screenHeight;
    dpi = params.dpi;
	suspended = 0;
    loaded = true;
    entryPoint = params.entryPoint;
    _defaultFBO = params.defaultFBO;
    if (_defaultFBO == 0)
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_defaultFBO);
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
    rsrcDir = params.resourceDirectory();
    if (!rsrcDir)
        rsrcDir = PHInode::directoryAtFSPath("./rsrc");
    else
        rsrcDir->retain();
    shdDir = rsrcDir->directoryAtPath("shaders");
    try {
        imgDir = rsrcDir->directoryAtPath("img");
    } catch(...) {
        imgDir = NULL; 
    }
    try {
        fntDir = rsrcDir->directoryAtPath("fnt");
    } catch (...) {
        fntDir = NULL;
    }
    PHDirectory * sndDir = NULL;
    try {
        sndDir = rsrcDir->directoryAtPath("snd");
        sndMan = new PHSoundManager(sndDir);
        animPool->addAnimator(sndMan);
        sndDir->release();
        sndDir = NULL;
    } catch (...)
    {
        if (sndDir)
            sndDir->release();
    }
    
    loadCapabilities();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
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

void PHGameManager::renderFrame(ph_float timeElapsed)
{	
    PHAutoreleasePool pool;

    lastElapsed = timeElapsed;
    setClearColor(PHBlackColor);
    setDepthClearValue(1.0f);

   // glClearDepth(1.0);
   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clearBuffers(PHGameManager::colorBuffers | PHGameManager::depthBuffer);
		
    setModelViewMatrix(PHIdentityMatrix);
    
	if (viewController)  
		viewController->_updateScene(timeElapsed);
    animPool->advanceAnimation(timeElapsed);
    
	view->render();
    
    ph_float tm = PHTime::getTime();
    if (showFPS)
        renderFPS(tm - lt);
    lt = tm;
    
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
        glDeleteVertexArrays(deleteVAOs.size(), &(deleteVAOs[0]));
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
    if (xr & PHGLBackFaceCulling)
    {
        if (states & PHGLBackFaceCulling)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
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
    animPool->scheduleAction(PHInv(this,PHGameManager::_appSuspended, NULL));
    
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
    animPool->scheduleAction(PHInv(this,PHGameManager::_appResumed, NULL));
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
        
        loadExtensionCompat();
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
    
    if (glBindVertexArray)
        glBindVertexArray(vao?(vao->vao):0);
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
       glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
   else
       glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
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
