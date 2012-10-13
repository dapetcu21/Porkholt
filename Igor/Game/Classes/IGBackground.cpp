/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGBackground.h"
#include <Porkholt/Core/PHImageview.h>
#include <Porkholt/Core/PHGameManager.h>

IGBackground::IGBackground(PHGameManager * gm) : PHViewController(gm)
{
}

PHView * IGBackground::loadView(const PHRect & f)
{
    PHImageView * iv = new PHImageView(f);
    iv->setImage(gm->imageNamed("bg"));
    return iv;
}

IGBackground::~IGBackground()
{
}

void IGBackground::updateScene(ph_float elapsed)
{
}

