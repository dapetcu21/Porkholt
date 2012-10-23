/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPARTICLEANIMATOR_H
#define PHPARTICLEANIMATOR_H

#include <Porkholt/Core/PHAnimator.h>

class PHParticleAnimator : public PHAnimator
{
    public:
        struct particle2D
        {
            PH3DPoint position;
            ph_float rotation;
            PHSize size;
            PHColor color;
            void * ud;
        };

        struct particle3D
        {
            PH3DPoint position;
            PHQuaternion rotation;
            PH3DSize size;
            PHColor color;
            void * ud;
        };

        enum particleType
        {
            particleType2D,
            particleType3D,
            particleTypeCollection
        };
        
        struct particles
        {
            int type;
            size_t n;
            union {
                particle2D * v2d;
                particle3D * v3d;
                particles * vcol;
            };
        };

        virtual void advanceAnimation(ph_float elapsedTime) = 0;
        virtual particles * calculatedParticles() = 0;
};

#endif
