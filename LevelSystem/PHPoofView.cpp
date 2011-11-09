//
//  PHPoofView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHPoofView.h"
#include "PHImageAnimator.h"


PHImage * PHPoofView::poofImg = NULL;
PHImage * PHPoofView::boomImg = NULL;

PHImage * PHPoofView::poofImage()
{
    if (!poofImg)
    {
        poofImg = PHImage::imageNamed("poof");
        poofImg->retain();
    }
    return poofImg;
}

PHImage * PHPoofView::boomImage()
{
    if (!boomImg)
    {
        boomImg = PHImage::imageNamed("boom");
        boomImg->retain();
    }
    return boomImg;
}

PHPoofView::PHPoofView(const PHRect & frame, int image) : PHImageView(frame)
{
    init(image);
}

PHPoofView::PHPoofView(int image) : PHImageView()
{
    init(image);
}

void PHPoofView::init(int image)
{
    mutex();
    PHImage * img = NULL;
    switch (image) {
        case poof:
            img = poofImage();
            break;
        case boom:
            img = boomImage();
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

void PHPoofView::poofImageRelease()
{
    poofImage()->release();
    poofImg = NULL;
}

void PHPoofView::boomImageRelease()
{
    boomImage()->release();
    boomImg = NULL;
}