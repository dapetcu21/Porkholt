/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHPERSPECTIVECAMERA_H
#define PHPERSPECTIVECAMERA_H

#include <Porkholt/Core/PHCamera.h>

class PHPerspectiveCamera : public PHCamera 
{
    private:
        PHMatrix mat;
        bool cache;
        ph_float ar;
        ph_float near,far;
        ph_float fov;

    public:
        PHPerspectiveCamera();

        ph_float aspectRatio() { return ar; }
        void setAspectRatio(ph_float aspect) { ar = aspect; cache = false; } 
        ph_float nearClippingPlane() { return near; }
        ph_float farClippingPlane() { return far; }
        void setNearClippingPlane(ph_float n) { near = n; cache = false; }
        void setFarClippingPlane(ph_float f) { far = f; cache = false; }
        ph_float fieldOfView() { return fov; }
        void setFieldOfView(ph_float f) { fov = f; cache = false; }

        void reshape();    
        PHMatrix projection();

};

#endif
