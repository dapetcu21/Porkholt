//
//  PHPoofView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHPoofView.h"
#include "PHImageAnimator.h"

PHImage * PHPoofView::img = NULL;

PHImage * PHPoofView::poofImage()
{
    if (!img)
    {
        img = PHImage::imageNamed("poof");
    }
    return img;
}

PHPoofView::PHPoofView(const PHRect & frame) : PHImageView(frame)
{
    init();
}

PHPoofView::PHPoofView() : PHImageView()
{
    init();
}

void PHPoofView::init()
{
    mutex();
    setImage(poofImage());
    PHImageAnimator * a = animator();
    if (!a)
        destroy(NULL, NULL);
    else
        a->reset(this,(PHCallback)&PHPoofView::destroy,NULL);
}

void PHPoofView::destroy(PHObject * sender, void * ud)
{
    if (ud)
        removeFromSuperview();
    else
        PHThread::mainThread()->executeOnThread(this, (PHCallback)&PHPoofView::destroy, (void*)1, false);
}

void PHPoofView::poofImageRelease()
{
    img = (PHImage*)poofImage()->release();
}