/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHNORMALIMAGE_H
#define PHNORMALIMAGE_H

#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHGLTexture.h>

class PHImageView;
class PHGameManager;
class PHGLVertexBufferObject;
class PHGLVertexArrayObject;
class PHFile;

class PHNormalImage : public PHImage
{
private:
    PHRect txc;

    PHFile * fd;
    bool _antialiasing;
    int _keepCount;
    uint8_t * buffer;
    size_t bw,bh,sz;
    enum PHGLTexture::pixelFormat fmt;
    
    PHGLTexture2D * tex;
    
    bool pload;
    
    virtual void _load() { if(!pload) loadFromFile(NULL,NULL); loadToTexture(NULL, NULL); }
public:
    
    PHNormalImage(PHGameManager * gameManager, PHFile * file, bool antialiasing = false, bool keepData = false);
    PHNormalImage(PHGLTexture2D * texture, const PHRect textureCoord);
    virtual ~PHNormalImage();
    
    void bindToTexture(int tx);
    PHRect textureCoordinates() { load(); return txc; }
    void setTextureCoordinates(const PHRect & r) { txc = r; }
    PHRect textureCoordinates(const PHRect & port);

    size_t bufferWidth() { return bw; }
    size_t bufferHeight() { return bh; }
    bool antialiasing() { return _antialiasing; }
    enum PHGLTexture::pixelFormat pixelFormat() { return fmt; }
    uint8_t * imageData() { return buffer; }

    int dataRetainCount() { return _keepCount; }
    void retainData();
    void releaseData();
    
    PHGLTexture2D * texture() { load(); return tex; }
    
    //immediate mode
    void renderInFrame(PHGameManager * gameManager, const PHRect & frm) { renderInFramePortionTint(gameManager,frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(gameManager,frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
    
    //VAOs
    void rebuildVAO(PHImageView * imageView, PHGLVertexArrayObject * & vao);
	
    void loadFromFile(PHObject * sender, void * ud);
	void loadToTexture(PHObject * sender, void * ud); //don't use
    
    virtual bool isNormal() { return true; }
    
    //algos
    static PHPoint * detectOutline(uint8_t * data, size_t xs, size_t ys, size_t w, size_t h, size_t bw, enum PHGLTexture::pixelFormat fmt, float treshold, size_t keypoints);
    PHPoint * detectOutline(float treshold, size_t keypoints) { load(); return detectOutline(buffer, round(txc.x*bw), round(txc.y*bh), _width, _height, bw, fmt, treshold, keypoints); }
};

#endif
