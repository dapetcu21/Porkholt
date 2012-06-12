/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHKeyframeAnimator.h>
#include <Porkholt/UI/PHTestViewController.h>
#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHMotion.h>

void PHTestViewController::callback1(PHObject * sender, void * ud)
{
	if (navController)
    {
        int anim = (int)(size_t)ud;
		navController->popViewController(anim);
    }
}

void PHTestViewController::callback2(PHObject * sender, void * ud)
{
	PHTestViewController * vc = new PHTestViewController();
	vc->init(gm);
	if (navController)
    {
        int anim = (int)(size_t)ud;
		navController->pushViewController(vc,anim);
    }
	vc->release();
}

void PHTestViewController::callback3(PHObject * sender, void * ud)
{
    PHLog("callback3: %x %x", (unsigned)(unsigned long long)sender, (unsigned int)(unsigned long long)ud);
}

PHView * PHTestViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	
	view2 = new PHTestView(PHRect(50, 50, 100, 100));
	view4 = new PHTestView(PHRect(150, 150, 100, 100));
	PHView * view3 = new PHTestView(PHRect(0,0,30,30));
	
	view3->setCenter(view2->boundsCenter());
	view3->setUserInput(true);
	view2->addChild(view3);
	view3->release();
	
	view2->setUserInput(true);
	view->addChild(view2);
	view2->release();
	
	view4->setUserInput(true);
	view->addChild(view4);
    
    vector<PHKeyframeAnimator::Keyframe> k;
    k.push_back(PHKeyframeAnimator::positionFrame(1.0f, PHPoint(200,150)));
    k.back().addRotation(0);
    
    k.push_back(PHKeyframeAnimator::rotationFrame(1.5f, M_PI_2));
    k.back().addPosition(PHPoint(200,150));
    
    k.push_back(PHKeyframeAnimator::jumpFrame(2.0f, 1, 0.0f));
    
    k.push_back(PHKeyframeAnimator::positionFrame(2.5f, PHPoint(200,200)));
    k.back().addRotation(M_PI_2);
    
    k.push_back(PHKeyframeAnimator::rotationFrame(3.0f, M_PI));
    k.back().addPosition(PHPoint(200,200));
    
    k.push_back(PHKeyframeAnimator::positionFrame(4.0f, PHPoint(150,200)));
    k.back().addRotation(M_PI);
    
    k.push_back(PHKeyframeAnimator::rotationFrame(4.5f, M_PI_2*3));
    k.back().addPosition(PHPoint(150,200));
    
    k.push_back(PHKeyframeAnimator::positionFrame(5.5f, PHPoint(150,150)));
    k.back().addRotation(M_PI_2*3);
    
    k.push_back(PHKeyframeAnimator::rotationFrame(6.0f, M_PI*2));
    k.back().addPosition(PHPoint(150,150));
    
    vector<PHKeyframeAnimator::Section> s;
    s.push_back(PHKeyframeAnimator::Section("default",k));
    
    k.clear();
    k.push_back(PHKeyframeAnimator::positionFrame(1.0f, PHPoint(175,175)));
    k.push_back(PHKeyframeAnimator::jumpFrame(1.0f, 0, 2.001f));
    s.push_back(PHKeyframeAnimator::Section("decoy",k));
    
    PHKeyframeAnimator * ka = new PHKeyframeAnimator(s);
    view4->addCinematicAnimation(ka);
    ka->playSection("default", PHInv(this, PHTestViewController::callback3, NULL));
    ka->release();
	view4->release();
	
	
	for (int i=0; i<PHNavigationController::NUMANIMATIONS; i++)
	{
		PHButtonView * button1 = new PHButtonView(PHRect(i*60+0, 0, 25, 40));
		PHButtonView * button2 = new PHButtonView(PHRect(i*60+25, 0, 25, 40));
		button1->setImage(gm->imageNamed("stop"));
		button1->setPressedImage(gm->imageNamed("start"));
	
		button2->setPressedImage(gm->imageNamed("stop"));
		button2->setImage(gm->imageNamed("start"));
	
		button1->setUpCallback(PHInv(this, PHTestViewController::callback1, (void*)i));
		button2->setUpCallback(PHInv(this, PHTestViewController::callback2, (void*)i));
	
		view->addChild(button1);
		view->addChild(button2);
	
		button1->release();
		button2->release();
	}
	
	return view;
}

void PHTestViewController::updateScene(ph_float timeElapsed)
{
	PHTilt tilt = PHMotion::sharedInstance()->getTilt();
	view2->setRotation(-tilt.roll); // "geostationary" view
	//view4->setRotation(tilt.pitch);
}
