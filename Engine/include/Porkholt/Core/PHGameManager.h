/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGAMEMANAGER_H
#define PHGAMEMANAGER_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHImageInitPool.h>
#include <Porkholt/Core/PHFontInitPool.h>
#include <Porkholt/Core/PHGLProgramInitPool.h>
#include <Porkholt/Core/PHMaterialInitPool.h>
#include <Porkholt/Core/PHMessage.h>
#include <Porkholt/Core/PHGLLight.h>

class PHAnimatorPool;
class PHDirectory;
class PHEventHandler;
class PHGLFramebuffer;
class PHGLShaderProgram;
class PHGLTexture;
class PHGLUniformStates;
class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHNavigationController;
class PHRemote;
class PHTextView;
class PHDrawable;
class PHSoundManager;
class PH2DCamera;

#undef uniformtrick
#ifdef PHGLUNIFORMSTATES_H
#define uniformtrick PHGLUniformStates::uniform
#else
#define uniformtrick void
#endif

enum PHGLCapabilities
{
    PHGLCapabilityNPOT = 0,
    PHGLCapabilityAppleLimitedNPOT,
    PHGLCapabilityOpenGLES,
    PHGLCapabilityGLES1,
    PHGLCapabilityGLES2,
    PHGLCapabilityShaders,
    PHGLCapabilityFuckedUpAdreno,
    PHGLNumberCapabilities
};

#define PHGameManager_maxTextures 16

class PHGameManagerInitParameters
{
public:
    PHGameManagerInitParameters() : screenWidth(480), screenHeight(340), fps(60), dpi(150), defaultFBO(0), entryPoint(NULL), resourceDir(NULL) {}

    float screenWidth;
    float screenHeight;
    int fps;
    float dpi;
    GLuint defaultFBO;
    void (*entryPoint)(PHGameManager *);
private:
    PHDirectory * resourceDir;

public:
    PHDirectory * resourceDirectory() const { return resourceDir; }
    void setResourceDirectory(PHDirectory * r);

    ~PHGameManagerInitParameters();
};

enum PHGLStates
{
    PHGLVertexArray = 1<<0,
    PHGLColorArray = 1<<1,
    PHGLTextureCoordArray = 1<<2,
    PHGLTexture0 = 1<<3,
    PHGLZTesting = 1<<4,
    PHGLZWriting = 1<<5,
    PHGLResetAllStates = 1<<6,
    PHGLNormalArray = 1<<7,
    PHGLBlending = 1<<8,
    PHGLBackFaceCulling = 1<<9,
    PHGLStencilTesting = 1<<10
};

class PHGameManager : public PHObject
#ifdef DEBUG_ALLOCATIONS
, public PHAllocationDumper
#endif
, public PHImageInitPool, public PHFontInitPool, public PHGLProgramInitPool, public PHMaterialInitPool, public PHMessagePool
{
private:
	PHDrawable * drawable;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
    PHAnimatorPool * animPool;
    PHSoundManager * sndMan;
    PHDirectory * rsrcDir;
    PHDirectory * shdDir;
    PHDirectory * matDir;
    PHDirectory * imgDir;
    PHDirectory * fntDir;

	float _screenWidth;
	float _screenHeight;
    PHRect _oldBounds;
    float dpi;
	int fps;
    float lt;
	bool suspended;
    bool loaded;
    float lastElapsed, frameBeg;
    
    bool useRemote;
    PHRemote * remote;
    
    bool showFPS;
    int frames;
    float fpsLeft;
    PHTextView * fpsView;
    PH2DCamera * fpsCamera;
    bool capped, frameAnim;
    void renderFPS(float timeElapsed);
    
    void (*entryPoint)(PHGameManager*);
    void * ud;
    
    PHMessage * exitmsg;

    map < PHThread*, list<PHAnimatorPool*> > animPoolStacks;
    PHMutex * animPoolMutex;
    
public:
    PHGameManager();
    ~PHGameManager();
    void init(const PHGameManagerInitParameters & params);

	float screenWidth() { return _screenWidth; };
	float screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    PHRect oldScreenBounds() { return _oldBounds; }
    void setScreenSize(float w, float h);
    void setViewport(int x, int y, int w, int h);
    void setViewport(const PHRect & r) { setViewport(r.x, r.y, r.width, r.height); }
    void viewport(int & x, int & y, int & width, int & height);
    PHRect viewport() { int x,y,w,h; viewport(x,y,w,h); return PHRect(x,y,w,h); }
	int framesPerSecond() { return fps; }
    void setFramesPerSecond(int f) { fps = f; }
    float frameInterval() { return 1.0f/fps; }
    float elapsedTime() { return lastElapsed; }
    float dotsPerInch() { return dpi; }
	void renderFrame();
	void appSuspended();
	void appResumed();
	void appQuits();
	void memoryWarning();
    void collectGarbageResources();
	PHDrawable * mainDrawable() { return drawable; };
    void setMainDrawable(PHDrawable * v);

    PHNavigationController * setUpNavigationController();

    PHGameManager * gameManager() { return this; }
    PHDirectory * resourceDirectory() { return rsrcDir; }
    PHDirectory * fontDirectory() { if (fntDir) return fntDir; throw string("no font directory"); }
    PHDirectory * imageDirectory() { if (imgDir) return imgDir; throw string("no image directory"); }
    PHDirectory * shaderDirectory() { if (shdDir) return shdDir; throw string("no shader directory"); }
    PHDirectory * materialDirectory() { if (matDir) return matDir; throw string("no material directory"); }

    const string musicNamed(const string & name);
    bool usesRemote() { return useRemote; }
    void setUsesRemote(bool ur) { useRemote = ur; }
    bool showsFPS() { return showFPS; }
    void setShowsFPS(bool fps) { showFPS = fps; }
    bool fpsCapped() { return capped; }
    void setFpsCapped(bool b) { capped = b; }
    bool frameAnimation() { return frameAnim; }
    void setFrameAnimation(bool f) { frameAnim = f; }
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    void processInput();
    PHEventHandler * eventHandler() { return evtHandler; }
    PHAnimatorPool * mainAnimatorPool() { return animPool; }
    PHSoundManager * soundManager() { return sndMan; } 

    //NOTE: There is one stack per thread
    PHAnimatorPool * animatorPool();
    void pushAnimatorPool(PHAnimatorPool * pool);
    void popAnimatorPool();
    
    PHMessage * deallocMessage() { return exitmsg; }
    
//------ OpenGL stuff ------    
private:
    GLuint _defaultFBO;
    int _maxVertexAttribs;
    
    uint32_t openGLStates,openGLVertexAttribStates;
    PHMatrix _modelView,_projection;
    PHColor _currentColor;
        
    list<PHGLShaderProgram*> spriteShaderStack;
    PHGLUniformStates * spriteStates;
    uniformtrick * modelViewSpriteUniform;
    uniformtrick * colorSpriteUniform;
    uniformtrick * textureSpriteUniform;
    PHGLShaderProgram * _shader;
    PHGLShaderProgram * _spriteShader;
    int rndMode;
    PHGLVertexBufferObject * boundVBOs[5];
    PHGLVertexArrayObject * _boundVAO;
    PHGLVertexArrayObject * _solidSquareVAO;
    PHGLVertexArrayObject * _fullScreenVAO;
    PHGLFramebuffer * _boundFBO;
    
    
    PHGLLight * lgth;
    PHColor ambient;

    PHColor _windowClearColor;
    float _windowDepthClearValue;
    
    friend class PHGLVertexBufferObject;
    friend class PHGLVertexArrayObject;
    friend class PHGLShaderProgram;
    friend class PHGLFramebuffer;
    friend class PHGLRenderbuffer;
    
    vector<GLuint> deleteVBOs;
    vector<GLuint> deleteVAOs;
    void queueDeleteVBO(GLuint vbo) { deleteVBOs.push_back(vbo); }
    void queueDeleteVAO(GLuint vao) { deleteVAOs.push_back(vao); }
    void clearDeleteQueue();
    
    PHGLTexture * textures[PHGameManager_maxTextures];
    int aTMU;
    int clat;
    
    #include <Porkholt/Core/PHGLExtCompat.h>
    
public:
    
    enum renderModes
    {
        defaultRenderMode = 0
    };
    
    GLuint defaultFBO() { return _defaultFBO; }
    void setRenderMode(int rm) { rndMode = rm; }
    int renderMode() { return rndMode; }
    
    int maxVertexAttribs() { return _maxVertexAttribs; }
    void setGLStates(uint32_t states) { setGLStates(states,0); }
    void setGLStates(uint32_t states, uint32_t vertexAttribStates);
    void setGLAttributeStates(uint32_t vertexAttribStates);
    uint32_t getGLStates() { return openGLStates; }
    void setModelViewMatrix(const PHMatrix & m);
    PHMatrix modelViewMatrix() { return _modelView; }
    void setProjectionMatrix(const PHMatrix & m);
    PHMatrix projectionMatrix() { return _projection; }
    void setColor(PHColor clr)
    {
        if (clr.a<0)
            clr = PHWhiteColor;
        if (clr != _currentColor)
        {
            _currentColor = clr;
            if (!useShaders())
            {
                PH24BitColor t(clr);
                PHGL::glColor4ub(t.r,t.g,t.b,t.a);
            }
        }
    }
    const PHColor & color()
    {
        return _currentColor;
    }

    void setWindowClearColor(const PHColor & c) { _windowClearColor = c; }
    const PHColor & windowClearColor() { return _windowClearColor; }
    void setWindowDepthClearValue(float value) { _windowDepthClearValue = value; }
    float windowDepthClearValue() { return _windowDepthClearValue; }
        
    PHGLShaderProgram * spriteShader() { if (spriteShaderStack.empty()) return NULL; return spriteShaderStack.back(); }
    void pushSpriteShader(PHGLShaderProgram * p);
    void popSpriteShader();
    PHGLVertexArrayObject * fullScreenVAO() { if (!_fullScreenVAO) buildFullScreenVAO(); return _fullScreenVAO; }
    void buildFullScreenVAO();
    PHGLVertexArrayObject * solidSquareVAO() { if (!_solidSquareVAO) buildSolidSquareVAO(); return _solidSquareVAO; }
    void buildSolidSquareVAO();
    
    PHGLShaderProgram * normalSpriteShader() { return _spriteShader; }

    PHGLUniformStates * spriteUniformStates() { return spriteStates; }
    void updateMatrixUniform();
    void updateColorUniform();
    void setTextureUniform(PHGLTexture * tex);
    void updateSpriteUniforms();

    PHGLShaderProgram * shader() { return _shader; }
    void useShader(PHGLShaderProgram * prog);
    
    void bindVBO(PHGLVertexBufferObject * vbo, int target);
    void bindVAO(PHGLVertexArrayObject * vao);
    PHGLVertexBufferObject * boundVBO(int target) { return boundVBOs[target]; }
    PHGLVertexArrayObject * boundVAO() { return _boundVAO; }
    void bindFramebuffer(PHGLFramebuffer * fbo);
    PHGLFramebuffer * boundFramebuffer() { return _boundFBO; }
    
    
#define PHIMAGEATTRIBUTE_POS 0
#define PHIMAGEATTRIBUTE_TXC 1
#define PHIMAGEATTRIBUTE_CLR 2
#define PHIMAGEATTRIBUTE_NRM 3

    void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (useShaders())
            PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, size, type, GL_FALSE, stride, ptr);
        else
            PHGL::glVertexPointer(size, type, stride, ptr);
    }
    
    void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (useShaders())
            PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, size, type, GL_FALSE, stride, ptr);
        else
            PHGL::glTexCoordPointer(size, type, stride, ptr);
    }
    
    void normalPointer(GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (useShaders())
            PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_NRM, 3, type, GL_FALSE, stride, ptr);
        else
            PHGL::glNormalPointer(type, stride, ptr);
    }
    
    void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (useShaders())
            PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_CLR, size, type, (type==GL_UNSIGNED_BYTE), stride, ptr);
        else
            PHGL::glColorPointer(size, type, stride, ptr);
    }
    
    PHGLLight * currentLight() { return lgth; }
    void setCurrentLight(PHGLLight * l) {
        if (l) l->retain();
        if (lgth) lgth->release();
        lgth = l;
    }
    PHColor ambientColor() { return ambient; }
    void setAmbientColor(const PHColor & c) { ambient = c; }
    
    void setActiveTexture(int tmu);
    int activeTexture() { return aTMU; }
    void bindTexture(PHGLTexture * tx);
    void destroyTexture(PHGLTexture * tx);
    PHGLTexture * boundTexture() { return textures[aTMU]; }

    int colorAttachmentCount();
};

#endif
