/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHNormalImage.h>
#include <errno.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHThreading.h>

PHNormalImage::PHNormalImage(PHGLTexture2D * texture, const PHRect textureCoord) : PHImage(texture->gameManager()), txc(textureCoord), tex(texture), pload(true)
{
    tex->retain();
    loaded = true;
}

PHNormalImage::PHNormalImage(PHGameManager * gameManager, PHFile * file, bool aa): PHImage(gameManager), tex(NULL), pload(false)
{
    fd = file;
    if (!fd)
        throw string("The fuck?");
    fd->retain();
    antialiasing = aa;
#ifdef PHIMAGE_ASYNCHRONEOUS_LOADING
    PHThread::detachThread(PHInv(this,PHNormalImage::loadFromFile, NULL));
#else
    loadFromFile(this, NULL);
#endif  
}

void PHNormalImage::loadFromFile(PHObject *sender, void *ud)
{
    if (!fd) return;

    loadMutex->lock();
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->lock();
#endif
    
    if (pload)
    {
        loadMutex->unlock();
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
        return;
    }
    
    bool pot = !PHGLTexture::supportsNPOT(gm);
    
    try {
        buffer = PHGLTexture::dataFromFile(fd, w, h, bw, bh, pot, sz, fmt);
        _width  = w;
        _height = h;
    } catch (string ex)
    {
        PHLog("%s", ex.c_str());
        fd->release();
        fd = NULL;
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
        loadMutex->unlock();
        buffer = NULL;
        return;
    }
    fd->release();
    fd = NULL;
    pload = true;
    
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->unlock();
#endif
    loadMutex->unlock();
    
    gm->mainAnimatorPool()->scheduleAction(PHInv(this, PHNormalImage::loadToTexture, NULL));
}

void PHNormalImage::loadToTexture(PHObject * sender, void * ud)
{	
    loadMutex->lock();
    if (loaded) {
        loadMutex->unlock();
        return;
    }
    
    tex = new PHGLTexture2D(gm);
    txc = tex->loadFromData(buffer, w, h, bw, bh, fmt, antialiasing);
	delete[] buffer;
    loaded = true;
    
    loadMutex->unlock();
}

PHNormalImage::~PHNormalImage()
{
    if (fd)
        fd->release();
    if (tex)
        tex->release();
}

void PHNormalImage::bindToTexture(int tx)
{
    if (!tex) return;
    gm->setActiveTexture(tx);
    tex->bind();
}

PHRect PHNormalImage::textureCoordinates(const PHRect & port)
{
    load();
    return PHRect(txc.x + txc.width*port.x,
                  txc.y + txc.height*port.y,
                  txc.width * port.width,
                  txc.height * port.height);
}

void PHNormalImage::renderInFramePortionTint(PHGameManager * gm, const PHRect & frm,const PHRect & port,const PHColor & tint)
{
    load();
    
	const GLfloat squareVertices[] = {
        frm.x,			frm.y,
        frm.x+frm.width,frm.y,
        frm.x,			frm.y+frm.height,
        frm.x+frm.width,frm.y+frm.height,
    };
    
    PHRect r = textureCoordinates(port);
	
	const GLfloat squareTexCoords[] = {
		r.x				, r.y,
		r.x + r.width	, r.y,
        r.x				, r.y + r.height,
        r.x + r.width   , r.y + r.height
    };
    
    int states = PHGLBlending | PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture0;
    gm->setGLStates(states);
    bindToTexture(0);
    gm->setColor(tint);
    gm->applySpriteShader();
    if (gm->useShaders())
    {
        PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
        PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords);
    } else {
        PHGL::glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        PHGL::glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    }
    PHGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHNormalImage::rebuildVAO(PHImageView * imageView, PHGLVertexArrayObject * & vao)
{
    if (!vao)
        vao = new PHGLVertexArrayObject(imageView->gameManager());
    PHImage::buildImageVAO(vao,
                           PHPoint(imageView->repeatX(),imageView->repeatY()),
                           imageView->textureCoordinates(),
                           txc
                           );
}
