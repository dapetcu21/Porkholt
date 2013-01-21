/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSound.h>

class PHAnimTest : public PHViewController
{
    public:
        PHAnimTest(PHGameManager * gm) : PHViewController(gm) {}

        PHView * loadView(const PHRect & r)
        {
            PHView * v = new PHView(r);
            PHPerspectiveCamera * camera = new PHPerspectiveCamera();
            camera->setNearClippingPlane(0.5f);
            camera->setFarClippingPlane(50.0f);

            gm->setAmbientColor(PHColor(0.5, 0.5, 0.5));

            PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PH3DPoint(1.25f, 0, 1.25f - 5), 1.5);
            gm->setCurrentLight(l);
            l->release();

            PHMeshBody * body = new PHMeshBody();
            body->setMesh(PHSphereMesh::sphere(gm));
            body->setPosition(PH3DPoint(0,0,-5));
            body->setMaterial(gm->materialNamed("chestie_albastra"));
            camera->addChild(body);

            v->addChild(camera);
            camera->release();

            body->beginCinematicAnimation(1.0f);
            body->animateMove(PHVector3(-1.5, 0, 0));
            body->animateScale(PHVector3(0.5, 0.5, 0.5));
            body->animateRotate(PHQuaternion(M_PI, PHVector3(0, 1, 0)));
            body->chainCinematicAnimation(2.0f);
            body->animateMove(PHVector3(3, 0, 0));
            body->animateScale(PHVector3(4, 4, 4));
            body->chainCinematicAnimation(1.0f);
            body->animateMove(PHVector3(-1.5, 0, 0));
            body->animateScale(PHVector3(0.5, 0.5, 0.5));
            body->commitCinematicAnimation();

            body->beginCinematicAnimation(10.0f);
            body->commitCinematicAnimation();

            //------------------------

            PHImageView * iv = new PHImageView(PHRect(0, 0, 200, 100));
            iv->setImage(gm->imageNamed("earth"));
            v->addChild(iv);
            iv->release();

            iv->beginCinematicAnimation(2.0f);
            iv->animateMove(PHPoint(200, 200));
            iv->animateScale(PHSize(0.25, 0.25));
            iv->chainCinematicAnimation(0.25f);
            iv->animateTint(PHClearColor);
            iv->chainCinematicAnimation(0.25f);
            iv->animateTint(PHColor(1.0f, 0.5f, 0.5f));
            iv->chainCinematicAnimation(2.0f);
            iv->animateField(PHAnimationField(PHCinematicActor::fieldMove, PHPoint(0, 0)));
            iv->animateField(PHAnimationField(PHCinematicActor::fieldScale, PHSize(1, 1), false, true));
            iv->commitCinematicAnimation();

            //------------------------

            PHSound * snd = gm->soundManager()->soundNamed("test");
            gm->soundManager()->setBackgroundMusic(snd);

            snd->beginCinematicAnimation(3.0f);
            snd->chainCinematicAnimation(1.0f);
            snd->animateGain(0);
            snd->chainCinematicAnimation(1.0f);
            snd->animateGain(1);
            snd->chainCinematicAnimation(3.0f);
            snd->chainCinematicAnimation(1.0f);
            snd->animatePitch(2.0f);
            snd->chainCinematicAnimation(0.5f);
            snd->chainCinematicAnimation(1.0f);
            snd->animatePitch(0.5f);
            snd->chainCinematicAnimation(0.5f);
            snd->chainCinematicAnimation(1.0f);
            snd->animatePitch(1.0f);
            snd->commitCinematicAnimation();

            return v;
        }

};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHAnimTest(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DECLARE(PHWGLES1 | PHWVSync | PHWShowFPS | PHWDepthBuffer, &PHGameEntryPoint,NULL);
 
