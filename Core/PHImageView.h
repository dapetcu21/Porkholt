/*
 *  PHImageView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H

#include "PHView.h"
#include "PHImage.h"

class PHImage;
class PHImageAnimator;
class PHAnimatorPool;

class PHImageView : public PHView
{
protected:
	PHImage * _image;
    PHImageAnimator * _animator;
	PHRect coords;
    PHColor tint;
    bool constrain;
    double _repeatX, _repeatY;
    
    void renderInFramePortionTint(const PHRect & fr, const PHRect & coords, const PHColor & clr);
    void renderCurved();
    void renderStraight();
    
    PHAnimatorPool * pool;
    
    void bezierCallback(PHBezierPath * sender, void *ud);
	void rebuildCurvedVBO();
    void rebuildStraightVBO();
    void rebuildVBO()
    {
        if (curve)
            rebuildCurvedVBO();
        else
            rebuildStraightVBO();
    }
    bool animatorNeedsVBORebuild();
    
    GLfloat * interleavedArrayFromAnchorList(const void * anchors, int &n);
    void textureCoordinatesFromAnchorList(GLfloat * buffer, size_t stride, const void * anchors);
    
    PHBezierPath * curve;
    
    GLuint arraysVBO,indexesVBO;
    int nVertices;
    int nIndexes;
    bool VBOneedsRebuilding;
    PHImage::VBOParams params1,params2;
    
    int lastAnimFrame,animFrame;
    
    void loadVBO()
    {
        if (VBOneedsRebuilding)
        {
            rebuildVBO();
            VBOneedsRebuilding = false;
        }
    }
    
public:
    PHAnimatorPool * animatorPool() { return pool; }
    void setAnimatorPool(PHAnimatorPool * p);
	PHImage * image() { return _image; };
    PHImageAnimator * animator() { return _animator; }
	void setImage(PHImage * image);
	PHImageView();
	PHImageView(const PHRect &frame);
	PHImageView(PHImage * image);
	virtual ~PHImageView();
	virtual void draw();
    virtual void render() { PHView::render();};
    
    const PHRect & textureCoordinates() { return coords; };
	void setTextureCoordinates(const PHRect & r) { coords = r; VBOneedsRebuilding = true; };
    
    PHColor & tintColor() { return tint; };
    void setTintColor(const PHColor & clr) { tint = clr; VBOneedsRebuilding = true; };
    
    virtual const PHColor & animatedColor() { return tint; }
    virtual void setAnimatedColor(const PHColor & c) { setTintColor(c); }
    
    bool constrainCurveToFrame() { return constrain; }
    void setConstrainCurveToFrame(bool c) { constrain = c; }
    
    double repeatX() { return _repeatX; }
    void setRepeatX(double rx) { _repeatX = rx; VBOneedsRebuilding = true; }
    
    double repeatY() { return _repeatY; }
    void setRepeatY(double ry) { _repeatY = ry; VBOneedsRebuilding = true; }
    
    PHBezierPath * bezierPath() { return curve; }
    void setBezierPath(PHBezierPath * bp);
    
    static PHImageView * imageFromLua(lua_State * L,const string & rootPath);
    static PHImageView * imageFromLua(lua_State * L,const string & root, PHAnimatorPool * pool);
    static PHImageView * imageFromClass(const string & clss);
  
    static void registerLuaInterface(lua_State * L);
};

#endif