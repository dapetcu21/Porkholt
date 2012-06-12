/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHFont.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHSerialization.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHAnimatorPool.h>

#define uint16From(d) (*((uint16_t*)(d)))
#define uint32From(d) (*((uint32_t*)(d)))
#define uint64From(d) (*((uint64_t*)(d)))
#define uint16At(d,p) uint16From((d)+(p))
#define uint32At(d,p) uint32From((d)+(p))
#define uint64At(d,p) uint64From((d)+(p))

PHFont::PHFont(PHGameManager * gameM, const string & path) : gm(gameM), loaded(false), tex(NULL), dataMutex(new PHMutex)
{
    size_t len;
    uint8_t * d = data = PHFileManager::loadFile(path, len);
    dataRetainCount = 1;
    bool failed = false;
    
    if (len<16 || (strncmp((char*)d, "PRKF", 4)!=0))
        failed = true;
    else
    {
        size_t txc = PHDecode(uint32At(d, 4));
        size_t img;
        if (PHGameManager::interfaceType() == PHGameManager::interfaceHD)
            img = PHDecode(uint32At(d, 12));
        else
            img = PHDecode(uint32At(d, 8));
        failed = !loadGlyphs(d+txc,len-txc);
        failed = failed || !loadImage(d+img,len-img);
        if (!failed) correctTextureCoordinates();
    }
    
    
    if (failed)
    {
        delete [] d;
        throw PHInvalidFileFormat;
    } 
    
    dataMutex->lock();
    dataRetainCount--;
    if (!dataRetainCount)
        delete [] data;
    dataMutex->unlock();
}


bool PHFont::loadGlyphs(uint8_t *d, size_t len)
{
    if (len<2) return false;
    size_t l = PHDecode(uint16At(d, 0));
    if (len<l+14) return false;
    name = string((char*)(d+2),l);
    pntSize = (ph_float)PHDecodeF(uint64At(d, l+2));
    size_t n = PHDecode(uint32At(d, l+10));
    if (len<l+14+n*(8*5+1)) return false;
    uint8_t * p = d+l+14;
    
    for (int i=0; i<n; i++)
    {
        glyph tmp;
        char c = (char)*p; p++; 
        tmp.minX = (ph_float)PHDecodeF(uint64From(p)); p+=8;
        tmp.maxX = (ph_float)PHDecodeF(uint64From(p)); p+=8;
        tmp.minY = (ph_float)PHDecodeF(uint64From(p)); p+=8;
        tmp.maxY = (ph_float)PHDecodeF(uint64From(p)); p+=8;
        tmp.aspectRatio = (ph_float)PHDecodeF(uint64From(p)); p+=8;
        glyphs[c] = tmp;
    }
    return true;
}

void PHFont::correctTextureCoordinates()
{
    for (map<char,glyph>::iterator i = glyphs.begin(); i!=glyphs.end(); i++)
    {
        glyph & g = i->second;
        g.maxX = (floor(g.maxX*width - 0.5) + 0.5)/width;
        g.maxY = (floor(g.maxY*height - 0.5) + 0.5)/height;
        g.minX = (ceil(g.minX*width - 0.5) + 0.5)/width;
        g.minY = (ceil(g.minY*height - 0.5) + 0.5)/height;
        if (g.maxX<0) g.maxX = 0;
        if (g.maxY<0) g.maxY = 0;
        if (g.minX<0) g.minX = 0;
        if (g.minY<0) g.minY = 0;
    }
}

bool PHFont::loadImage(uint8_t *d, size_t len)
{
    if (len<8) return false;
    width = PHDecode(uint32At(d,0));
    height = PHDecode(uint32At(d,4));
    if (len<8+width*height) return false;
    imageData = d+8;
    dataRetainCount++;
    gm->animatorPool()->scheduleAction(PHInv(this, PHFont::loadToTexture, NULL));
    return true;
}

void PHFont::loadToTexture(PHObject * sender, void * ud)
{	
    dataMutex->lock();
    if (!imageData | loaded)
    {
        dataMutex->unlock();
        return;
    }
    loaded = true;
    
    tex = new PHGLTexture2D(gm);
    tex->bind(); 
    tex->setWrapS(PHGLTexture::clampToEdge);
    tex->setWrapT(PHGLTexture::clampToEdge);
    tex->setMinFilter(PHGLTexture::linear);
    tex->setMagFilter(PHGLTexture::linear);
    tex->setData(imageData, width, height, PHGLTexture::A8);
    dataRetainCount--;
    if (dataRetainCount == 0)
        delete [] data;
    dataMutex->unlock();
}

PHFont::~PHFont()
{
    dataMutex->release();
    tex->release();
}
