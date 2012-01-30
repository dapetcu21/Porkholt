//
//  PHPoofView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHPoofView.h"
#include "PHImageAnimator.h"


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

void PHPoofView::setGameManager(PHGameManager * gm)
{
    PHView::setGameManager(gm);
    if (gm && !image())
    {
        init(img);
    }
}

void PHPoofView::init(int image)
{
    mutex();
    PHImage * img = NULL;
    switch (image) {
        case poof:
            img = poofImage(_gameManager);
            break;
        case boom:
            img = boomImage(_gameManager);
            break;
        default:
            img = NULL;
            break;
    }
    setImage(img);
    PHImageAnimator * a = animator();
    if (!a)
        destroy(NULL, NULL);
    else
        a->reset(PHInv(this,PHPoofView::destroy,NULL));
}

void PHPoofView::destroy(PHObject * sender, void * ud)
{
    if (ud)
        removeFromSuperview();
    else
        PHThread::mainThread()->scheduleOnThread(PHInv(this, PHPoofView::destroy, (void*)1), false);
}