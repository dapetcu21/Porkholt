//
//  PHEntryPoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHMainEvents.h"
#include "PHMenuController.h"
#include "PHChapterController.h"

void PHMainEvents::entryPoint()
{
    //	PHViewController * vc = new PHChapterController(PHFileManager::resourcePath()+"/levels/current");
    PHViewController * vc = new PHMenuController();
	vc->init();
    viewController->pushViewController(vc);
}
