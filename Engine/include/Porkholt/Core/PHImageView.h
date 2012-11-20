/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHMaterial.h>

class PHImage;
class PHImageAnimator;
class PHAnimatorPool;
class PHCurve;
class PHDirectory;
class PHGLShaderProgram;

#define PH_REGISTERIMAGEVIEW(clss) PH_REGISTERCLASS(PHImageView::initMap, #clss, clss)

class PHImageView : public PHView
{
protected:
    class ImageMaterial : public PHMaterial
    {
        protected:
            PHImageView * _imageView;
        public:
            ImageMaterial(PHImageView * iv) : _imageView(iv) {}
            bool materialSupportsRenderMode(int rm);
            void renderVAO(PHGLVertexArrayObject * vao, PHGLUniformStates * additionalUniforms);
    };
    friend class ImageMaterial;
    ImageMaterial imageMaterial;

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
	void rebuildCurvedVAO();
    void rebuildStraightVAO();
    void destroyVAOs();
    void destroyStraightVAO();
    void destroyCurvedVAO();
    void rebuildVAO()
    {
        if (curve)
            rebuildCurvedVAO();
        else
            rebuildStraightVAO();
    }
    bool animatorNeedsVAORebuild();
    
    PHCurve * curve;
    
    bool VAOneedsRebuilding;
    
    PHGLVertexArrayObject * curveVAO;
    PHGLVertexArrayObject * straightVAO1, * straightVAO2;
    
    int lastAnimFrame,animFrame;
    
    void loadVAO()
    {
        if (VAOneedsRebuilding)
        {
            rebuildVAO();
            VAOneedsRebuilding = false;
        }
    }
    
    bool _normalMapping;

    virtual void attachedToGameManager();
    
    PHGLShaderProgram * _shader;
    PHMaterial * _material;

public:
    PHMaterial * material();
    void setMaterial(PHMaterial * m);

    PHGLShaderProgram * shader() { return _shader; }
    void setShader(PHGLShaderProgram * shad);
    
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
	void setTextureCoordinates(const PHRect & r) { coords = r; VAOneedsRebuilding = true; };
    
    PHColor & tintColor() { return tint; };
    void setTintColor(const PHColor & clr) { tint = clr; };
    
    virtual PHColor cinematicCustomColor() { if (tint.a<0) return PHWhiteColor; return tint; }
    virtual void setCinematicCustomColor(const PHColor & c);
    
    bool constrainCurveToFrame() { return constrain; }
    void setConstrainCurveToFrame(bool c) { constrain = c; }
    
    ph_float repeatX() { return _repeatX; }
    void setRepeatX(ph_float rx) { _repeatX = rx; VAOneedsRebuilding = true; }
    
    ph_float repeatY() { return _repeatY; }
    void setRepeatY(ph_float ry) { _repeatY = ry; VAOneedsRebuilding = true; }
    
    PHCurve * shape() { return curve; }
    void setShape(PHCurve * bp);
    
    virtual void loadFromLua(lua_State * L);
    static PHImageView * imageFromLua(lua_State * L, PHGameManager * man, PHDirectory * rootDir);
    static PHImageView * imageFromLua(lua_State * L, PHGameManager * man, PHDirectory * rootDir, PHAnimatorPool * pool);
    static PHImageView * imageFromClass(const string & clss);
  
    static void registerLuaInterface(lua_State * L);
    static const string _luaClass;
    
    static map<string,PHAllocator> * initMap;
};

#endif
