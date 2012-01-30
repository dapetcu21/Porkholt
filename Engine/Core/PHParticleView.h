//
//  PHParticleView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/18/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHPARTICLEVIEW_H
#define PHPARTICLEVIEW_H

#include "PHImageView.h"

class PHParticleAnimator;
class PHAnimatorPool;
class PHParticleView : public PHImageView
{
private:
    PHAnimatorPool * particlePool;
    PHParticleAnimator * particleAnim;
    PHMutex * particleM;
    
    void renderParticles(void * particles, const PHRect & texCoord, const PHColor & tint);
    
public:
    PHParticleView();
    PHParticleView(const PHRect &frame);
    PHParticleView(PHImage * image);
    void init();
    
    virtual ~PHParticleView();
    virtual void render();
    
    PHAnimatorPool * particleAnimatorPool() { return particlePool; }
    PHParticleAnimator * particleAnimator() { return particleAnim; }
    void setParticleAnimator(PHParticleAnimator * anim );
    void setParticleAnimatorPool(PHAnimatorPool * animPool );
    
    virtual void loadFromLua(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
};


#endif
