/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGAMEMANAGER_H
#define PHGAMEMANAGER_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHImageInitPool.h>
#include <Porkholt/Core/PHFontInitPool.h>
#include <Porkholt/Core/PHGLProgramInitPool.h>
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
class PHView;
class PHViewController;

enum PHGLCapabilities
{
    PHGLCapabilityNPOT = 0,
    PHGLCapabilityAppleLimitedNPOT,
    PHGLCapabilityOpenGLES,
    PHGLCapabilityGLES1,
    PHGLCapabilityGLES2,
    PHGLCapabilityShaders,
    PHGLNumberCapabilities
};

#define PHGameManager_maxTextures 16

class PHGameManagerInitParameters
{
public:
    PHGameManagerInitParameters() : screenWidth(480), screenHeight(340), fps(60), dpi(150), defaultFBO(0), entryPoint(NULL), resourceDir(NULL) {}

    ph_float screenWidth;
    ph_float screenHeight;
    int fps;
    ph_float dpi;
    GLuint defaultFBO;
    void (*entryPoint)(PHGameManager *);
private:
    PHDirectory * resourceDir;

public:
    PHDirectory * resourceDirectory() const { return resourceDir; }
    void setResourceDirectory(PHDirectory * r);
};

enum PHGLStates
{
    PHGLVertexArray = 1<<0,
    PHGLColorArray = 1<<1,
    PHGLTextureCoordArray = 1<<2,
    PHGLTexture0 = 1<<3,
    PHGLZTesting = 1<<4,
    PHGLResetAllStates = 1<<5,
    PHGLNormalArray = 1<<6,
    PHGLBlending = 1<<7
};

class PHGameManager : public PHObject, public PHImageInitPool, public PHFontInitPool, public PHGLProgramInitPool
{
private:
	PHView * view;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
    PHAnimatorPool * animPool;
    PHDirectory * rsrcDir;
    PHDirectory * shdDir;
    PHDirectory * imgDir;
    PHDirectory * fntDir;

	ph_float _screenWidth;
	ph_float _screenHeight;
    ph_float dpi;
	int fps;
    ph_float lt;
	bool suspended;
    bool loaded;
    ph_float lastElapsed, frameBeg;
    
    bool useRemote;
    PHRemote * remote;
    
    bool showFPS;
    int frames;
    ph_float fpsLeft;
    PHTextView * fpsView;
    bool capped;
    void renderFPS(ph_float timeElapsed);
    
    void (*entryPoint)(PHGameManager*);
    
    void setProjection();
    void * ud;
    
    PHMessage * exitmsg;

    map < PHThread*, list<PHAnimatorPool*> > animPoolStacks;
    PHMutex * animPoolMutex;
    
public:
    PHGameManager();
    ~PHGameManager();
    void init(const PHGameManagerInitParameters & params);

	ph_float screenWidth() { return _screenWidth; };
	ph_float screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    void setScreenSize(ph_float w, ph_float h);
	int framesPerSecond() { return fps; }
    void setFramesPerSecond(int f) { fps = f; }
    ph_float frameInterval() { return 1.0f/fps; }
    ph_float elapsedTime() { return lastElapsed; }
    ph_float dotsPerInch() { return dpi; }
	void renderFrame(ph_float timeElapsed);
	void appSuspended();
	void appResumed();
    void _appResumed(PHObject * sender, void * ud);
    void _appSuspended(PHObject * sender, void * ud);
	void appQuits();
	void memoryWarning();
	PHView * mainView() { return view; };
    void setMainView(PHView * v);

    PHGameManager * gameManager() { return this; }
    PHDirectory * resourceDirectory() { return rsrcDir; }
    PHDirectory * fontDirectory() { if (fntDir) return fntDir; throw string("no font directory"); }
    PHDirectory * imageDirectory() { if (imgDir) return imgDir; throw string("no image directory"); }
    PHDirectory * shaderDirectory() { if (shdDir) return shdDir; throw string("no shader directory"); }

    const string musicNamed(const string & name);
    bool usesRemote() { return useRemote; }
    void setUsesRemote(bool ur) { useRemote = ur; }
    bool showsFPS() { return showFPS; }
    void setShowsFPS(bool fps) { showFPS = fps; }
    bool fpsCapped() { return capped; }
    void setFpsCapped(bool b) { capped = b; }
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    void processInput();
    PHEventHandler * eventHandler() { return evtHandler; }
    PHAnimatorPool * mainAnimatorPool() { return animPool; }

    //NOTE: There is one stack per thread
    PHAnimatorPool * animatorPool();
    void pushAnimatorPool(PHAnimatorPool * pool);
    void popAnimatorPool();
    
    PHNavigationController * navigationController() { return viewController; }
    PHView * rootView() { return view; }
    
    PHMessage * deallocMessage() { return exitmsg; }
    
//------ OpenGL stuff ------    
private:
    GLuint _defaultFBO;
    int _maxVertexAttribs;
    
    uint32_t openGLStates,openGLVertexAttribStates;
    PHMatrix _modelView,_projection;
    PHColor _currentColor;
    set<string> extensions;
    bool parsedExtensions;
    int openGLVersionMajor,openGLVersionMinor,glslVersion;
    string glslHeader;
    bool openGLCaps[PHGLNumberCapabilities];
    
    list<PHGLShaderProgram*> spriteShaderStack;
    PHGLUniformStates * spriteStates;
    PHGLShaderProgram * _shader;
    PHGLShaderProgram * _spriteShader;
    int rndMode;
    PHGLVertexBufferObject * boundVBOs[5];
    PHGLVertexArrayObject * _boundVAO;
    PHGLVertexArrayObject * _solidSquareVAO;
    PHGLVertexBufferObject * _solidSquareVBO;
    PHGLFramebuffer * _boundFBO;
    
    
    PHGLLight * lgth;
    PHColor ambient;
    
    friend class PHGLVertexBufferObject;
    friend class PHGLVertexArrayObject;
    friend class PHGLShaderProgram;
    friend class PHGLFramebuffer;
    friend class PHGLRenderBuffer;
    
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
                glColor4ub(t.r,t.g,t.b,t.a);
            }
        }
    }
    const PHColor & color();
    void loadCapabilities();
    bool isGLES() { return openGLCaps[PHGLCapabilityOpenGLES]; }
    bool useShaders() { return openGLCaps[PHGLCapabilityShaders]; }
    int openGLMajorVersion() { return openGLVersionMajor; }
    int openGLMinorVersion() { return openGLVersionMinor; }
    int openGLSLVersion() { return glslVersion; }
    bool hasCapability(int cap) { return openGLCaps[cap]; }
    bool hasExtension(const string & ext);
    
    PHGLShaderProgram * spriteShader() { if (spriteShaderStack.empty()) return NULL; return spriteShaderStack.back(); }
    void pushSpriteShader(PHGLShaderProgram * p);
    void popSpriteShader();
    PHGLVertexArrayObject * solidSquareVAO() { if (!_solidSquareVAO) buildSolidSquareVAO(); return _solidSquareVAO; }
    void buildSolidSquareVAO();
    
    PHGLShaderProgram * normalSpriteShader() { return _spriteShader; }
    
    enum
    {
        modelViewSpriteUniform = 0,
        colorSpriteUniform,
        textureSpriteUniform
    };
    
    PHGLUniformStates * spriteUniformStates() { return spriteStates; }
    void applySpriteShader();
    void applyShader(PHGLShaderProgram * shader);
    void reapplyMatrixUniform();
    void reapplyColorUniform();
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
        if (openGLCaps[PHGLCapabilityShaders])
            glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, size, type, GL_FALSE, stride, ptr);
        else
            glVertexPointer(size, type, stride, ptr);
    }
    
    void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (openGLCaps[PHGLCapabilityShaders])
            glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, size, type, GL_FALSE, stride, ptr);
        else
            glTexCoordPointer(size, type, stride, ptr);
    }
    
    void normalPointer(GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (openGLCaps[PHGLCapabilityShaders])
            glVertexAttribPointer(PHIMAGEATTRIBUTE_NRM, 3, type, GL_FALSE, stride, ptr);
        else
            glNormalPointer(type, stride, ptr);
    }
    
    void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
    {
        if (openGLCaps[PHGLCapabilityShaders])
            glVertexAttribPointer(PHIMAGEATTRIBUTE_CLR, size, type, (type==GL_UNSIGNED_BYTE), stride, ptr);
        else
            glColorPointer(size, type, stride, ptr);
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
