/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHFONT_H
#define PHFONT_H

#include <Porkholt/Core/PHMain.h>

class PHGameManager;
class PHGLTexture2D;

class PHFont : public PHObject
{
public:
	
    struct glyph
    {
        ph_float minX,maxX,minY,maxY,aspectRatio;
        glyph() : minX(0), maxX(0), minY(0), maxY(0), aspectRatio(0) {};
    };
    
    
    const string & fontName() { return name; }
    ph_float prerenderPointSize() { return pntSize; }
    
protected:
	PHFont(PHGameManager * gameManager, const string & path);
    ~PHFont();
    friend class PHFontInitPool;
    
    PHGameManager *gm;
    PHGLTexture2D * tex;
    map<char,glyph> glyphs;
    
    bool loadGlyphs(uint8_t * d, size_t len);
    bool loadImage(uint8_t * d, size_t len);
    
    string name;
    ph_float pntSize;
    
    uint8_t * data;
    uint8_t * imageData;
    int dataRetainCount;
    PHMutex * dataMutex;

    size_t width,height;
    
    bool loaded;

    virtual void loadToTexture(PHObject * sender, void * ud);
    void correctTextureCoordinates();
    
public:
    PHGLTexture2D * texture() { if (!loaded) loadToTexture(this,NULL); return tex; }
    const glyph & glyphForCharacter(char c) { return glyphs[c]; }
};

#endif
