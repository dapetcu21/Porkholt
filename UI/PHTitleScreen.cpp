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
    double w = frame.width;
    double h = frame.height;
    img = PHImage::imageNamed("title");
    double hi = ((double)img->height())/((double)img->width())*(SIZ_BG_WID*w);
    PHImageView * bg = new PHImageView(PHRect(
        BORDER_BG_LEFT*w,
        POS_BG_Y*h-hi/2,
        SIZ_BG_WID*w,
        hi
    ));
    bg->setImage(img);
    bg->setTag(3562);
    
    img = PHImage::imageNamed("start_game");
    hi = ((double)img->height())/((double)img->width())*(SIZ_BT*w);
    PHButtonView * bt = new PHButtonView(PHRect(
        POS_BT_X*w-SIZ_BT*w/2,
        POS_BT_Y*h-hi/2,
        SIZ_BT*w,
        hi
    ));
    bt->setImage(img);
    bt->setPressedImage(PHImage::imageNamed("start_game_pressed"));
    bt->setUpCallback(PHInv(this,PHTitleScreen::buttonPressed, NULL));
    bt->setUserInput(true);
    
    v->addSubview(bg);
    v->addSubview(bt);
    bg->release();
    bt->release();
    
    return v;
}

void PHTitleScreen::buttonPressed(PHObject * sender, void * ud)
{
    PHViewController * vc = new PHChapterController(PHFileManager::resourcePath()+"/levels/current");
	vc->init(_gameManager);
    PHImage::loadAllImages();
    navigationController()->pushViewController(vc,PHNavigationController::SlideLeft);
    vc->release();
}