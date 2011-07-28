//
//  PHPoofView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHPoofView.h"
#include "PHImageAnimator.h"

PHImage * PHPoofView::poofImage()
{
    static PHImage * i = NULL;
    if (!i)
        i = PHImage::imageNamed("poof");
    return i;
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