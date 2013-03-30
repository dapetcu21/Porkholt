/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHTime.h>

#include <Porkholt/IO/PHFile.h>

class PHTestAnim : public PHParticleAnimator
{
    protected:
        particles * p;
    public:
        PHTestAnim() : PHParticleAnimator()
        {
            p = new particles;
            p->n = 1;
            p->type = particleType2D;
            p->v2d = new particle2D;
            p->v2d[0].position = PHPoint(100, 100);
            p->v2d[0].rotation = M_PI/4;
            p->v2d[0].size = PHPoint(50, 50);
            p->v2d[0].color = PHWhiteColor;
            p->v2d[0].ud = (void*)(size_t)1;
        }
        
        void advanceAnimation(float elapsedTime)
        {
            p->v2d[0].rotation += M_PI * elapsedTime;
        }

        particles * calculatedParticles()
        {
            return p; 
        }
};

class PHParticlesTest: public PHViewController
{
    public:
        PHParticlesTest(PHGameManager * gm) : PHViewController(gm) {}

        PHView * loadView(const PHRect & r)
        {
            PHView * v = new PHView(r);
            PHParticleView * pv = new PHParticleView(v->bounds());
            PHParticleAnimator * pa = new PHTestAnim;
            pv->setParticleAnimator(pa);
            pv->setImage(gm->imageNamed("placeholder"));
            pa->release();
            v->addChild(pv);
            pv->release();
            return v;
        }

};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHParticlesTest(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync | PHWShowFPS, &PHGameEntryPoint,NULL);
 
