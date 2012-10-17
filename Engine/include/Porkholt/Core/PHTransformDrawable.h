/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHTRANSFORMDRAWABLE_H
#define PHTRANSFORMDRAWABLE_H

#include <Porkholt/Core/PHDrawable.h>

class PHTransformDrawable : public PHDrawable 
{
    protected:
        PHMatrix mat, custom;
        bool cache, cus;
        PHQuaternion rot;
        PH3DPoint tra;
        PHVector3 sca;

    public:
        PHTransformDrawable();
        const PHMatrix & matrix();
        void render();

        void setCustomMatrix(const PHMatrix & m);
        void setTranslation(const PH3DPoint & p) { tra = p; cache = false; }
        void setRotation(const PHQuaternion & q) { rot = q; cache = false; }
        void setScale(const PHVector3 & s) { sca = s; cache = false; }

        const PH3DPoint & translation() { return tra; }
        const PHVector3 & scale() { return sca; }
        const PHQuaternion & rotation() { return rot; }
        const PHMatrix & customMatrix() { return custom; }
};

#endif
