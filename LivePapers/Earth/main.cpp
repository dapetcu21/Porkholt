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

#include <ctime>

#include "ESettings.h"

class PHEarthViewController : public PHViewController
{
protected:
    PHMeshBody * earth, *athmos;
    PHImageView * background;
    PHBody * box;
    ph_float time;
    ESettings s;
    PHPerspectiveCamera * container;
    PHColor oldColor1, oldColor2;
    
    PHView * loadView(const PHRect & r)
    {
        container = new PHPerspectiveCamera();
        container->setNearClippingPlane(0.5f);
        container->setFarClippingPlane(50.0f);

        background = new PHImageView(r);
        background->setAutoresizeMask(PHView::ResizeAll);
        background->setUserInput(true);
        background->addChild(container);

        box = new PHBody();

        earth = new PHMeshBody();
        earth->setMesh(PHSphereMesh::sphere(gm));
        PHNormalImage * img = (PHNormalImage*)gm->imageNamed("earth", true);

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
        loadSettings();

        oldColor1 = PHInvalidColor; 
        oldColor2 = PHWhiteColor;
        
        return background;
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
#ifdef PH_LIVEPAPERS
        background->setImage(s.bgType ? LPPHGetWallpaperImage(gm) : NULL);
#endif
    }
    
    void updateScene(ph_float timeElapsed)
    {
        earth->setRotation(PHQuaternion(time, PHVector3(0.0,-1.0,0.0)));
        time = PHWarp(time + timeElapsed * s.rotationSpeed, M_PI * 2);

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
    return PHWMain(PHMAIN_ARGS, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), PHWResizable | PHWVSync | PHWDepthBuffer | PHWFrameAnimation, &PHGameEntryPoint,NULL);
}
 
