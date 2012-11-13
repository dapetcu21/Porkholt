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
    bool antialiasing;
    uint8_t * buffer;
    size_t w,h,bw,bh,sz;
    enum PHGLTexture::pixelFormat fmt;
    
    PHGLTexture2D * tex;
    
    bool pload;
    
    virtual void _load() { if(!pload) loadFromFile(NULL,NULL); loadToTexture(NULL, NULL); }
public:
    
    PHNormalImage(PHGameManager * gameManager, PHFile * file, bool antialiasing);
    PHNormalImage(PHGLTexture2D * texture, const PHRect textureCoord);
    virtual ~PHNormalImage();
    
    void bindToTexture(int tx);
    PHRect textureCoordinates() { load(); return txc; }
    PHRect textureCoordinates(const PHRect & port);
    
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
    
};

#endif
