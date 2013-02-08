/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHMotion.h>

#include <Porkholt/IO/PHFile.h>

class PHAccelTest: public PHViewController
{
    PHImageView * iv;
    public:
        PHAccelTest(PHGameManager * gm) : PHViewController(gm) {}

        PHView * loadView(const PHRect & r)
        {
            PHView * v = new PHView(r);
            ph_float m = min(r.width, r.height);
            PHView * container = new PHView(PHRect(r.width / 2, r.height /2 , 15, 15));
            container->setScalingCenter(PHPoint(0, 0));
            container->setScale(PHSize(m/30, m/30));
            v->addChild(container);

            iv = new PHImageView();
            iv->setImage(gm->imageNamed("arrow"));
            container->addChild(iv);

            container->release();

            return v;
        }

        void updateScene(ph_float elapsed)
        {
            PHAcceleration a = PHMotion::getAcceleration();
            PHLog("%f %f %f", a.x, a.y, a.z);
            PHVector2 v(a.x, a.y);
            ph_float len = v.length();
            v.normalize();
            iv->setFrame(PHRect(0, -1.5, len, 3));
            iv->setRotationalCenter(PHVector2(0, 1.5));
            iv->setRotation(PHAngleFromNormalizedVector(v));
        }

};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHAccelTest(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync | PHWShowFPS, &PHGameEntryPoint,NULL);
 
