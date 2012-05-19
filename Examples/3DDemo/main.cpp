//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#include "PHWindowing.h"
#include "PHTestView.h"
#include "PHViewController.h"
#include "PHMeshBody.h"
#include "PHSphereMesh.h"
#include "PHProjectionChanger.h"
#include "PHNormalImage.h"

class PH3DDemoViewController : public PHViewController
{
protected:
    PHMeshBody * lbody, *body;
    ph_float time;
    
    PHView * loadView(const PHRect & r)
    {
        PHProjectionChanger * container = new PHProjectionChanger(PHMatrix::perspective(M_PI/4, gm->screenWidth()/gm->screenHeight(), 0.5f, 50.0f));

        body = new PHMeshBody();
        body->setMesh(PHSphereMesh::sphere(gm));
        body->setPosition(PH3DPoint(0,0,-5));
        PHMaterial * mat = new PHMaterial(PHWhiteColor, PHWhiteColor,10.0f);
        body->setMaterial(mat);
        mat->release();
        body->setImage(gm->imageNamed("earth"));
        container->addChild(body);
        
        lbody = new PHMeshBody();
        lbody->setMesh(PHSphereMesh::sphere(gm));
        lbody->setScale(PH3DSize(0.2,0.2,0.2));
        mat = new PHMaterial(PHClearColor, PHClearColor, 0, PHWhiteColor);
        lbody->setMaterial(mat);
        mat->release();
        container->addChild(lbody);
        
        PHView * v = new PHView(r);
        v->addChild(container);
        container->release();
        
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PH3DOriginPoint, 2);
        gm->setCurrentLight(l);
        l->release();
        gm->setAmbientColor(PHColor(0.5,0.5,0.5));
        
        return v;
    }
    
    void updateScene(ph_float timeElapsed)
    {
        PH3DPoint p(0 + sin(time)*2.5, 0, -5 + cos(time)*2.5);
        lbody->setPosition(p);
        body->setRotation(PHQuaternion(time/3, PHVector3(0.0,-1.0,0.0)));
        gm->currentLight()->position = p;
        time += timeElapsed * 2;
    }
    
public:
    PH3DDemoViewController() : lbody(NULL), time(0) {}
    ~PH3DDemoViewController()
    {
        if (lbody)
            lbody->release();
        if (body)
            body->release();
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PH3DDemoViewController();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWVideoMode(800, 600, 60, PHWVideoMode::FullscreenWindowed), PHWResizable | PHWVSync, &PHGameEntryPoint,NULL);
}
 
