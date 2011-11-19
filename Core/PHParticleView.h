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
class PHParticleView : public PHImageView
{
public:
    PHParticleView();
    PHParticleView(const PHRect &frame);
    PHParticleView(PHImage * image);
    
    virtual ~PHParticleView();
    virtual void render();
    
};


#endif
