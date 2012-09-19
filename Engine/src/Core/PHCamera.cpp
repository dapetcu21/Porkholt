/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCamera.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

void PHCamera::render()
{
    PHMatrix om = gm->modelViewMatrix();
    PHMatrix op = gm->projectionMatrix();
    if (im)
        gm->setProjectionMatrix(projection());
    else
        gm->setProjectionMatrix(op * om * projection());
    gm->setModelViewMatrix(PHIdentityMatrix);

    renderChildren();

    gm->setModelViewMatrix(om);
    gm->setProjectionMatrix(op);
}

void PHCamera::attachedToGameManager()
{
    gm->messageWithName("reshapeWindow")->addListener(PHInvN(this, PHCamera::_reshape));
}

void PHCamera::_reshape()
{
    reshape();
}

PHCamera::PHCamera() : im(true)
{
    setUserInput(true);
}

PHCamera::~PHCamera()
{
    if (gm)
        gm->messageWithName("reshapeWindow")->removeListener(this);
}

PHPositionalVector PHCamera::positionInMyCoordinates(PHDrawableCoordinates * c)
{
    return projection().inverse() * c->positionInDrawable(im?NULL:parent());
}
