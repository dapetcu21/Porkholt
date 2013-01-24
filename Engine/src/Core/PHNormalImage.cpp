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
#include <Porkholt/Core/PHGLUniformStates.h>

PHNormalImage::PHNormalImage(PHGLTexture2D * texture, const PHRect textureCoord) : PHImage(texture->gameManager()), txc(textureCoord), tex(texture), pload(true), _keepCount(0)
{
    tex->retain();
    loaded = true;
    bw = tex->width();
    bh = tex->height();
    _width = round(bw * txc.width);
    _height = round(bh * txc.height);
    _antialiasing = tex->minFilter() == PHGLTexture::linearMipmapNearest;
    fmt = tex->format();
}

PHNormalImage::PHNormalImage(PHGameManager * gameManager, PHFile * file, bool aa, bool keep): PHImage(gameManager), tex(NULL), pload(false), _antialiasing(aa), _keepCount(keep != false)
{
    fd = file;
    if (!fd)
        throw string("The fuck?");
    fd->retain();
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
        buffer = PHGLTexture::dataFromFile(fd, _width, _height, bw, bh, pot, sz, fmt);
        retainData();
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
    txc = tex->loadFromData(buffer, _width, _height, bw, bh, fmt, _antialiasing);
    _antialiasing = tex->minFilter() == PHGLTexture::linearMipmapNearest;
    releaseData();
    loaded = true;
    
    loadMutex->unlock();
}

void PHNormalImage::retainData()
{
    _keepCount++;
}

void PHNormalImage::releaseData()
{
    if (_keepCount <= 0) return;
    if (!--_keepCount)
        delete[] buffer;
}

PHNormalImage::~PHNormalImage()
{
    if (fd)
        fd->release();
    if (tex)
        tex->release();
    if (_keepCount)
        delete[] buffer;
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
    
    gm->updateMatrixUniform();
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
    gm->setTextureUniform(texture());
    gm->setColor(tint);
    gm->updateColorUniform();
    gm->spriteUniformStates()->apply(gm->spriteShader());
    if (gm->useShaders())
    {
        PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
        PHGL::glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords);
    } else {
        PHGL::glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        PHGL::glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    }
    PHGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gm->setTextureUniform(NULL);
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


PHPoint * PHNormalImage::detectOutline(uint8_t * data, size_t xs, size_t ys, size_t w, size_t h, size_t bw, enum PHGLTexture::pixelFormat fmt, ph_float treshold, size_t keypoints)
{
   struct pnt
   {
       size_t x,y;
   };
   return NULL;
}
