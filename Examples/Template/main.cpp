/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestViewController.h>

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHTestViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync, &PHGameEntryPoint,NULL);
 
