/*
 *  PHImage.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */
#ifndef PHIMAGE_H
#define PHIMAGE_H
#include "PHMain.h"

#define PHIMAGE_ASYNCHRONEOUS_LOADING
#define PHIMAGE_ORDERED_LOADING

class PHImage;
#include <map>
class PHBezierPath;


class PHImage : public PHObject
{
public:
#ifdef PHIMAGE_ORDERED_LOADING
    static PHMutex * loadingMutex;
#endif
	
protected:
	PHImage(const string & path);
	
	int _height;
    int _width;
    
    bool loaded;
    virtual void _load() = 0;
    
    struct VBOParams
    {
        int nElements;
        size_t vertexOffset,texCoordOffset;
        GLint vertexSize,texCoordSize;
        GLenum vertexType, texCoordType, renderMode;
        GLsizei vertexStride,texCoordStride;
    };
    friend class PHImageView;
    friend class PHImageAnimator;
    
    static void buildImageVBO(GLuint vbo, VBOParams & params, const PHPoint & repeat, const PHRect & portion, const PHRect & texCoord, const PHPoint & adjustment);
    
public:
    void load() { if (!loaded) _load(); }
	int height() { load(); return _height; };
	int width() { load(); return _width; };

    virtual bool isNormal() { return false; };
    virtual bool isAnimated() { return false; };
    
};

#endif