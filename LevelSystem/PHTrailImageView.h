//
//  PHTrailImageView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Home. All rights reserved.
//


#ifndef INSIDE_PHVIEW_H
#ifndef INSIDE_PHIMAGEVIEW_H
#ifndef PHTRAILIMAGEVIEW_H
#define PHTRAILIMAGEVIEW_H

#include "PHMain.h"

class PHTrailImageView : public PHImageView
{
protected:
    struct pframe
    {
        GLfloat m[16];
        PHColor tint;
    };
    int trail;
    int snap;
    int ssnap;
    list<pframe> frames;
    
    PHView * _stopView;
    
    void saveState(pframe & fr);
    void loadState(const pframe & fr);
    void saveMinState(pframe & fr);
    void loadMinState(const pframe & fr);
public:
	PHTrailImageView();
	PHTrailImageView(const PHRect &frame);
	PHTrailImageView(PHImage * image);
	virtual ~PHTrailImageView();
    virtual void render();
    virtual void auxRender();
    
    int trailSize() { return trail; }
    void setTrailSize(int tr) { trail = tr; }
    int snapshotInterval() { return snap; }
    void setSnapshotInterval(int s) { snap = s; }
    PHView * stopView() { return _stopView; }
    void setStopView(PHView * vw) { _stopView = vw; frames.clear(); }
    
    void pushFrame();
};

#endif
#endif
#endif