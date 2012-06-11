/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHImage.h>

class PHImage;
class PHImageAnimator;
class PHAnimatorPool;
class PHCurve;

#define PH_REGISTERIMAGEVIEW(clss) PH_REGISTERCLASS(PHImageView::initMap, #clss, clss)

class PHImageView : public PHView
{
protected:
	PHImage * _image;
    PHImageAnimator * _animator;
	PHRect coords;
    PHColor tint;
    bool constrain;
    ph_float _repeatX, _repeatY;
    
    void renderInFramePortionTint(const PHRect & fr, const PHRect & coords, const PHColor & clr);
    void renderCurved();
    void renderStraight();
    
    PHAnimatorPool * pool;
    
    void curveCallback(PHCurve * sender, void *ud);
	void rebuildCurvedVBO();
    void rebuildStraightVBO();
    void destroyVAOs();
    void destroyStraightVAO();
    void destroyCurvedVAO();
    void rebuildVBO()
    {
        if (curve)
            rebuildCurvedVBO();
        else
            rebuildStraightVBO();
    }
    bool animatorNeedsVBORebuild();
    
    PHCurve * curve;
    
    bool VBOneedsRebuilding;
    
    PHGLVertexArrayObject * curveVAO;
    PHGLVertexBufferObject * curveAttributeVBO, * curveElementVBO;
    
    PHGLVertexArrayObject * straightVAO1, * straightVAO2;
    PHGLVertexBufferObject * straightVBO1, * straightVBO2;
    
    int lastAnimFrame,animFrame;
    
    void loadVBO()
    {
        if (VBOneedsRebuilding)
        {
            rebuildVBO();
            VBOneedsRebuilding = false;
        }
    }
    
    bool _normalMapping;
    
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
    bool supportsRenderMode(int rm);
    
    void setNormalMapping(bool nm) { _normalMapping = nm; }
    bool normalMapping() { return _normalMapping; }
    
    const PHRect & textureCoordinates() { return coords; };
	void setTextureCoordinates(const PHRect & r) { coords = r; VBOneedsRebuilding = true; };
    
    PHColor & tintColor() { return tint; };
    void setTintColor(const PHColor & clr) { tint = clr; };
    
    virtual PHColor cinematicCustomColor() { return tint; }
    virtual void setCinematicCustomColor(const PHColor & c) { setTintColor(c); }
    
    bool constrainCurveToFrame() { return constrain; }
    void setConstrainCurveToFrame(bool c) { constrain = c; }
    
    ph_float repeatX() { return _repeatX; }
    void setRepeatX(ph_float rx) { _repeatX = rx; VBOneedsRebuilding = true; }
    
    ph_float repeatY() { return _repeatY; }
    void setRepeatY(ph_float ry) { _repeatY = ry; VBOneedsRebuilding = true; }
    
    PHCurve * shape() { return curve; }
    void setShape(PHCurve * bp);
    
    virtual void loadFromLua(lua_State * L);
    static PHImageView * imageFromLua(lua_State * L, PHGameManager * man, const string & rootPath);
    static PHImageView * imageFromLua(lua_State * L, PHGameManager * man, const string & root, PHAnimatorPool * pool);
    static PHImageView * imageFromClass(const string & clss);
  
    static void registerLuaInterface(lua_State * L);
    static const string _luaClass;
    
    static map<string,PHAllocator> * initMap;
};

#endif
