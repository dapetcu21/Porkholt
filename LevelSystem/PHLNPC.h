//
//  PHLNPC.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/15/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLNPC_H
#define PHLNPC_H

#include "PHLObject.h"

class PHTrailImageView;

class PHLNPC : public PHLObject
{
protected:
    PHImageView * bodyView;
    PHImageView * faceView;
    
private:
    bool trail,utrail;
    int traillen;
    bool staticFace;
    PHView * worldView;
    bool fflip,bflip,aflip,flipped;
    bool trailPossible;
public:
    PHLNPC();
    virtual ~PHLNPC();
    
    void setTrail(bool tr) { trail = tr; }
    bool hasTrail() { return trail; }
    void setUsesTrail(bool tr) { utrail = tr; }
    bool usesTrail() { return utrail; }
    void setFaceFlipping(bool tr) { fflip = tr; }
    bool faceFlipping() { return fflip; }
    void setBodyFlipping(bool tr) { bflip = tr; }
    bool bodyFlipping() { return bflip; }
    void setAutomaticFlipping(bool tr) { aflip = tr; }
    bool automaticFlipping() { return aflip; }
    void setTrailLength(int len) { traillen = len; }
    int trailLength() { return traillen; }
    void flip();
    bool isFlipped() {  return flipped; }
    void setFlipped(bool sf) { if (flipped!=sf) flip(); }
    
    virtual void updatePosition();
    virtual void updateView();
    virtual void loadView();
    virtual void loadFromLua(lua_State * L, const string & root,b2World * world);
    static void registerLuaInterface(lua_State * L);
};

#endif