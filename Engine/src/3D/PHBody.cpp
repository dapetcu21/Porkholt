/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/3D/PHBody.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

PHBody::PHBody() : pos(PH3DOriginPoint), rot(PHIdentityQuaternion), scl(PH3DUnitSize), _rotationalCenter(PH3DOriginPoint), _scalingCenter(PH3DOriginPoint), effOrder(PHBody::EffectOrderRotateScale), matrixCached(false), effectCached(false)
{
    
}

PHBody::~PHBody()
{

}

void PHBody::setPosition(const PH3DPoint & p)
{
    pos = p;
    matrixCached = false;
}

void PHBody::setRotation(const PHQuaternion & r)
{
    rot = r;
    effectCached = false;
}

void PHBody::setScale(const PH3DSize & s)
{
    scl = s;
    effectCached = false;
}

PHMatrix PHBody::applyMatrices()
{
    if (matrixCached && effectCached)
        return matrixCache;
    if (!effectCached)
    {
        effectCached = true;
        effectCache = PHIdentityMatrix;
        int eo = effOrder;
        while (eo)
        {
            switch (eo&3)
            {
                case EffectRotate:
                {
                    if (rot == PHIdentityQuaternion) break; 
                    rot.normalize();
                    if (_rotationalCenter == PH3DOriginPoint)
                        effectCache *= rot.rotationMatrix();
                    else
                        effectCache *= PHMatrix::translation(_rotationalCenter) * rot.rotationMatrix() * PHMatrix::translation(-_rotationalCenter);
                    break;
                }
                case EffectScale:
                {
                    if (scl == PH3DUnitSize) break;
                    if (_scalingCenter == PH3DOriginPoint)
                        effectCache *= PHMatrix::scaling(scl);
                    else
                        effectCache *= PHMatrix::translation(_scalingCenter) * PHMatrix::scaling(scl) * PHMatrix::translation(-_scalingCenter);                    
                    break;
                }
                case EffectCustom:
                {
                    effectCache *= customEffect;
                    break;
                }
            }
            eo>>=2;
        }
    }
    matrixCached = true;
    matrixCache = PHMatrix::translation(pos) * effectCache;
    return matrixCache;
}

void PHBody::render()
{
    PHMatrix o = gm->modelViewMatrix();
    gm->setModelViewMatrix(o * applyMatrices());
    draw();
    renderChildren();
    gm->setModelViewMatrix(o);
}

PHPositionalVector PHBody::positionInMyCoordinates(PHDrawableCoordinates * d)
{
    return applyMatrices().inverse() * d->positionInDrawable(parent());
}

