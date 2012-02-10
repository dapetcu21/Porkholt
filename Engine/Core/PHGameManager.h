/*
 *  PHGameManager.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHGAMEMANAGER_H
#define PHGAMEMANAGER_H

#include "PHMain.h"
#include "PHImageInitPool.h"
#include "PHFontInitPool.h"
#include "PHGLProgramInitPool.h"

class PHView;
class PHViewController;
class PHNavigationController;
class PHRemote;
class PHEventHandler;
class PHSoundManager;
class PHGLShaderProgram;
class PHGLUniformStates;

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

class PHGameManagerInitParameters
{
public:
    PHGameManagerInitParameters() : screenWidth(480), screenHeight(340), fps(60), dpi(150), resourcePath("./rsrc"), entryPoint(NULL), defaultFBO(0) {}

    ph_float screenWidth;
    ph_float screenHeight;
    int fps;
    ph_float dpi;
    string resourcePath;
    GLuint defaultFBO;
    void (*entryPoint)(PHGameManager *);
};

enum PHGLStates
{
    PHGLVertexArray = 1<<0,
    PHGLColorArray = 1<<1,
    PHGLTextureCoordArray = 1<<2,
    PHGLTexture = 1<<3,
    PHGLResetAllStates = 1<<4,
    PHGLNormalArray = 1<<5
};

class PHGameManager : public PHObject, public PHImageInitPool, public PHFontInitPool, public PHGLProgramInitPool
{
private:
	PHView * view;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
    PHSoundManager * sndManager;
	ph_float _screenWidth;
	ph_float _screenHeight;
    ph_float dpi;
	int fps;
	bool suspended;
    bool loaded;
    static int globalHD;
    bool hd;
    string resPath;
    GLuint _defaultFBO,_defaultFBOf;
    
#ifdef PH_SIMULATOR
    PHRemote * remote;
#endif
    
    void (*entryPoint)(PHGameManager*);
    
    void setProjection();
    void * ud;
    
    void updateHD();
    
    uint32_t openGLStates;
    PHMatrix _modelView,_projection;
    PHColor _currentColor;
    set<string> extensions;
    bool parsedExtensions;
    int openGLVersionMajor,openGLVersionMinor;
    bool openGLCaps[PHGLNumberCapabilities];
    
    list<PHGLShaderProgram*> spriteShaderStack;
    PHGLUniformStates * spriteStates;
    PHGLShaderProgram * _shader;
    PHGLShaderProgram * _spriteShader, * _coloredSpriteShader, * _noTexSpriteShader, * _coloredNoTexSpriteShader, * _textShader, * _missingNormalSpriteShader;
    int rndMode;
    
public:
    PHGameManager();
    ~PHGameManager();
    void init(const PHGameManagerInitParameters & params);
	ph_float screenWidth() { return _screenWidth; };
	ph_float screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    void setScreenSize(ph_float w, ph_float h);
	int framesPerSecond() { return fps; }
    ph_float dotsPerInch() { return dpi; }
    const string & resourcePath() { return resPath; }
    void setResourcePath(const string & r) { resPath = r; }
	void renderFrame(ph_float timeElapsed);
    static void globalFrame(ph_float timeElapsed);
	void appSuspended();
	void appResumed();
    bool isHD() { return hd; }
    GLuint defaultFBO() { if (_defaultFBOf==0) return _defaultFBO; return _defaultFBOf; }
    static bool isGloballyHD() { return globalHD > 0; }
    void _appResumed(PHObject * sender, void * ud);
    void _appSuspended(PHObject * sender, void * ud);
	void appQuits();
	void memoryWarning();
	PHView * mainView() { return view; };
	void remove(void * ud);
    const string imageDirectory();
    PHGameManager * gameManager() { return this; }
    const string fontDirectory();
    const string shaderDirectory();
    const string musicNamed(const string & name);
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    enum renderModes
    {
        defaultRenderMode = 0
    };
    
    void setRenderMode(int rm) { rndMode = rm; }
    int renderMode() { return rndMode; }
    
    void processInput();
    PHEventHandler * eventHandler() { return evtHandler; }
    PHSoundManager * soundManager() { return sndManager; }
    
    enum interfaceType
    {
        interfaceSD = 0,
        interfaceHD,
        numInterfaceTypes
    };
    static int interfaceType();
    
    PHNavigationController * navigationController() { return viewController; }
    PHView * rootView() { return view; }
    
    void setOpenGLStates(uint32_t states);
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
    int openGlMinorVersion() { return openGLVersionMinor; }
    bool hasCapability(int cap) { return openGLCaps[cap]; }
    
    PHGLShaderProgram * spriteShader() { if (spriteShaderStack.empty()) return NULL; return spriteShaderStack.back(); }
    void pushSpriteShader(PHGLShaderProgram * p);
    void popSpriteShader();
    
    PHGLShaderProgram * normalSpriteShader() { return _spriteShader; }
    PHGLShaderProgram * coloredSpriteShader() { return _coloredSpriteShader; }
    PHGLShaderProgram * noTexSpriteShader() { return _noTexSpriteShader; }
    PHGLShaderProgram * coloredNoTexSpriteShader() { return _coloredNoTexSpriteShader; }
    PHGLShaderProgram * textShader() { return _textShader; }
    PHGLShaderProgram * missingNormalSpriteShader() { return _missingNormalSpriteShader; }
    
    enum
    {
        modelViewSpriteUniform = 0,
        colorSpriteUniform,
        textureSpriteUniform
    };
    
    PHGLUniformStates * spriteUniformStates() { return spriteStates; }
    void applySpriteShader();
    void reapplyMatrixUniform();
    void reapplyColorUniform();
    PHGLShaderProgram * shader() { return _shader; }
    void useShader(PHGLShaderProgram * prog);
    
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
};

#define PHGLVertexPointer _gameManager->vertexPointer
#define PHGLTexCoordPointer _gameManager->texCoordPointer
#define PHGLColorPointer _gameManager->colorPointer
#define PHGLNormalPointer _gameManager->normalPointer

#define PHGLModelView _gameManager->modelViewMatrix
#define PHGLSetModelView _gameManager->setModelViewMatrix
#define PHGLSetColor _gameManager->setColor
#define PHGLHasCapability _gameManager->hasCapability

#endif