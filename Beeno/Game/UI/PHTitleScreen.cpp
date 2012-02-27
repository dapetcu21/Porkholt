//
//  PHTitleScreen.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/20/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTitleScreen.h"
#include "PHImageView.h"
#include "PHImage.h"
#include "PHButtonView.h"
#include "PHChapterController.h"
#include "PHFileManager.h"

#include "PHTestViewController.h"

#define POS_BG_Y 0.65
#define BORDER_BG_LEFT 0.05
#define SIZ_BG_WID 0.6

#define POS_BT_X 0.5
#define POS_BT_Y 0.3
#define SIZ_BT 0.15

PHView * PHTitleScreen::loadView(const PHRect & frame)
{
    PHImage * img;
    PHView * v = new PHView(frame);
    v->setUserInput(true);
    ph_float w = frame.width;
    ph_float h = frame.height;
    img = gm->imageNamed("title");
    ph_float hi = ((ph_float)img->height())/((ph_float)img->width())*(SIZ_BG_WID*w);
    PHImageView * bg = new PHImageView(PHRect(
        BORDER_BG_LEFT*w,
        POS_BG_Y*h-hi/2,
        SIZ_BG_WID*w,
        hi
    ));
    bg->setImage(img);
    bg->setTag(3562);
    
    img = gm->imageNamed("start_game");
    hi = ((ph_float)img->height())/((ph_float)img->width())*(SIZ_BT*w);
    PHButtonView * bt = new PHButtonView(PHRect(
        POS_BT_X*w-SIZ_BT*w/2,
        POS_BT_Y*h-hi/2,
        SIZ_BT*w,
        hi
    ));
    bt->setImage(img);
    bt->setPressedImage(gm->imageNamed("start_game_pressed"));
    bt->setUpCallback(PHInv(this,PHTitleScreen::buttonPressed, NULL));
    bt->setUserInput(true);
    
    PHButtonView * bl = new PHButtonView(PHRect(0,0,50,50));
    bl->setImage(gm->imageNamed("placeholder"));
    bl->setPressedImage(gm->imageNamed("placeholder"));
    bl->setUpCallback(PHInv(this,PHTitleScreen::testPressed, NULL));
    bl->setUserInput(true);
    
    v->addSubview(bg);
    v->addSubview(bt);
    v->addSubview(bl);
    bg->release();
    bt->release();
    
    return v;
}

void PHTitleScreen::testPressed(PHObject * sender, void * ud)
{
    PHTestViewController * vc = new PHTestViewController();
	vc->init(gameManager());
    navigationController()->pushViewController(vc);
}

void PHTitleScreen::buttonPressed(PHObject * sender, void * ud)
{
    PHViewController * vc = new PHChapterController(gm->resourcePath()+"/levels/current");
	vc->init(gm);
    gm->loadAllImages();
    navigationController()->pushViewController(vc,PHNavigationController::SlideLeft);
    vc->release();
}