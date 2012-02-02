//
//  PHTrailImageView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//


#ifndef PHTRAILIMAGEVIEW_H
#define PHTRAILIMAGEVIEW_H

#include "PHImageView.h"

class PHTrailImageView : public PHImageView
{
protected:
    struct pframe
    {
        PHMatrix m;
        PHColor tint;
    };
    int trail;
    int snap;
    int ssnap;
    list<pframe> frames;
    
    PHView * _stopView;
    PHImage * auxImg;
    
    void saveState(pframe & fr);
    const PHMatrix & loadState(const pframe & fr);
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
    int actualTrailSize() { return (int)(frames.size()); }
    
    void setAuxImage(PHImage * img) { if (auxImg) auxImg->release(); auxImg=img; if (auxImg) auxImg->retain(); }
    PHImage * auxImage() { return auxImg; }
    
    void pushFrame();
};

#endif