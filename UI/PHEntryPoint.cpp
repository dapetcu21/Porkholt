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
    //	PHViewController * vc = new PHChapterController(PHFileManager::resourcePath()+"/levels/current");
    PHViewController * vc = new PHMenuController();
	vc->init(this);
    viewController->pushViewController(vc);
}
