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
class PHCurve;
class PHGameManager;
class PHGLVertexArrayObject;
class PHGLVertexBufferObject;

class PHImage : public PHObject
{
public:
#ifdef PHIMAGE_ORDERED_LOADING
    static PHMutex * loadingMutex;
#endif
	
protected:
	PHImage(const string & path, PHGameManager * gameManager);
	
	int _height;
    int _width;
    
    bool loaded;
    virtual void _load() = 0;
    
    PHGameManager * gm;
    
    PHImage * _normalMap;
    PHMutex * loadMutex;
    
    friend class PHImageView;
    friend class PHImageAnimator;
    friend class PHImageInitPool;
    
    static void buildImageVAO(PHGLVertexArrayObject * vao, PHGLVertexBufferObject * vbo, const PHPoint & repeat, const PHRect & portion, const PHRect & texCoord, const PHPoint & adjustment);
    
public:
    void load() { loadMutex->lock(); /*PHLog("loadMutex lock l");*/ if (!loaded) _load(); /*PHLog("loadMutex unlock l");*/ loadMutex->unlock(); }
	int height() { load(); return _height; };
	int width() { load(); return _width; };

    virtual bool isNormal() { return false; };
    virtual bool isAnimated() { return false; };
    
    PHImage * normalMap() { return _normalMap; } 
    
    virtual ~PHImage() { loadMutex->release(); if (_normalMap) _normalMap->release(); }
};

#endif