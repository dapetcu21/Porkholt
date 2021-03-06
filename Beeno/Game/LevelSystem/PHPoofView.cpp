/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHPoofView.h"
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>


PHImage * PHPoofView::poofImage(PHGameManager * man)
{
    return man->imageNamed("poof");
}

PHImage * PHPoofView::boomImage(PHGameManager * man)
{
    return man->imageNamed("boom");
}

PHPoofView::PHPoofView(const PHRect & frame, int image) : PHImageView(frame), img(image)
{
}

PHPoofView::PHPoofView(int image) : PHImageView(), img(image)
{
}

void PHPoofView::attachedToGameManager()
{
    PHImageView::attachedToGameManager();
    if (image()) return;
    mutex();
    PHImage * imag = NULL;
    switch (img) {
        case poof:
            imag = poofImage(gm);
            break;
        case boom:
            imag = boomImage(gm);
            break;
        default:
            imag = NULL;
            break;
    }
    setImage(imag);
    PHImageAnimator * a = animator();
    if (!a)
        destroy(NULL, NULL);
    else
        a->reset(PHInv(this,PHPoofView::destroy,NULL));
}

void PHPoofView::destroy(PHObject * sender, void * ud)
{
    if (ud)
        removeFromParent();
    else
        gm->mainAnimatorPool()->scheduleAction(PHInv(this, PHPoofView::destroy, (void*)1), false);
}
