/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHWindowing.h"
#include "PHTestViewController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHTestViewController * vc = new PHTestViewController();
	  vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWGLES1 | PHWVSync, &PHGameEntryPoint,NULL);
}
 
