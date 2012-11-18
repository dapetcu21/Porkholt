/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHTransformDrawable.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHGameManager.h>

PHTransformDrawable::PHTransformDrawable() : cache(false), cus(false), rot(PHIdentityQuaternion), tra(0,0,0), sca(1,1,1)
{
}

void PHTransformDrawable::setCustomMatrix(const PHMatrix & m)
{
    custom = m;
    cus = (custom == PHIdentityMatrix);
}

void PHTransformDrawable::render()
{
    PHMatrix o = gm->modelViewMatrix();
    gm->setModelViewMatrix(o * matrix());
    renderChildren();
    gm->setModelViewMatrix(o);
}

const PHMatrix & PHTransformDrawable::matrix()
{
    if (!cache)
    {
        cache = true;
        bool first = true;
                if (tra.x || tra.y || tra.z)
        {
            if (first)
            {
                mat = PHMatrix::translation(tra);
                first = false;
            } else 
                mat *= PHMatrix::translation(tra);
        }
        if (rot != PHIdentityQuaternion)
        {
            if (first)
            {
                mat = rot.rotationMatrix();
                first = false;
            } else
                mat *= rot.rotationMatrix();
        }
        if (sca.x != 1 || sca.y != 1 || sca.y != 1)
        {
            if (first)
            {
                mat = PHMatrix::scaling(sca);
                first = false;
            } else
                mat *= PHMatrix::scaling(sca);
        }
        if (cus)
        {
            if (first)
            {
                mat = custom;
                first = false;
            } else
                mat *= custom;
        }
        if (first)
            mat = PHIdentityMatrix;
    }
    return mat;
}

PHPositionalVector PHTransformDrawable::positionInMyCoordinates(PHDrawableCoordinates * d)
{
    return matrix().inverse() * d->positionInDrawable(parent());
}

PHPositionalVector PHTransformDrawable::positionInParent(PHDrawableCoordinates * d, PHPositionalVector & p)
{
    return matrix() * p;
}

