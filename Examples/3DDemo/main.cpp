/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/3D/PHProjectionChanger.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHImageView.h>

class PH3DDemoViewController : public PHViewController
{
protected:
    PHMeshBody * lbody, *body;
    ph_float time;
    
    PHView * loadView(const PHRect & r)
    {
        PHProjectionChanger * container = new PHProjectionChanger(PHMatrix::perspective(M_PI/4, gm->screenWidth()/gm->screenHeight(), 0.5f, 50.0f));
        PHView * v = new PHView(r);
        v->addChild(container);
        container->release();

        body = new PHMeshBody();
        body->setMesh(PHSphereMesh::sphere(gm));
        body->setPosition(PH3DPoint(0,0,-5));
        PHMaterial * mat = new PHMaterial(PHWhiteColor, PHWhiteColor,10.0f);
        body->setMaterial(mat);
        mat->release();
        PHNormalImage * img = (PHNormalImage*)gm->imageNamed("earth", true);
        body->setImage(img);
        container->addChild(body);

        PHImageView * iv = new PHImageView(PHRect(0,0,200,100));
        iv->setImage(img);
        v->addChild(iv);
        iv->release();
        
        iv = new PHImageView(PHRect(200,0,200,100));
        iv->setImage(img->normalMap());
        v->addChild(iv);
        iv->release();

        lbody = new PHMeshBody();
        lbody->setMesh(PHSphereMesh::sphere(gm));
        lbody->setScale(PH3DSize(0.2,0.2,0.2));
        mat = new PHMaterial(PHClearColor, PHClearColor, 0, PHWhiteColor);
        lbody->setMaterial(mat);
        mat->release();
        container->addChild(lbody);
        
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
    return PHWMain(argc, argv, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), PHWResizable | PHWVSync | PHWDepthBuffer, &PHGameEntryPoint,NULL);
}
 
