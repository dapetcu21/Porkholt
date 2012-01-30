//
//  PHEntryPoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHGameManager.h"
#include "PHMenuController.h"
#include "PHChapterController.h"

void PHGameManager::entryPoint()
{
    view = new PHView(PHRect(0,0,_screenWidth,_screenHeight));
    view->setGameManager(this);
	view->setBackgroundColor(PHGrayColor);
	view->setUserInput(true);
	
	viewController = new PHNavigationController();
	viewController->init(this);
	viewController->_viewWillAppear();
	view->addSubview(viewController->getView());
	viewController->_viewDidAppear();
    
    PHMenuController * vc = new PHMenuController();
	vc->init(this);
    viewController->pushViewController(vc);
}
