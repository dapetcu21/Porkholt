/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHTextureCanvas.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

#include <ctime>

#include "ESettings.h"

#define PART_ROWS 5
#define PART_COLS 5
#define PART_SIZE 0.08
#define PART_SPEED 0.5
#define NUM_PART (PART_ROWS * PART_COLS)
#define floatrand() ((float)rand() / (float)RAND_MAX)

class PHStarsAnimator : public PHParticleAnimator
{
    protected:
        struct part 
        {
            PHVector2 pos;
            float offset;
            float opacity;
        } v[NUM_PART];
        particles parts;
        particle2D pv[NUM_PART];
        float time;
    public:
        PHStarsAnimator() : time(0)
        {
            const float dx = 2.0f / PART_ROWS;
            const float dy = 2.0f / PART_COLS;
            for (int i = 0; i < PART_ROWS; i++)
                for (int j = 0; j < PART_COLS; j++)
                {
                    part & p = v[i * PART_COLS + j];
                    p.pos = PHVector2(
                            -1.0f + dx * i + floatrand() * (1.5f * dx),
                            -1.0f + dy * j + floatrand() * (1.5f * dy));
                    p.offset = floatrand() * (M_PI * 2.0f);
                    p.opacity = floatrand() * 0.5f + 0.5f;
                }
            parts.n = NUM_PART;
            parts.type = particleType2D;
            parts.v2d = pv;
            for (int i = 0; i < NUM_PART; i++)
            {
                pv[i].position = v[i].pos;
                pv[i].rotation = 0;
                float size = PART_SIZE * (floatrand() * 0.5f + 0.5f);
                pv[i].size = PHSize(size, size);
                pv[i].ud = NULL;
            }
        }


        virtual void advanceAnimation(ph_float elapsedTime)
        {
            time = PHWarp(elapsedTime * PART_SPEED + time, M_PI * 2);
            for (int i = 0; i < NUM_PART; i++)
            {
                pv[i].color = PHColor(1, 1, 1, v[i].opacity * max(0.0f, sinf(time + v[i].offset)));
            }
        }

        virtual particles * calculatedParticles()
        {
            return &parts;
        }
};

class PHEarthViewController : public PHViewController, public PHDrawableInputDelegate
{
protected:
    PHMeshBody * earth, *athmos;
    PHImageView * background;
    PHBody * box;
    ph_float time;
    ESettings s;
    PHPerspectiveCamera * container;
    PHColor oldColor1, oldColor2;
    PHParticleView * stars;
    PHStarsAnimator * starsAnimator;

    bool fingerDown;
    ph_float lastFingerPos;
    ph_float fingerPos;
    ph_float currentSpeed;

    
    PHView * loadView(const PHRect & r)
    {
        fingerDown = false;
        currentSpeed = 0;

        container = new PHPerspectiveCamera();
        container->setNearClippingPlane(0.5f);
        container->setFarClippingPlane(50.0f);

        background = new PHImageView(r);
        background->setAutoresizeMask(PHView::ResizeAll);
        background->setAutoresizesSubviews(true);
        background->setUserInput(true);
        background->setInputDelegate(this);

        stars = new PHParticleView(PHRect(r.width * 0.5, r.height * 0.5, r.width * 0.5, r.height * 0.5)); 
        stars->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedRight | PHView::ResizeFlexibleWidth | PHView::ResizeFlexibleHeight);

        starsAnimator = new PHStarsAnimator();
        stars->setParticleAnimator(starsAnimator);
        stars->setImage(gm->imageNamed("star"));

        background->addChild(stars);
        background->addChild(container);

        box = new PHBody();

        earth = new PHMeshBody();
        earth->setMesh(PHSphereMesh::sphere(gm));

        athmos = new PHMeshBody();
        athmos->setMesh(PHSphereMesh::sphere(gm));
#define athmosScale 1.1
        athmos->setScale(PH3DSize(athmosScale, athmosScale, athmosScale));
        athmos->setMaterial(gm->materialNamed("atmosphere"));
        athmos->setBackFaceCullingEnabled(false);
        athmos->setBlendingEnabled(true);
        athmos->setZWritingEnabled(false);
        athmos->setRotation(PHQuaternion(M_PI_2, PHVector3(0.0,-1.0,0.0)));

        box->addChild(earth);
        box->addChild(athmos);

        container->addChild(box); 

        PHGLLight * l = new PHGLLight(PHGLLight::directionalLight, PH3DOriginPoint, 1.0f);
        gm->setCurrentLight(l);
        l->release();

        gm->messageWithName("reloadPreferences")->addListener(PHInvBind(this, PHEarthViewController::loadSettings, NULL));
        gm->messageWithName("wallpaperImageChanged")->addListener(PHInvBind(this, PHEarthViewController::setBackground, NULL));
        gm->messageWithName("reshapeWindow")->addListener(PHInvBind(this, PHEarthViewController::reshape, NULL));

        oldColor1 = PHInvalidColor; 
        oldColor2 = PHWhiteColor;

        loadSettings();
        reshape();
        
        return background;
    }

    void reshape()
    {
        PHRect f = stars->frame();
        float s = max(f.width, f.height);
        stars->setScalingCenter(PHOriginPoint);
        stars->setScale(PHSize(s, s));

        if (this->s.bgType == 2)
        {
            PHSize siz;
            float ar = gm->screenWidth() / gm->screenHeight();
            PHNormalImage * img = (PHNormalImage*)gm->imageNamed("starfield");
            float iar = (float)img->width() / (float)img->height();

            if (iar > ar)
            {
                siz.width = ar/iar;
                siz.height = 1.0f;
            } else {
                siz.width = 1.0f;
                siz.height = iar/ar;
            }
            background->setTextureCoordinates(PHRect((PHVector2(1.0f, 1.0f) - siz) * 0.5, siz));
        } else
            background->setTextureCoordinates(PHWholeRect);
    }

    PHGLTexture2D * alternateTexture()
    {
        PH2DCamera * cam = new PH2DCamera();
        PHNormalImage * img = (PHNormalImage*)gm->imageNamed("alt_earth");
        PHSize sz = PHSize(img->width(), img->height());
        cam->setScreenSize(sz);

        img->texture()->retain();
        return img->texture();

        PHImageView * iv = new PHImageView(PHRect(0, 0, sz.width, sz.height));
        iv->setImage(img);
        iv->setTintColor(s.color1);

        PHTextureCanvas * canvas = new PHTextureCanvas(gm);
        canvas->setTextureSize(sz);
        canvas->setColorFormat(PHGLTexture::RGBA8);
        canvas->setClearColor(s.color2);

        canvas->addChild(cam);
        cam->addChild(iv);

        canvas->render();

        PHGLTexture2D * tex = canvas->colorTexture();
        tex->retain();
        
        cam->release();
        canvas->release();
        iv->release();

        return tex;
    }

    void loadSettings()
    {
        s.load();
        box->setPosition(s.position);
        box->setScale(PH3DSize(s.size, s.size, s.size));
        box->setRotation(PHQuaternion::fromEuler(0, s.earthPitch, s.earthRoll));
        container->setRotation(PHQuaternion::fromEuler(0, s.cameraRoll, 0));
        string opts("earth[");
        if (s.normalMapStrength > 0.01)
        {
            opts += "normalMap = true, ";
            earth->additionalUniforms()->at("normalMapStrength") = s.normalMapStrength;
        }
        if (s.specular)
            opts += "specular = true, ";
        if (s.night)
            opts += "night = true, ";
        opts += "]";
        athmos->additionalUniforms()->at("color") = s.athmosColor;
        athmos->setHidden(s.athmosColor.a < 0.01);
        earth->setMaterial(gm->materialNamed(opts));
        gm->setWindowClearColor(s.bgColor);
        stars->setHidden(!s.stars);

        if (s.staticSun)
        {
            float sunAngle = s.sunAngle - M_PI_2;
            gm->currentLight()->position = PH3DPoint(cos(sunAngle), 0, - sin(sunAngle));
        }
        setBackground();

        PHColor c1 = s.color1, c2 = s.color2;
        if (!s.mapType)
            c1 = c2 = PHInvalidColor;
        if (c1 != oldColor1 || c2 != oldColor2)
        {
            oldColor1 = c1; 
            oldColor2 = c2;
            PHGLUniformStates::uniform & u = earth->additionalUniforms()->at("tex");
            if (s.mapType)
            {
                PHGLTexture2D * tex = alternateTexture();
                u = tex;
                tex->release();
            } else
                u = ((PHNormalImage*)gm->imageNamed("earth"))->texture();
        }

        int fps = 30;
        switch (s.fpsType)
        {
            case 0:
                fps = 20;
                break;
            case 1:
                fps = 30;
                break;
            case 2:
                fps = 60;
                break;
        }

#ifdef PH_LIVEPAPERS
        LPPHSetFPS(gm, fps);
        LPPHClearWallpaperImage(gm);
#endif
        gm->collectGarbageResources();
    }

    void setBackground()
    {
        PHImage * img = NULL;
        switch (s.bgType)
        {
            case 1:
#ifdef PH_LIVEPAPERS
                img = LPPHGetWallpaperImage(gm);
#endif
                break;
            case 2:
                img = gm->imageNamed("starfield");
                break;
        }
        background->setImage(img);
        reshape();
    }

    virtual void drawableRecievedEvent(PHDrawable * d, PHEvent * evt)
    {
        switch (evt->type())
        {
            case PHEvent::touchDown:
                if (!fingerDown)
                {
                    fingerDown = true;
                    evt->setHandled(true);
                    fingerPos = lastFingerPos = evt->location().x;
                }
                break;
            case PHEvent::touchCancelled:
            case PHEvent::touchUp:
                fingerDown = false;
            case PHEvent::touchMoved:
                fingerPos = evt->location().x;
                break;
        }
    }
    
    void updateScene(ph_float timeElapsed)
    {
        earth->setRotation(PHQuaternion(time, PHVector3(0.0f, 1.0f, 0.0f)));
        if (fingerDown)
        {
            currentSpeed = (fingerPos - lastFingerPos) / timeElapsed;
            lastFingerPos = fingerPos;
        }
        else
            PHLowPassFilter(currentSpeed, s.rotationSpeed, timeElapsed, 5.0f);
        time = PHWarp(time + timeElapsed * currentSpeed, M_PI * 2);

        if (!s.staticSun)
        {
            time_t tim = ::time(NULL);
            const struct tm * lt;
            lt = localtime(&tim);
            float sunAngle = (lt->tm_hour * 60 + lt->tm_min) * (2 * M_PI / (24*60)) + M_PI_2;
            gm->currentLight()->position = PH3DPoint(cos(sunAngle), 0, - sin(sunAngle));
        }
    }
    
public:
    PHEarthViewController(PHGameManager * gm) : PHViewController(gm), earth(NULL), athmos(NULL), time(0) {}
    ~PHEarthViewController()
    {
        if (container)
            container->release();
        if (athmos)
            athmos->release();
        if (earth)
            earth->release();
        if (box)
            box->release();
        if (stars)
            stars->release();
        if (starsAnimator)
            starsAnimator->release();
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHEarthViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    #ifdef PH_DEBUG
    gm->setShowsFPS(true);
    #endif
    vc->release();
}

PHMAIN_DEFINE {
    srand(time(NULL));
    return PHWMain(PHMAIN_ARGS, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), PHWResizable | PHWVSync | PHWDepthBuffer | PHWFrameAnimation, &PHGameEntryPoint,NULL);
}
 
