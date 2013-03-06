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
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/IO/PHDirectory.h>
#include <dlfcn.h>

#define fadeMargins 0.2f

float frand(float f)
{
    return rand() * f * (1.0f / RAND_MAX);
}

float frand(float s, float f)
{
    return rand() * (f - s) * (1.0f / RAND_MAX) + s;
}

struct BubblesSettings
{
    int bgType;
    float r,g,b;
    float rs,gs,bs;
    int particleCount;
    float particleLifetime, particleSize, velocityMagnitude, particleAlpha, minimumDepth, maximumDepth;
    bool swirl;
    bool sharpEdges;
};

class BubblesPro
{
    protected:
        void * _handle;
        void (*_loadSettings)(BubblesSettings *);
        void * (*_settingsViewController)(void*,BubblesSettings*);
        
    public:
        BubblesPro() : _handle(NULL)
        {
        }
        
        ~BubblesPro()
        {
            if (_handle)
                dlclose(_handle);
        }
        
        void loadHandle()
        {
            if (_handle) return;
            _handle = dlopen("/Library/LivePapers/Plugins/BubblesPro.dylib", RTLD_LOCAL | RTLD_LAZY);
            if (_handle)
            {
                _loadSettings = (void (*)(BubblesSettings*))dlsym(_handle, "BubblesLoadSettings"); 
                _settingsViewController = (void * (*)(void*, BubblesSettings*))dlsym(_handle, "BubblesSettingsViewController");
            } else {
                _loadSettings = NULL;
                _settingsViewController = NULL;
            }
        }
        bool isPro() 
        {
            loadHandle();
            return _settingsViewController != NULL;
        }
        
        void loadDefaults(BubblesSettings * s)
        {
            s->bgType = 0;
            s->r = 1.0f; s->g = 1.0f; s->b = 1.0f;
            s->rs = 1.0f; s->gs = 1.0f; s->bs = 1.0f;
            s->particleCount = 30;
            s->particleLifetime = 20.0f;
            s->particleSize = 0.5f;
            s->velocityMagnitude = 0.2f;
            s->particleAlpha = 0.5f;
            s->minimumDepth = 0.0f;
            s->maximumDepth = 1.0f;
            s->swirl = false;
            s->sharpEdges = false;
        }

        void loadSettings(BubblesSettings * s)
        {
            loadDefaults(s);
            loadHandle();
            if (_loadSettings)
                _loadSettings(s);
        }
        
        void * viewController(void * dict)
        {
            loadHandle();
            if (_settingsViewController)
            {
                BubblesSettings def;
                loadDefaults(&def);
                return _settingsViewController(dict, &def);
            }
            return NULL;
        }
};

static BubblesPro proFeatures;

extern "C" void * __attribute__((visibility("default"))) LPInitPrefsViewController(void * info)
{
    return proFeatures.viewController(info);
}

extern "C" int __attribute__((visibility("default"))) LPHasPrefs(void * info)
{
    return proFeatures.isPro();
}

class BubblesAnimator : public PHParticleAnimator, public PHDrawableInputDelegate
{
    protected:
        struct part
        {
            int type;
            PHColor clr;
            float timeElapsed;
            float lifeTime;
            float size;
            PHVector2 pos;
            PHVector2 vel;
            float velocityMagnitude;
        } * partv;
        
        BubblesSettings s;
        
        particles parts;
        particle2D * v;
        
        PHParticleView * particleView;
        PH2DCamera * cam;
        
        map<void *, PHPoint> points;
    public:
        BubblesAnimator(PH2DCamera * c, PHParticleView * pv) : cam(c), particleView(pv), v(NULL), partv(NULL)
        {
            cam->retain();
            parts.type = particleType2D;
        }
        
        void loadSettings(BubblesSettings * sett)
        {
            memcpy(&s, sett, sizeof(BubblesSettings));
            parts.n = s.particleCount;
            parts.v2d = v = (particle2D*)realloc(v, sizeof(particle2D) * s.particleCount);
            partv = (struct part*)realloc(partv, sizeof(struct part) * s.particleCount);
            for (int i = 0; i < s.particleCount; i++)
                partv[i].type = -1;
            advanceAnimation(0);
        }

        BubblesSettings & settings()
        {
            return s;
        }
        
        ~BubblesAnimator()
        {
            cam->release();
            free(v);
            free(partv);
        }
        
        particles * calculatedParticles()
        {
            return &parts;
        }

        void advanceAnimation(ph_float elapsed)
        {
            if (!v || !partv) return;
            PHSize sz = cam->realScreenSize();
            PHSize halfsz = sz * 0.5f;
            for (int i = 0; i < s.particleCount; i++)
            {
                part * p = partv + i;
                if (p->type != -1)
                {
                    for (map<void*, PHPoint>::iterator i = points.begin(); i != points.end(); i++)
                    {
                        PHPoint pnt = p->pos - i->second;
                        pnt.normalize();
                        float ang = PHAngleFromNormalizedVector(pnt);
                        if (s.swirl)
                        {
                        } else {
                            if (ang < M_PI)
                                ang = M_PI - ang;
                            else
                                ang = -M_PI - ang;
                        }
                        p->vel += (PHVector2(0, 1).rotated(ang)) * elapsed;
                    }
                    p->pos += p->vel * elapsed;
                    float m = p->vel.length(), n = m;
                    PHLowPassFilter(m, p->velocityMagnitude, elapsed, 1.0f);
                    if (n)
                        p->vel *= m / n;
                    if (!PHPointInRect(p->pos, PHRect(
                                    - halfsz.x - p->size,
                                    - halfsz.y - p->size,
                                      sz.x + 2.0f * p->size,
                                      sz.y + 2.0f * p->size)))
                        p->type = -1;
                    p->timeElapsed += elapsed;
                    if (p->timeElapsed > p->lifeTime)
                        p->type = -1;
                }
                if (p->type == -1)
                {
                    float depth = frand(s.minimumDepth, s.maximumDepth);
                    p->type = 5 - floor(depth * 5.9f);
                    p->timeElapsed = 0;
                    p->lifeTime = frand(s.particleLifetime * 0.5f, s.particleLifetime);
                    p->clr = PHColor(1.0f, 1.0f, 1.0f, depth * s.particleAlpha);
                    p->size = s.particleSize * depth;
                    p->pos = PHPoint(frand(sz.x) - halfsz.x, frand(sz.y) - halfsz.y);
                    p->velocityMagnitude = depth * s.velocityMagnitude;
                    p->vel = PHVector2(0.0f, 1.0f).rotated(frand(M_PI * 2)) * p->velocityMagnitude;
                }
                v[i].color = p->clr;
                float tq = p->timeElapsed / p->lifeTime;
                if (tq < fadeMargins)
                    v[i].color.a *= tq * (1.0f / fadeMargins);
                else if (tq > (1.0f - fadeMargins))
                    v[i].color.a *= (1.0f - tq) * (1.0f / fadeMargins);
                v[i].position = p->pos;
                v[i].size = PHSize(p->size, p->size);
                v[i].ud = (void*)(size_t)p->type;
                v[i].rotation = 0;
            }
        }
        
        void addPoint(void * ud, PHPoint p)
        {
            points.insert(make_pair(ud, p));
        }

        void replacePoint(void * ud, PHPoint p)
        {
            map<void*, PHPoint>::iterator i = points.find(ud);
            i->second = p;
        }
        
        void removePoint(void * ud, PHPoint p)
        {
            points.erase(ud);
        }
        
        void drawableRecievedEvent(PHDrawable * d, PHEvent * evt)
        {
            switch (evt->type())
            {
                case PHEvent::touchDown:
                    evt->setHandled(true);
                    this->addPoint(evt->userData(), evt->drawableLocation()->pointInView(particleView));
                    break;
                case PHEvent::touchMoved:
                    this->replacePoint(evt->userData(), evt->drawableLocation()->pointInView(particleView));
                    break;
                case PHEvent::touchCancelled:
                case PHEvent::touchUp:
                    this->removePoint(evt->userData(), evt->drawableLocation()->pointInView(particleView));
                    break;
            }

        }
};

class BubblesViewController : public PHViewController
{
    protected:
        BubblesAnimator * anim;
        PHParticleView * part;
        PHImageView * bg;
    public:
        BubblesViewController(PHGameManager * gm) : PHViewController(gm), anim(NULL), bg(NULL) {};
        
        void reloadSettings()
        {
            BubblesSettings s;
            proFeatures.loadSettings(&s);
            anim->loadSettings(&s);
            part->setImage(gm->imageNamed(s.sharpEdges ? "sharp_particle" : "particles"));
            switch (s.bgType)
            {
                case 0:
                case 2:
                    bg->setTintColor(PHInvalidColor);
                    bg->setShader(gm->spriteShader());
                    break;
                case 1:
                    bg->setTintColor(PHColor(s.r, s.g, s.b));
                    bg->setShader(gm->shaderProgramNamed("tinted"));
                    bg->additionalUniforms()->at("color2").setValue(PHColor(s.rs, s.gs, s.bs));
                    break;
            }
            bg->setImage(s.bgType == 2 ? LPPHGetWallpaperImage(gm) : gm->imageNamed("background"));
        }
        
        void wallpaperChanged()
        {
            if (anim && bg && anim->settings().bgType == 2)
                bg->setImage(LPPHGetWallpaperImage(gm));
        }
        
        PHView * loadView(const PHRect & r)
        {
            PHGL::glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
            bg = new PHImageView(r);
            bg->setAutoresizeMask(PHView::ResizeAll);
            bg->setAutoresizesSubviews(true);
            bg->setUserInput(true);
            bg->retain();
            
            PHParticleView * v = new PHParticleView(PHRect(r.width * 0.5f, r.height * 0.5f, r.width * 0.5f, r.height * 0.5f));
            if (!anim)
                anim = new BubblesAnimator((PH2DCamera*)gm->mainDrawable(), v);
            v->setParticleAnimator(anim);
            v->setUseUDAsAtlasIndex(true);
            v->setAutoresizeMask(PHView::ResizeFlexibleWidth | PHView::ResizeFlexibleHeight | PHView::ResizeFixedUp | PHView::ResizeFixedRight);
            bg->addChild(v);
            bg->setInputDelegate(anim);
            part = v;

            reloadSettings();
            gm->messageWithName("reloadPreferences")->addListener(PHInvBind(this, BubblesViewController::reloadSettings, NULL));
            gm->messageWithName("wallpaperImageChanged")->addListener(PHInvBind(this, BubblesViewController::wallpaperChanged, NULL));
            
            return bg;
        }
        
        ~BubblesViewController()
        {
            if (anim)
                anim->release();
            if (part)
                part->release();
            if (bg)
                bg->release();
        }
}; 

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new BubblesViewController(gm);

    PH2DCamera * camera = new PH2DCamera();
    gm->setMainDrawable(camera);
    camera->setScreenSize(PHSize(0, 4.0f));
    PHSize bounds = camera->realScreenSize();
    PHViewControllerHolder * holder = new PHViewControllerHolder(PHRect(0, 0, bounds.x, bounds.y));
    camera->addChild(holder);
    
    holder->setViewController(vc);
    
    camera->release();
    holder->release();
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync | PHWResizable | PHWFrameAnimation, &PHGameEntryPoint,NULL);
 
