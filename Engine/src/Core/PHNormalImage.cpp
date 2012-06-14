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

PHNormalImage::PHNormalImage(const string & path, PHGameManager * gameManager): PHImage(path,gameManager), tex(NULL), pload(false)
{
    fd = -1;
    if (PHGameManager::isGloballyHD())
        fd = open((path+".hd").c_str(), O_RDONLY);
    if (fd<0)
        fd = open(path.c_str(), O_RDONLY);
	if (fd<0)
		throw string("Can't open file \"") + path + "\": " + strerror(errno);
    antialiasing = PHFileManager::fileExists(path+".aa");
#ifdef PHIMAGE_ASYNCHRONEOUS_LOADING
    PHThread * thread = new PHThread;
    thread->setFunction(PHInv(this,PHNormalImage::loadFromFile, NULL));
    thread->start();
    thread->release();
#else
    loadFromFile(this, NULL);
#endif  
}

void PHNormalImage::loadFromFile(PHObject *sender, void *ud)
{
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
        buffer = PHGLTexture::dataFromFileDescriptor(fd, w, h, bw, bh, pot, sz, fmt);
        _width  = w;
        _height = h;
    } catch (string ex)
    {
        PHLog("%s", ex.c_str());
        close(fd);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
        loadMutex->unlock();
        buffer = NULL;
        return;
    }
    close(fd);
    pload = true;
    
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->unlock();
#endif
    loadMutex->unlock();
    
    gm->animatorPool()->scheduleAction(PHInv(this, PHNormalImage::loadToTexture, NULL));
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
        r.x				, r.y + r.height,
        r.x + r.width   , r.y + r.height,
		r.x				, r.y,
		r.x + r.width	, r.y
    };
    
    int states = PHGLBlending | PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture0;
    gm->setGLStates(states);
    bindToTexture(0);
    gm->setColor(tint);
    gm->applySpriteShader();
    if (gm->useShaders())
    {
        glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
        glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords);
    } else {
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHNormalImage::rebuildVAO(PHImageView * imageView, PHGLVertexArrayObject * & vao, PHGLVertexBufferObject * & vbo)
{
    if (!vao)
        vao = new PHGLVertexArrayObject(imageView->gameManager());
    if (!vbo)
        vbo = new PHGLVertexBufferObject(imageView->gameManager());
    PHImage::buildImageVAO(vao,
                           vbo,
                           PHPoint(imageView->repeatX(),imageView->repeatY()),
                           imageView->textureCoordinates(),
                           txc,
                           PHOriginPoint
                           );
}
