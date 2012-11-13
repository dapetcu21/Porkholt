/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHIMAGE_H
#define PHIMAGE_H
#include <Porkholt/Core/PHMain.h>

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
	PHImage(PHGameManager * gameManager);
	
	int _height;
    int _width;
    
    volatile bool loaded;
    virtual void _load() = 0;
    
    PHGameManager * gm;
    
    PHImage * _normalMap;
    PHMutex * loadMutex;
    
    friend class PHImageView;
    friend class PHImageAnimator;
    friend class PHImageInitPool;
    
    static void buildImageVAO(PHGLVertexArrayObject * vao, const PHPoint & repeat, const PHRect & portion, const PHRect & texCoord);
    
public:
    void load(); 
	int height() { load(); return _height; };
	int width() { load(); return _width; };

    virtual bool isNormal() { return false; };
    virtual bool isAnimated() { return false; };
    
    PHImage * normalMap() { return _normalMap; } 
    void setNormalMap(PHImage * n)
    {
        if (n)
            n->retain();
        if (_normalMap)
            _normalMap->release();
        _normalMap = n;
    }
    
    virtual ~PHImage(); 
};

#endif
