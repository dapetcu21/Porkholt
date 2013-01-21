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

PHPositionalVector PHBody::positionInParent(PHDrawableCoordinates * d, PHPositionalVector & p)
{
    return applyMatrices() * p;
}

void PHBody::setAnimationFieldV3(int field, const PHVector3 & v)
{
    switch (field)
    {
        case PHCinematicActor::fieldMove:
            setPosition(v);
            break;
        case PHCinematicActor::fieldScale:
            setScale(v);
            break;
        default:
            break;
    }
}

void PHBody::setAnimationFieldQ(int field, const PHQuaternion & v)
{
    switch (field)
    {
        case PHCinematicActor::fieldRotate:
        {
            setRotation(v);
            PHVector3 u = v.rotationAxis();
            PHLog("%f  %f %f %f", v.rotationAngle(), u.x, u.y, u.z);
            break;
        }
        default:
            break;
    }
}

PHVector3 PHBody::getAnimationFieldV3(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldMove:
            return position();
        case PHCinematicActor::fieldScale:
            return scale();
        default:
            return PH3DOriginPoint;
    }
}

PHQuaternion PHBody::getAnimationFieldQ(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldRotate:
            return rotation();
        default:
            return PHIdentityQuaternion;
    }
}

void PHBody::attachedToGameManager()
{
    actorAttachedToGameManager(gm);
}
