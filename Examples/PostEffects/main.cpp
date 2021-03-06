/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHPostProcess.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>

class PH3DDemoViewController : public PHViewController
{
protected:
    PHMeshBody * lbody, *body;
    float time;
    
    PHView * loadView(const PHRect & r)
    {
        PHView * v = new PHView(r);
        
        PHPostProcess * canvas = new PHPostProcess(gm);
        v->addChild(canvas);
        canvas->setColorFormat(PHGLFBOAttachment::RGBA8);
        canvas->setDepthFormat(PHGLFBOAttachment::Depth16);
        canvas->setMaterial(gm->materialNamed("cell_shading"));
        canvas->additionalUniforms()->at("texture").setValue(canvas->colorTexture());       
        PHGLTexture1D * cell_map = new PHGLTexture1D(gm);
        #define cells 5
        uint8_t b[cells];
        for (int i = 0; i<cells; i++)
            b[i] = i * (1.0f/(cells-1)) * 255;
        cell_map->setData(b, cells, PHGLTexture::R8);
        cell_map->setWrapS(PHGLTexture::clampToEdge);
        cell_map->setMinFilter(PHGLTexture::nearest);
        cell_map->setMagFilter(PHGLTexture::nearest);
        canvas->additionalUniforms()->at("cellmap").setValue(cell_map);
        cell_map->release();

        canvas->setEffectEnabled(true);
        

        PHPerspectiveCamera * container = new PHPerspectiveCamera();
        container->setNearClippingPlane(0.5f);
        container->setFarClippingPlane(50.0f);
        canvas->addChild(container);
        canvas->release();
        container->release();

        body = new PHMeshBody();
        body->setMesh(PHSphereMesh::sphere(gm));
        body->setPosition(PH3DPoint(0,0,-5));
        body->setMaterial(gm->materialNamed("chestie_albastra"));
        container->addChild(body);

        PHImage * img = gm->imageNamed("earth", true);
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
        lbody->setMaterial(gm->materialNamed("chestie_alba"));
        container->addChild(lbody);
        
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PH3DOriginPoint, 2);
        gm->setCurrentLight(l);
        l->release();
        gm->setAmbientColor(PHColor(0.5,0.5,0.5));
        
        return v;
    }
    
    void updateScene(float timeElapsed)
    {
        PH3DPoint p(0 + sin(time)*2.5, 0, -5 + cos(time)*2.5);
        lbody->setPosition(p);
        body->setRotation(PHQuaternion(time/3, PHVector3(0.0,-1.0,0.0)));
        gm->currentLight()->position = p;
        time += timeElapsed * 2;
    }
    
public:
    PH3DDemoViewController(PHGameManager * gm) : PHViewController(gm), lbody(NULL), time(0) {}
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
    PHViewController * vc = new PH3DDemoViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), PHWResizable | PHWVSync | PHWDepthBuffer, &PHGameEntryPoint,NULL);
}
 
