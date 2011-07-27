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
#include <sstream>

PHView * PHChapterController::loadView(const PHRect & frame)
{
    PHImageView * view = new PHImageView(frame);
    view->setImage(bg);
    view->setUserInput(true);
    double upperBorder = 0.425*frame.height;
    double lowerBorder = 0.175*frame.height;
    double leftBorder = 0.1*frame.width;
    double rightBorder = 0.1*frame.width;
    double levelSize = 0.1*frame.width;
    int columns = 5;
    int rows = 2;
    double vertSpacing = (rows==1)?0:((frame.height-upperBorder-lowerBorder-levelSize)/(rows-1));
    double horisSpacing = (columns==1)?0:((frame.width-leftBorder-rightBorder-levelSize)/(columns-1));
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
            vv->setImage(PHImage::imageFromPath(oss1.str()));
            vv->setPressedImage(PHImage::imageFromPath(oss2.str()));
            vv->setUpCallBack(this, (PHCallback)&PHChapterController::mouseUp, (void*)(j*columns+i+1));
            view->addSubview(vv);
            vv->release();
        }
    return view;
}

void PHChapterController::updateScene(double timeElapsed)
{
    
}

void PHChapterController::mouseUp(PHObject * sender, void * ud)
{
    if (getViewState()!=StateAppeared) return;
    
    ostringstream oss;
    oss<<path<<"/lvl"<<(int)ud;
    PHLevelController * lvlvc = new PHLevelController(oss.str());
    lvlvc->init();
    PHViewController * vc = lvlvc->mainViewController();
    navController->pushViewController(vc,PHNavigationController::FadeToColor);
    vc->release();
    lvlvc->release();
}

PHChapterController::PHChapterController(const string & _path) : path(_path)
{
    if (PHFileManager::fileExists(path+"/bg.png"))
        bg = PHImage::imageFromPath(path+"/bg.png");
    else
        bg = NULL;
    
}

PHChapterController::~PHChapterController()
{
    bg->release();
}