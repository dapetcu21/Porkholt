/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHViewControllerHolder.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/IO/PHDirectory.h>

static const PHPoint points[] = {
    PHPoint(0.251411, 0.498848),
    PHPoint(0.209374, 0.588037),
    PHPoint(0.162346, 0.588037),
    PHPoint(0.204664, 0.409678),
    PHPoint(0.117883, 0.409678),
    PHPoint(0.163555, 0.322891),
    PHPoint(0.026880, 0.322236),
};

static const int starters[] = {1,   0};

static const int connections[][3] = {
    {2,  1, 3},  //0
    {1,  2},     //1
    {0   },      //2
    {2,  4, 5},  //3
    {0   },      //4
    {2,  5, 6},  //5
    {0   },      //6
};

#define fadeTime 0.15f

class ElectricParticleAnimator : public PHParticleAnimator
{
    protected:
        struct part {
            int stage;
            int endpoint;
            PHVector2 pos;
            ph_float elapsed;
        } * pv; 
        particles particles;
        particle2D * p2v;
        ph_float _particleSize;
        int _particleCount; 
        ph_float _particleVelocity;
        PHParticleView * _particleView;
        PHColor _particleColor;
        
    public:
        ElectricParticleAnimator() : pv(NULL), p2v(NULL), _particleCount(0), _particleSize(0), _particleVelocity(0)
        {
            particles.n = 0;
            particles.v2d = NULL;
            particles.type = particleType2D;
        }
        
        PHParticleView * particleView() { return _particleView; }
        void setParticleView(PHParticleView * pv) { _particleView = pv; }
        ph_float particleSize() { return _particleSize; }
        void setParticleSize(ph_float f) { _particleSize = f; }
        ph_float particleVelocity() { return _particleVelocity; }
        void setParticleVelocity(ph_float v) { _particleVelocity = v; }
        PHColor particleColor() { return _particleColor; }
        void setParticleColor(const PHColor & c) { _particleColor = c; }
        int particleCount() { return _particleCount; }
        void setParticleCount(int c)
        {
            if (_particleCount == c) return;
            _particleCount = c;
            pv = (part*)realloc(pv, sizeof(part) * _particleCount);
            p2v = (particle2D*)realloc(p2v, sizeof(particle2D) * _particleCount);
            particles.n = _particleCount;
            particles.v2d = p2v;
            for (int i = 0; i < _particleCount; i++)
            {
                pv[i].stage = 3;
                pv[i].elapsed = rand() * (10.0f / RAND_MAX);
            }
        }

        void advanceAnimation(ph_float elapsed)
        {
            if (!elapsed) return;   
            PHSize bounds = _particleView->bounds().size();
            for (int i = 0; i < _particleCount; i++)
            {
                part * p = pv + i;
                ph_float elps = elapsed;
                while (elps > FLT_EPSILON)
                {
                    bool consumedAll = p->elapsed <= elps;
                    ph_float el;
                    if (consumedAll)
                    {
                        el = p->elapsed;
                        elps -= el;
                        p->elapsed = 0.0f;
                    } else {
                        el = elps;
                        elps = 0.0f;
                        p->elapsed -= el;
                    }
                    
                    if (p->stage == 1)
                        p->pos += (points[p->endpoint] - p->pos).normalized() * _particleVelocity * el;

                    if (consumedAll)
                    {
                        p->stage = (p->stage + 1) & 3;
                        switch (p->stage)
                        {
                            case 0:
                                p->elapsed = fadeTime / _particleVelocity;
                                p->endpoint = starters[rand() % starters[0] + 1]; 
                                p->pos = points[p->endpoint]; 
                                break;
                            case 1:
                            case 2:
                                if (connections[p->endpoint][0])
                                {
                                    p->stage = 1;
                                    p->endpoint = connections[p->endpoint][rand() % connections[p->endpoint][0] + 1];
                                    p->elapsed = (points[p->endpoint] -  p->pos).length() / _particleVelocity;
                                } else {
                                    p->elapsed = fadeTime / _particleVelocity;
                                    break;
                                }
                                break;
                            case 3:
                                p->elapsed = rand() * (20.0f / RAND_MAX);
                                break;
                        }
                    }
                }
                
                particle2D * p2 = p2v + i;
                p2->position = p->pos * _particleView->bounds().size();
                p2->rotation = 0;
                p2->color = _particleColor;
                p2->size = (p->stage == 3) ? PHOriginPoint : PHSize(_particleSize * bounds.height, _particleSize * bounds.height);
                switch(p->stage)
                {
                    case 0:
                        p2->color.a *= 1.0f - (p->elapsed * _particleVelocity / fadeTime);
                        break;
                    case 2:
                        p2->color.a *= (p->elapsed * _particleVelocity / fadeTime);
                        break;

                }
                
            }
        }
        
        struct particles * calculatedParticles()
        {
            return &particles;
        }
};

class ElectricViewController : public PHViewController
{
    protected:
        PHParticleView * particleView;
        PHImageView * foregroundView;
        PHView * backgroundView;
        ElectricParticleAnimator * particleAnimator;
    public:
        ElectricViewController(PHGameManager * gm) : PHViewController(gm) {};
        ~ElectricViewController()
        {
            if (particleAnimator)
                particleAnimator->release();
            if (backgroundView)
                backgroundView->release();
            if (foregroundView)
                foregroundView->release();
            if (particleView)
                particleView->release();
        }
        
        PHView * loadView(const PHRect & r)
        {
            PHView * v = new PHView(r);
            v->setAutoresizeMask(PHView::ResizeAll);
            v->setAutoresizesSubviews(true);
            
            PHImageView * iv = new PHImageView(r);
            iv->setImage(gm->imageNamed("mask_phl"));
            iv->setShader(gm->shaderProgramNamed("background"));
            iv->setZPosition(-1.0f);
            iv->setZTestingEnabled(true);
            iv->setAutoresizeMask(PHView::ResizeAll);
            v->addChild(iv);
            foregroundView = iv;
            
            PHView * bg = new PHView(r);
            bg->setZTestingEnabled(true);
            bg->setAutoresizeMask(PHView::ResizeAll);
            v->addChild(bg);
            backgroundView = bg;
            
            PHParticleView * pv = new PHParticleView(r);
            ElectricParticleAnimator * pa = new ElectricParticleAnimator(); 
            pa->setParticleView(pv);
            pv->setParticleAnimator(pa);
            pv->setAutoresizeMask(PHView::ResizeAll);
            pv->setZTestingEnabled(true);
            pv->setImage(gm->imageNamed("particle"));
            v->addChild(pv);
            particleView = pv;
            particleAnimator = pa;
            
            reloadSettings();
            return v;
        }
        
        void reloadSettings()
        {
            foregroundView->setTintColor(PHColor(0.9f, 0.1f, 0.1f)); 
            backgroundView->setBackgroundColor(PHColor(1.0f, 1.0f, 1.0f));
            particleAnimator->setParticleColor(PHColor(0.9f, 0.1f, 0.1f)); 
            
            particleAnimator->setParticleSize(0.20f);
            particleAnimator->setParticleCount(10);
            particleAnimator->setParticleVelocity(0.6f); 
        }
}; 

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new ElectricViewController(gm);

    PH2DCamera * camera = new PH2DCamera();
    gm->setMainDrawable(camera);
//    camera->setScreenSize(PHSize(0, 4.0f));
    PHSize bounds = camera->realScreenSize();
    PHViewControllerHolder * holder = new PHViewControllerHolder(PHRect(0, 0, bounds.x, bounds.y));
    camera->addChild(holder);
    
    holder->setViewController(vc);
    
    camera->release();
    holder->release();
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync | PHWResizable | PHWFrameAnimation | PHWDepthBuffer, &PHGameEntryPoint,NULL);
 
