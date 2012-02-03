//
//  PHFont.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHFONT_H
#define PHFONT_H

#include "PHMain.h"

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
	PHFont(const string & path);
    friend class PHFontInitPool;
    
    unsigned int texID;
    map<char,glyph> glyphs;
    
    bool loadGlyphs(uint8_t * d, size_t len);
    bool loadImage(uint8_t * d, size_t len);
    
    string name;
    ph_float pntSize;
    
    uint8_t * data;
    uint8_t * imageData;
    int dataRetainCount;
    
    size_t width,height;
    
    bool loaded;

    virtual void loadToTexture(PHObject * sender, void * ud);
    void correctTextureCoordinates();
    
public:
    unsigned int textureID() { if (!loaded) loadToTexture(this,NULL); return texID; }
    const glyph & glyphForCharacter(char c) { return glyphs[c]; }
};

#endif