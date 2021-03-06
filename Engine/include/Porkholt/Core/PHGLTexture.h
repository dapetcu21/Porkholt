/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLTEXTURE_H
#define PHGLTEXTURE_H

#include <Porkholt/Core/PHGLFBOAttachment.h>

class PHGameManager;
class PHStream;

class PHGLTexture : public PHGLFBOAttachment
{
protected:
    GLuint tex;
    PHGameManager * gm;
    friend class PHGLFramebuffer;
    
public:
    GLuint name() { return tex; }
    
    PHGLTexture(PHGameManager * gameManager, GLenum target);
    PHGLTexture(PHGameManager * gameManager, GLenum target, GLuint tex); //already initialized texture
    ~PHGLTexture();
    
    enum wrapType
    {
        clamp = 0,
        clampToBorder,
        clampToEdge,
        mirroredRepeat,
        repeat
    };
    
    enum aaFilter 
    {
        nearest,
        linear,
        nearestMipmapNearest,
        nearestMipmapLinear,
        linearMipmapNearest,
        linearMipmapLinear
    };
       
protected:
    enum wrapType wS, wT, wR;
    enum aaFilter min, mag;
    bool mipMap;
    bool bound;
    
    friend class PHGameManager;
    
    void _bind();
    static GLenum openGLEnumFromWrapType(enum wrapType t);
    static GLenum openGLEnumFromAAFilter(enum aaFilter t);
    GLenum target;
    
public:
    void bind();
    void bind(int tmu);
    
#ifdef GL_TEXTURE_WRAP_S
    enum wrapType wrapS() { return wS; }
    void setWrapS(enum wrapType t);
#endif
#ifdef GL_TEXTURE_WRAP_T
    enum wrapType wrapT() { return wT; }
    void setWrapT(enum wrapType t);
#endif
#ifdef GL_TEXTURE_WRAP_R
    enum wrapType swapR() { return wR; }
    void setWrapR(enum wrapType t);
#endif

    void setMinFilter(enum aaFilter t);
    void setMagFilter(enum aaFilter t);
    enum aaFilter minFilter() { return min; }
    enum aaFilter magFilter() { return mag; }
    
    bool hasMipMap() { return mipMap; }
    void updateMipMap();
    
    static uint8_t * dataFromFile(const string & file, size_t & width, size_t & height, size_t & bufWidth, size_t & bufHeight, bool powerOfTwo, size_t & size, enum pixelFormat & format);
    static uint8_t * dataFromFile(PHStream * fd, size_t & width, size_t & height, size_t & bufWidth, size_t & bufHeight, bool powerOfTwo, size_t & size, enum pixelFormat & format);
    
    static bool supportsNPOT(PHGameManager * gm);

    PHGameManager * gameManager() { return gm; }
};

#ifdef GL_TEXTURE_1D
class PHGLTexture1D : public PHGLTexture
{
private:
    size_t w;
public:
    PHGLTexture1D(PHGameManager * gm) : PHGLTexture(gm, GL_TEXTURE_1D) {}
    PHGLTexture1D(PHGameManager * gm, GLuint tex, size_t _w) : PHGLTexture(gm, GL_TEXTURE_1D, tex), w(_w) {}
    size_t width() { return w; }
    void setData(uint8_t * data, size_t width, enum pixelFormat format);
};
#endif

#ifdef GL_TEXTURE_2D
class PHGLTexture2D : public PHGLTexture
{
private:
    size_t w,h;
public:
    PHGLTexture2D(PHGameManager * gm) : PHGLTexture(gm, GL_TEXTURE_2D) {}
    PHGLTexture2D(PHGameManager * gm, GLuint tex, size_t _w, size_t _h) : PHGLTexture(gm, GL_TEXTURE_2D, tex), w(_w), h(_h) {}
                                        
    void setData(uint8_t * data, size_t width, size_t height, enum pixelFormat format);
    size_t width() { return w; }
    size_t height() { return h; }
    PHRect loadFromFile(const string & fname, bool antialiasing = false);
    PHRect loadFromFile(PHStream * fd, bool antialiasing = false);
    
    PHRect loadFromData(uint8_t * buf, size_t w, size_t h, size_t bw, size_t bh, enum pixelFormat fmt, bool aa);
};
#endif

#ifdef GL_TEXTURE_3D
class PHGLTexture3D : public PHGLTexture
{
private:
    size_t w,h,d;
public:
    PHGLTexture3D(PHGameManager * gm) : PHGLTexture(gm, GL_TEXTURE_3D) {}
    PHGLTexture3D(PHGameManager * gm, GLuint tex, size_t _w, size_t _h, size_t _d) : PHGLTexture(gm, GL_TEXTURE_3D, tex), w(_w), h(_h), d(_d) {}
    
    size_t width() { return w; }
    size_t height() { return h; }
    size_t depth() { return d; }
    void setData(uint8_t * data, size_t width, size_t height, size_t depth, enum pixelFormat format);
};
#endif

#ifdef GL_TEXTURE_CUBE_MAP
class PHGLTextureCubeMap : public PHGLTexture
{
public:
    enum cubemapFace
    {
        positiveX,
        negativeX,
        positiveY,
        negativeY,
        positiveZ,
        negativeZ
    };
    
    PHGLTextureCubeMap(PHGameManager * gm) : PHGLTexture(gm, GL_TEXTURE_CUBE_MAP) {}
    PHGLTextureCubeMap(PHGameManager * gm, GLuint tex) : PHGLTexture(gm, GL_TEXTURE_CUBE_MAP, tex) {}
 
    void setData(uint8_t * data, size_t width, size_t height, enum cubemapFace face, enum pixelFormat format);
};
#endif

#endif
