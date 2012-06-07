/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHBODY_H
#define PHBODY_H

#include "PHDrawable.h"

class PHBody : public PHDrawable
{
private:
    PH3DPoint pos;
    PHQuaternion rot;
    PH3DSize scl;
    PH3DPoint _rotationalCenter, _scalingCenter;
    int effOrder;
    PHMatrix matrixCache, effectCache, customEffect;
    bool matrixCached, effectCached;
    
public:
    PHBody();
    ~PHBody();
    
    PH3DPoint position() { return pos; }
    PH3DSize scale() { return scl; }
    PHQuaternion rotation() { return rot; }
    void setPosition(const PH3DPoint & p);
    void setRotation(const PHQuaternion & r);
    void setScale(const PH3DSize & s);
    
    enum Effects
    {
        EffectScale = 1,
        EffectRotate = 2,
        EffectCustom = 3
    };
    
	enum EffectOrder
	{
        EffectOrderScaleRotate = EffectScale | EffectRotate<<2,
        EffectOrderRotateScale = EffectRotate | EffectScale<<2,
		EffectOrderScaleRotateCustom = EffectScale | EffectRotate<<2 | EffectCustom<<4,
		EffectOrderRotateScaleCustom = EffectRotate | EffectScale<<2 | EffectCustom<<4,
        EffectOrderScaleCustomRotate = EffectScale | EffectCustom<<2 | EffectRotate<<4,
        EffectOrderRotateCustomScale = EffectRotate | EffectCustom<<2 | EffectScale<<4,
        EffectOrderCustomScaleRotate = EffectCustom | EffectScale<<2 | EffectRotate<<4,
		EffectOrderCustomFlipRotateScale = EffectCustom | EffectRotate<<2 | EffectScale<<4
	};
    
    
    int effectOrder() { return effOrder; };
	void setEffectOrder(int eff) { effOrder = eff; effectCached = false; };
    PH3DPoint rotationalCenter() { return _rotationalCenter; }
    void setRotationalCenter(const PH3DPoint &center) { _rotationalCenter = center; effectCached = false; }
    PH3DPoint scalingCenter() { return _scalingCenter; }
	void setScalingCenter(const PH3DPoint &center) { _scalingCenter = center; effectCached = false; }
    PHMatrix customMatrix() { return customEffect; }
    void setCustomMatrix(const PHMatrix & c) { customEffect = c; effectCached = false; }
	
    PHMatrix applyMatrices();
    virtual void render();
};

#endif
