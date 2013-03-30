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

class PHRotateBall : public PHDrawable
{
protected:
    PHBody * _body;
    PHVector3 lastPoint;
    PHVector3 axis;
    float rotSpeed;
    bool manual;
public:

    PHRotateBall() : _body(NULL), manual(false), axis(0,0,1), rotSpeed(0) {};
    ~PHRotateBall() 
    { 
        if (_body)
            _body->release();
    }

    void setBody(PHBody * b) 
    {
        if (b)
            b->retain();
        if (_body)
            _body->release();
        _body = b;
    }

    bool manualRotation() { return manual; }

    void touchEvent(PHEvent * evt)
    {
        if (!_body) return;
        PHPositionalVector v = evt->drawableLocation()->positionInDrawable(_body);

        //intersecting a line with a sphere
        PH3DPoint s = v.start, t = v.end;
        PH3DPoint r;
        double a=0.0f, b=0.0f, c=-1.0f;

        if (s.x == t.x) { r.x = s.x; c+=r.x*r.x; }
        else { a += (t.x-s.x)*(t.x-s.x); b += 2*s.x*(t.x-s.x); c+= s.x*s.x; }
        if (s.y == t.y) { r.y = s.y; c+=r.y*r.y; }
        else { a += (t.y-s.y)*(t.y-s.y); b += 2*s.y*(t.y-s.y); c+= s.y*s.y; }
        if (s.z == t.z) { r.z = s.z; c+=r.z*r.z; }
        else { a += (t.z-s.z)*(t.z-s.z); b += 2*s.z*(t.z-s.z); c+= s.z*s.z; }

        double delta = b*b - 4*a*c;

        if (delta >= 0)
        {
            double rdelta = sqrt(delta);
            double k1 = (-b + rdelta)/(2*a), k2 = (-b - rdelta)/(2*a);
            if (k2 < k1)
                k1 = k2;
            if (s.x != t.x) r.x = k1*(t.x-s.x) + s.x; 
            if (s.y != t.y) r.y = k1*(t.y-s.y) + s.y; 
            if (s.z != t.z) r.z = k1*(t.z-s.z) + s.z; 

            if (!manual)
                lastPoint = r;
            manual = true;

            PHQuaternion q;
            switch (evt->type())
            {
                case PHEvent::touchDown:
                    evt->setHandled(true);
                    lastPoint = r;
                    break;
                case PHEvent::touchUp:
                    rotSpeed = 0;
                    q = PHQuaternion::fromPointsOnSphere(lastPoint, r);
                    _body->setRotation(_body->rotation() * q);
                    break;
                case PHEvent::touchMoved:
                    q = PHQuaternion::fromPointsOnSphere(lastPoint, r);
                    _body->setRotation(_body->rotation() * q);
                    axis = q.rotationAxis();
                    rotSpeed = q.rotationAngle() / (evt->time() - evt->lastTime());
                    if (rotSpeed > 15)
                        rotSpeed = 15;
            };
        }

        
        if (delta < 0 || evt->type() == PHEvent::touchUp)
        {
            manual = false;
        }
    }

    void updateScene(float elapsed)
    {
        if (_body && !manual && rotSpeed>0 && rotSpeed < 1000000000000.0f)
        {
            _body->setRotation(_body->rotation() * PHQuaternion(rotSpeed * elapsed, axis));
            PHLowPassFilter(rotSpeed, 0, elapsed, 3.0f);
        } 
    }
};

class PH3DDemoViewController : public PHViewController
{
protected:
    PHMeshBody * lbody, *body;
    float time;
    PHRotateBall * rotator;
    
    PHView * loadView(const PHRect & r)
    {
        PHPerspectiveCamera * container = new PHPerspectiveCamera();
        container->setNearClippingPlane(0.5f);
        container->setFarClippingPlane(50.0f);

        PHView * v = new PHView(r);
        v->setUserInput(true);
        v->addChild(container);
        container->release();

        body = new PHMeshBody();
        body->setMesh(PHSphereMesh::sphere(gm));
        body->setPosition(PH3DPoint(0,0,-5));
        body->setMaterial(gm->materialNamed("chestie_albastra"));
        PHNormalImage * img = (PHNormalImage*)gm->imageNamed("earth", true);
        container->addChild(body);

        rotator = new PHRotateBall();
        rotator->setBody(body);
        rotator->setUserInput(true);
        container->addChild(rotator);
        rotator->release();

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
        //body->setRotation(PHQuaternion(time/3, PHVector3(0.0,-1.0,0.0)));
        gm->currentLight()->position = p;
        rotator->updateScene(timeElapsed);
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
 
