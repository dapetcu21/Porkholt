//
//  PHChapterController.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef INSIDE_PHVIEWCONTROLLER_H
#ifndef PHCHAPTERCONTROLLER_H

class PHChapterController : public PHViewController
{
private:
    string path;
    PHImage * bg;
protected:
    virtual PHView * loadView(const PHRect & frame);
public:
    virtual void updateScene(double timeElapsed);
    PHChapterController(const string & path);
    virtual ~PHChapterController();
    
	void mouseUp(PHObject * sender, void * ud);
};

#endif
#endif