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

class PHAnimatedImage: public PHImage
{
private:
    static lua_State * L;
    static PHMutex * luaMutex;
    PHThread * thread;
    
    string path;
    
    void loadFromLua();
    void cleanupLua();
    
    struct frame
    {
        int frameno;
        int type;
        union
        {
            double duration;
            int fnumber;
            string * sectionName;
            int section;
        };
        bool luaDuration;
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
        int awidth,aheight;
        texture() : texid(-1), buffer(NULL) {};
    };
    
    vector<PHAnimatedImage::texture> textures;
    int nfrm;
    
    
    bool antialiasing;
    uint8_t clr,bdepth;
    size_t buffersize;
    int cols,rows,ipt;
    
    friend class PHImageAnimator;
    
    virtual void _load() { loadImages(NULL,NULL); }
    
public:
    PHAnimatedImage(const string & s);
    virtual ~PHAnimatedImage();
    
    virtual bool isAnimated() { return true; }
    
    void loadImages(PHObject * sender, void * ud);
    void loadTextures(PHObject * sender, void * ud);
    
    int sectionNo(const string & sectionName);
    
    PHImageAnimator * newAnimator();
};

#endif