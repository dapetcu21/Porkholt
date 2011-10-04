//
//  PHTextControllerView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTextControllerView.h"
#include "PHTextController.h"

PHTextControllerView::PHTextControllerView(PHTextController * del) : PHView(), delegate(del)
{
    
}

void PHTextControllerView::touchEvent(PHEvent * touch)
{
    if (touch->type() == PHEvent::touchDown)
        delegate->advance();
}