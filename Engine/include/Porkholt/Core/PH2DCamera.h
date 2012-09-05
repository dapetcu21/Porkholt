/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PH2DCAMERA_H
#define PH2DCAMERA_H

#include <Porkholt/Core/PHCamera.h>

class PH2DCamera : public PHCamera 
{
    private:
        PHMatrix mat;
        bool cache;
        PHSize sz;

   public:
        PH2DCamera();
        void setScreenSize(const PHSize & s) { sz = s; cache = false; }
        void reshape();    
        PHMatrix projection();
};

#endif
