/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHANIMATEDIMAGE_H
#define PHANIMATEDIMAGE_H

#include <Porkholt/Core/PHImage.h>

class PHImageAnimator;
class PHAnimatorPool;
class PHGameManager;
class PHTextureAtlas;
class PHDirectory;

class PHAnimatedImage: public PHImage
{
private:
    static lua_State * L;
    static PHMutex * luaMutex;
    
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
    PHTextureAtlas * atl;
    
    friend class PHImageAnimator;
    
    virtual void _load();
    
public:
    PHAnimatedImage(PHGameManager * gameManager, PHDirectory * dir);
    virtual ~PHAnimatedImage();
    
    virtual bool isAnimated() { return true; }
    
    int sectionNo(const string & sectionName);

    PHTextureAtlas * atlas() { return atl; }
    void setAtlas(PHTextureAtlas * a);
    
    PHImageAnimator * newAnimator();
    PHImageAnimator * newAnimator(PHAnimatorPool * p);
};

#endif
