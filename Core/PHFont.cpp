//
//  PHFont.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHFont.h"
#include "PHFileManager.h"
#include "PHSerialization.h"
#include "PHGameManager.h"

map<string,PHFont*> PHFont::fonts;

#define uint16From(d) (*((uint16_t*)(d)))
#define uint32From(d) (*((uint32_t*)(d)))
#define uint64From(d) (*((uint64_t*)(d)))
#define uint16At(d,p) uint16From((d)+(p))
#define uint32At(d,p) uint32From((d)+(p))
#define uint64At(d,p) uint64From((d)+(p))

PHFont::PHFont(const string & path) : loaded(false), texID(-1)
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
    
    dataRetainCount--;
    if (!dataRetainCount)
        delete [] data;
}


bool PHFont::loadGlyphs(uint8_t *d, size_t len)
{
    if (len<2) return false;
    size_t l = PHDecode(uint16At(d, 0));
    if (len<l+14) return false;
    name = string((char*)(d+2),l);
    pntSize = PHDecodeF(uint64At(d, l+2));
    size_t n = PHDecode(uint32At(d, l+10));
    if (len<l+14+n*(8*5+1)) return false;
    uint8_t * p = d+l+14;
    
    for (int i=0; i<n; i++)
    {
        glyph tmp;
        char c = (char)*p; p++; 
        tmp.minX = PHDecodeF(uint64From(p)); p+=8;
        tmp.maxX = PHDecodeF(uint64From(p)); p+=8;
        tmp.minY = PHDecodeF(uint64From(p)); p+=8;
        tmp.maxY = PHDecodeF(uint64From(p)); p+=8;
        tmp.aspectRatio = PHDecodeF(uint64From(p)); p+=8;
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
    PHThread::mainThread()->executeOnThread(PHInv(this, PHFont::loadToTexture, NULL), false);
    return true;
}

void PHFont::loadToTexture(PHObject * sender, void * ud)
{	
    if (!imageData) return;
    if (loaded) return;
    loaded = true;
    
	glGenTextures(1,&texID);
    glBindTexture(GL_TEXTURE_2D, texID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (int)width, (int)height, 0, 
				 GL_ALPHA, GL_UNSIGNED_BYTE, imageData);	
    
    dataRetainCount--;
    if (dataRetainCount == 0)
        delete [] data;
}

PHFont* PHFont::fontFromPath(const string & path)
{
    PHFont * fnt;
	map<string,PHFont*>::iterator i = fonts.find(path);
	if (i==fonts.end())
	{
		try {
            fnt = new PHFont(path);
		} catch (string ex)
		{
			PHLog(ex);
			return NULL;
		}
		fonts[path] = fnt;
	} else {
		fnt = fonts[path];
	}
	return fnt;
}

PHFont* PHFont::fontNamed(const string & name)
{
    return PHFont::fontFromPath(PHFileManager::resourcePath()+"/fnt/"+name+".phf");
}

void PHFont::collectGarbage()
{
    map<string,PHFont*>::iterator i;
	map<string,PHFont*> tmp;
	for (i = fonts.begin(); i!=fonts.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	fonts = tmp;
}