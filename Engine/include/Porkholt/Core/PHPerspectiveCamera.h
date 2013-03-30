/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHPERSPECTIVECAMERA_H
#define PHPERSPECTIVECAMERA_H

#include <Porkholt/Core/PHCamera.h>

class PHPerspectiveCamera : public PHCamera 
{
    private:
        PHMatrix mat;
        bool cache;
        float ar;
        float near,far;
        float fov;
        PHQuaternion rot;

    public:
        PHPerspectiveCamera();

        float aspectRatio() { return ar; }
        void setAspectRatio(float aspect) { ar = aspect; cache = false; } 
        float nearClippingPlane() { return near; }
        float farClippingPlane() { return far; }
        void setNearClippingPlane(float n) { near = n; cache = false; }
        void setFarClippingPlane(float f) { far = f; cache = false; }
        float fieldOfView() { return fov; }
        void setFieldOfView(float f) { fov = f; cache = false; }
        PHQuaternion rotation() { return rot; }
        void setRotation(const PHQuaternion & q) { rot = q; cache = false; }

        void reshape();    
        PHMatrix projection();

};

#endif
