/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHChapterController.h"
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/UI/PHButtonView.h>
#include "PHLevelController.h"
#include <Porkholt/Core/PHNavigationController.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHFont.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <sstream>

#define SIZ_BK_WID 0.09
#define BORDER_BK_RIGHT 0 
#define BORDER_BK_DOWN 0

PHView * PHChapterController::loadView(const PHRect & frame)
{
    if (gm->imageExists("bg", path))
        bg = gm->imageNamed("bg", path);
    else
        bg = NULL;
    
    PHImageView * view = new PHImageView(frame);
    view->setImage(bg);
    view->setUserInput(true);
    float upperBorder = 0.425*frame.height;
    float lowerBorder = 0.175*frame.height;
    float leftBorder = 0.1*frame.width;
    float rightBorder = 0.1*frame.width;
    float levelSize = 0.1*frame.width;
    int columns = 5;
    int rows = 2;
    float vertSpacing = (rows==1)?0:((frame.height-upperBorder-lowerBorder-levelSize)/(rows-1));
    float horisSpacing = (columns==1)?0:((frame.width-leftBorder-rightBorder-levelSize)/(columns-1));
    for (int i=0; i<columns; i++)
        for (int j=0; j<rows; j++)
        {
            ostringstream oss1,oss2,oss;
            oss <<"lvl"<<j*columns+i+1;
            if (!(path->directoryExists(oss.str()))) continue;
            PHDirectory * lvlDir = path->directoryAtPath(oss.str());
            PHRect frame = PHRect(leftBorder+horisSpacing*i, lowerBorder+vertSpacing*(rows-j-1), levelSize, levelSize);
            PHButtonView * vv = new PHButtonView(frame);
            vv->setImage(gm->imageNamed("thumb", lvlDir));
            vv->setPressedImage(gm->imageNamed("thumb_pressed", lvlDir));
            vv->setUpCallback(PHInv(this, PHChapterController::mouseUp, (void*)(size_t)(j*columns+i+1)));
            view->addChild(vv);
            vv->release();
        }
   
    PHTextView * dadoamne = new PHTextView(PHRect(leftBorder,frame.height-upperBorder,frame.width-rightBorder-leftBorder, upperBorder));
    dadoamne->setFont(gm->fontNamed("BDCartoonShout"));
    dadoamne->setText("Da Doamne sa mearga");
    dadoamne->setFontSize(25);
    dadoamne->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
    dadoamne->setFontColor(PHWhiteColor);
    view->addChild(dadoamne);
    dadoamne->release();
    
    PHImage * img = gm->imageNamed("back");
    float hi = ((float)img->height())/((float)img->width())*(SIZ_BK_WID*frame.width);
    PHButtonView * back = new PHButtonView(PHRect((1.0f-BORDER_BK_RIGHT-SIZ_BK_WID)*frame.width,BORDER_BK_DOWN*frame.height,SIZ_BK_WID*frame.width,hi));
    back->setImage(img);
    back->setPressedImage(gm->imageNamed("back_pressed"));
    back->setUpCallback(PHInv(this, PHChapterController::backPressed,NULL));
    view->addChild(back);
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
        oss<<"lvl"<<nr+1;
        ret = !path->directoryExists(oss.str());
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
    PHAutoreleasePool ap;
    ostringstream oss;
    oss<<"lvl"<<nr;
    PHLevelController * lvlvc = new PHLevelController(gm, path->directoryAtPath(oss.str()));
    lvlvc->setEndLevelCallback(PHInv(this, PHChapterController::levelEnded, (void*)(size_t)nr));
    PHViewController * vc = lvlvc->mainViewController();
    navController->navigationController()->pushViewController(vc,PHNavigationController::FadeToColor,replace);
    vc->release();
    lvlvc->release();

}

PHChapterController::PHChapterController(PHGameManager * gm, PHDirectory * _path) : PHViewController(gm), path(_path)
{
    path->retain();
}

PHChapterController::~PHChapterController()
{
    path->release();
    if (bg)
        bg->release();
}
