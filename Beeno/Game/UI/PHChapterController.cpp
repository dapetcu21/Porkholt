//
//  PHChapterController.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHChapterController.h"
#include "PHImageView.h"
#include "PHButtonView.h"
#include "PHLevelController.h"
#include "PHNavigationController.h"
#include "PHFileManager.h"
#include "PHTextView.h"
#include "PHFont.h"
#include <sstream>

#define SIZ_BK_WID 0.09
#define BORDER_BK_RIGHT 0 
#define BORDER_BK_DOWN 0

PHView * PHChapterController::loadView(const PHRect & frame)
{
    PHImageView * view = new PHImageView(frame);
    view->setImage(bg);
    view->setUserInput(true);
    ph_float upperBorder = 0.425*frame.height;
    ph_float lowerBorder = 0.175*frame.height;
    ph_float leftBorder = 0.1*frame.width;
    ph_float rightBorder = 0.1*frame.width;
    ph_float levelSize = 0.1*frame.width;
    int columns = 5;
    int rows = 2;
    ph_float vertSpacing = (rows==1)?0:((frame.height-upperBorder-lowerBorder-levelSize)/(rows-1));
    ph_float horisSpacing = (columns==1)?0:((frame.width-leftBorder-rightBorder-levelSize)/(columns-1));
    for (int i=0; i<columns; i++)
        for (int j=0; j<rows; j++)
        {
            ostringstream oss1,oss2,oss;
            oss<<path<<"/lvl"<<j*columns+i+1;
            oss1<<path<<"/lvl"<<j*columns+i+1<<"/thumb.png";
            oss2<<path<<"/lvl"<<j*columns+i+1<<"/thumb_pressed.png";
            if (!PHFileManager::isDirectory(oss.str())) continue;
            PHRect frame = PHRect(leftBorder+horisSpacing*i, lowerBorder+vertSpacing*(rows-j-1), levelSize, levelSize);
            PHButtonView * vv = new PHButtonView(frame);
            vv->setImage(gm->imageFromPath(oss1.str()));
            vv->setPressedImage(gm->imageFromPath(oss2.str()));
            vv->setUpCallback(PHInv(this, PHChapterController::mouseUp, (void*)(j*columns+i+1)));
            view->addSubview(vv);
            vv->release();
        }
   
    PHTextView * dadoamne = new PHTextView(PHRect(leftBorder,frame.height-upperBorder,frame.width-rightBorder-leftBorder, upperBorder));
    dadoamne->setFont(gm->fontNamed("BDCartoonShout"));
    dadoamne->setText("Da Doamne sa mearga");
    dadoamne->setFontSize(25);
    dadoamne->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
    dadoamne->setFontColor(PHWhiteColor);
    view->addSubview(dadoamne);
    dadoamne->release();
    
    PHImage * img = gm->imageNamed("back");
    ph_float hi = ((ph_float)img->height())/((ph_float)img->width())*(SIZ_BK_WID*frame.width);
    PHButtonView * back = new PHButtonView(PHRect((1.0f-BORDER_BK_RIGHT-SIZ_BK_WID)*frame.width,BORDER_BK_DOWN*frame.height,SIZ_BK_WID*frame.width,hi));
    back->setImage(img);
    back->setPressedImage(gm->imageNamed("back_pressed"));
    back->setUpCallback(PHInv(this, PHChapterController::backPressed,NULL));
    view->addSubview(back);
    back->release();
    return view;
}

void PHChapterController::levelEnded(PHObject * sender, void *ud)
{
    int nr = (size_t)ud;
    PHLevelController * lvl = (PHLevelController*)sender;
    bool ret = true;
    if (lvl->outcome()==PHLevelController::LevelWon)
    {
        ostringstream oss;
        oss<<path<<"/lvl"<<nr+1;
        ret = !PHFileManager::isDirectory(oss.str());
    }
    if (ret)
    {
        navController->navigationController()->popViewController(PHNavigationController::FadeToColor);
    } else
        loadLevel(nr+1, true);
}

void PHChapterController::mouseUp(PHObject * sender, void * ud)
{
    if (getViewState()!=StateAppeared) return;
    loadLevel((size_t)ud, false);
}

void PHChapterController::backPressed(PHObject * sender, void *ud)
{
    if (getViewState()!=StateAppeared) return;
    navController->popViewController(PHNavigationController::SlideRight);
}

void PHChapterController::loadLevel(int nr,bool replace)
{
    ostringstream oss;
    oss<<path<<"/lvl"<<nr;
    PHLevelController * lvlvc = new PHLevelController(oss.str());
    lvlvc->init(gm);
    lvlvc->setEndLevelCallback(PHInv(this, PHChapterController::levelEnded, (void*)nr));
    PHViewController * vc = lvlvc->mainViewController();
    navController->navigationController()->pushViewController(vc,PHNavigationController::FadeToColor,replace);
    vc->release();
    lvlvc->release();

}

PHChapterController::PHChapterController(const string & _path) : path(_path)
{
    if (PHFileManager::fileExists(path+"/bg.png"))
        bg = gm->imageFromPath(path+"/bg.png");
    else
        bg = NULL;
    
}

PHChapterController::~PHChapterController()
{
    if (bg)
        bg->release();
}