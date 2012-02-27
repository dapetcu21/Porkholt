//
//  PHTrailImageView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTrailImageView.h"

PH_REGISTERIMAGEVIEW(PHTrailImageView)

#define INIT_LIST ,snap(1),ssnap(0),trail(0),_stopView(NULL), auxImg(NULL)

PHTrailImageView::PHTrailImageView() : PHImageView() INIT_LIST { dontDrawOnMain = false; }

PHTrailImageView::PHTrailImageView(const PHRect &frame): PHImageView(frame) INIT_LIST { dontDrawOnMain = false; }

PHTrailImageView::PHTrailImageView(PHImage * image) : PHImageView(image) INIT_LIST { dontDrawOnMain = false;}

PHTrailImageView::~PHTrailImageView() {
    if (auxImg)
        auxImg->release();
}

void PHTrailImageView::saveState(pframe & fr)
{
    fr.m = loadMatrixTree(_stopView);
    saveMinState(fr);
}

const PHMatrix & PHTrailImageView::loadState(const pframe & fr)
{
    loadMinState(fr);
    return fr.m;
}

void PHTrailImageView::saveMinState(pframe & fr)
{
    fr.tint = tintColor();
}

void PHTrailImageView::loadMinState(const pframe & fr)
{
    setTintColor(fr.tint);
}

void PHTrailImageView::auxRender()
{
    PHMatrix om = gm->modelViewMatrix();
    pframe fr;
    saveMinState(fr);
    PHMatrix m = _stopView?_stopView->loadMatrixTree(NULL):PHIdentityMatrix;
    int n = (int)frames.size();
    int nr = 1;
    for (list<pframe>::iterator i = frames.begin(); i!=frames.end(); i++)
    {
        gm->setModelViewMatrix(m*loadState(*i));
        nr++;
        if (tint==PHInvalidColor)
            tint=PHWhiteColor;
        tint.a*= ((ph_float)nr)/n * 0.4;
        //tint.r*=0.6;
        //tint.g*=0.6;
        //tint.b*=0.6;
        drawBackground();
        if (auxImg)
        {
            PHImage * im = image();
            if (im) 
                im->retain();
            setImage(auxImg);
            draw();
            setImage(im);
            if (im)
                im->release();
        } else 
            draw();
    }
    loadMinState(fr);
    gm->setModelViewMatrix(om);
}

void PHTrailImageView::render()
{
    if (auxLayer==NULL)
        auxRender();
    PHImageView::render();
    ssnap=(ssnap+1)%snap;
    if (!ssnap)
        pushFrame();
}
                              


void PHTrailImageView::pushFrame()
{
    pframe tmp;
    saveState(tmp);
    frames.push_back(tmp);
    for (int i=0; i<2; i++)
        if (frames.size()>trail)
            frames.pop_front();
        else 
            break;
}