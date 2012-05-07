//
//  PHAnimatedImage.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/9/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHANIMATEDIMAGE_H
#define PHANIMATEDIMAGE_H

#include "PHImage.h"

class PHImageAnimator;
class PHAnimatorPool;
class PHGameManager;

class PHAnimatedImage: public PHImage
{
private:
    static lua_State * L;
    static PHMutex * luaMutex;
    
    string path;
    
    void loadFromLua();
    void cleanupLua();
    
    struct frame
    {
        int frameno;
        int type;
        union
        {
            ph_float duration;
            int fnumber;
            string * sectionName;
            int section;
        };
        bool luaDuration;
        bool fade;
    };
    
    struct section
    {
        string name;
        vector<PHAnimatedImage::frame> frames;
    };
    
    vector<PHAnimatedImage::section *> sections;
    int defaultSection;
    
    struct texture
    {
        unsigned int texid;
        uint8_t * buffer;
        size_t size;
        size_t rowsize;
        size_t awidth,aheight;
        texture() : texid(-1), buffer(NULL) {};
    };
    
    vector<PHAnimatedImage::texture> textures;
    int nfrm;
    
    
    bool antialiasing;
    uint8_t clr,bdepth;
    size_t buffersize;
    int cols,rows,ipt;
    
    bool pload;
    
    friend class PHImageAnimator;
    
    virtual void _load() { if(!pload) loadImages(NULL,NULL); loadTextures(NULL, NULL);  }
    
public:
    PHAnimatedImage(const string & s, PHGameManager * gameManager);
    virtual ~PHAnimatedImage();
    
    virtual bool isAnimated() { return true; }
    
    void loadImages(PHObject * sender, void * ud);
    void loadTextures(PHObject * sender, void * ud);
    
    int sectionNo(const string & sectionName);
    
    PHImageAnimator * newAnimator();
    PHImageAnimator * newAnimator(PHAnimatorPool * p);
};

#endif