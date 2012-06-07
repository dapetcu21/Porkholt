/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHNORMALIMAGE_H
#define PHNORMALIMAGE_H

#include "PHImage.h"
#include "PHGLTexture.h"

class PHImageView;
class PHGameManager;
class PHGLVertexBufferObject;
class PHGLVertexArrayObject;
class PHNormalImage : public PHImage
{
private:
    PHRect txc;
    int fd;
    bool antialiasing;
    uint8_t * buffer;
    size_t w,h,bw,bh,sz;
    enum PHGLTexture::pixelFormat fmt;
    
    PHGLTexture2D * tex;
    
    bool pload;
    
    virtual void _load() { if(!pload) loadFromFile(NULL,NULL); loadToTexture(NULL, NULL); }
public:
    
    PHNormalImage(const string & path, PHGameManager * gameManager);
    virtual ~PHNormalImage();
    
    void bindToTexture(int tx);
    PHRect textureCoordinates() { return txc; }
    PHRect textureCoordinates(const PHRect & port);
    
    PHGLTexture2D * texture() { load(); return tex; }
    
    //immediate mode
    void renderInFrame(PHGameManager * gameManager, const PHRect & frm) { renderInFramePortionTint(gameManager,frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(gameManager,frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
    
    //VAOs
    void rebuildVAO(PHImageView * imageView, PHGLVertexArrayObject * & vao, PHGLVertexBufferObject * & vbo);
	
    void loadFromFile(PHObject * sender, void * ud);
	void loadToTexture(PHObject * sender, void * ud); //don't use
    
    virtual bool isNormal() { return true; }
    
};

#endif
