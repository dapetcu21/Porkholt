/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHCAMERA_H
#define PHCAMERA_H

#include <Porkholt/Core/PHDrawable.h>

class PHCamera : public PHDrawable 
{
    public:
        virtual PHMatrix projection() = 0;
        virtual void reshape() = 0;
        ~PHCamera();
        PHCamera() : im(false) {};

        void setIgnoresMatrices(bool i) { im = i; }
        bool ignoresMatrices() { return im; }

    protected:
        bool im;
        void _reshape();
        void attachedToGameManager();
        virtual void render();
};

#endif
