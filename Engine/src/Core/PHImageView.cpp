/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHTrailImageView.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHBezierPath.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHKeyframeAnimator.h>
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHDeferredView.h>
#include <Porkholt/Core/PHGLShaderProgram.h>

const string PHImageView::_luaClass("PHImageView");

#define PHIMAGEVIEW_INIT imageMaterial(this), _material(&imageMaterial), _image(NULL), _animator(NULL), coords(PHWholeRect), tint(PHInvalidColor), pool(NULL), curve(NULL), VAOneedsRebuilding(true), constrain(true), _repeatX(1), _repeatY(1), _additionalUniforms(NULL), lastAnimFrame(-1), animFrame(-1), _normalMapping(true), curveVAO(NULL), straightVAO1(NULL), straightVAO2(NULL), _shader(NULL)

PHMaterial * PHImageView::material()
{
    if (_material == &imageMaterial)
        return NULL;
    return _material;
}

void PHImageView::setMaterial(PHMaterial * m)
{
    if (m)
        m->retain();
    else
        m = &imageMaterial;
    if (material())
        _material->release();
    _material = m;
}

void PHImageView::setShader(PHGLShaderProgram *sh)
{
    if (sh)
        sh->retain();
    if (_shader)
        _shader->release();
    _shader = sh;
}

void PHImageView::setAnimationFieldC(int field, const PHColor & clr)
{
    switch (field)
    {
        case PHCinematicActor::fieldColor:
            if (clr == PHWhiteColor)
                setTintColor(PHInvalidColor);
            else
                setTintColor(clr);
            break;
        default:
            PHView::setAnimationFieldC(field, clr);
    }
}

PHColor PHImageView::getAnimationFieldC(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldColor:
            if (tint.a<0)
                return PHWhiteColor;
            return tint;
        default:
            return PHView::getAnimationFieldC(field);
    }
}

bool PHImageView::ImageMaterial::materialSupportsRenderMode(int rm)
{
    return (rm == PHGameManager::defaultRenderMode) || ((rm == PHDeferredView::normalMapRenderMode) && _imageView->_image && _imageView->_image->normalMap());
}

bool PHImageView::supportsRenderMode(int rm)
{
    return _material->materialSupportsRenderMode(rm);
}

PHImageView::PHImageView() : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = &_luaClass;
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), PHIMAGEVIEW_INIT
{
    luaClass = &_luaClass;
}

PHImageView::PHImageView(PHImage * image) : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = &_luaClass;
	setImage(image);
}

void PHImageView::setAnimatorPool(PHAnimatorPool * p) 
{
    pool = p; 
    if (_animator)
        _animator->setAnimatorPool(p);
}

void PHImageView::setImage(PHImage * img)
{
    if (_animator)
    {
        _animator->release();
        _animator = NULL;
    }
    if (img)
    {
        img->retain();
        if (img->isAnimated())
            _animator = ((PHAnimatedImage*)img)->newAnimator(pool);
    }
    if (_image)
        _image->release();
    _image = img;
    VAOneedsRebuilding = true;
}

void PHImageView::destroyVAOs()
{
    destroyStraightVAO();
    destroyCurvedVAO();
}

void PHImageView::destroyStraightVAO()
{
    if (straightVAO1)
    {
        straightVAO1->release();
        straightVAO1 = NULL;
    }
    if (straightVAO2)
    {
        straightVAO2->release();
        straightVAO2 = NULL;
    }
}

void PHImageView::destroyCurvedVAO()
{
    if (curveVAO)
    {
        curveVAO->release();
        curveVAO = NULL;
    }
}

PHImageView::~PHImageView()
{
    if (curve)
        curve->release();
    curve = NULL;
    destroyVAOs();
    if (_animator)
        _animator->release();
	if (_image)
		_image->release();
    if (_additionalUniforms)
        _additionalUniforms->release();
}

char missingnormals_sprites[] = "missingnormals_sprites";

void PHImageView::renderInFramePortionTint(const PHRect & fr, const PHRect & crd, const PHColor & clr)
{
    if (!_image) return;
    bool nrm = _normalMapping && (gm->renderMode() == PHDeferredView::normalMapRenderMode);
    PHImage * img = _image;
    if (nrm)
    {
        if (img->normalMap())
            _image = img->normalMap();
        else
            gm->pushSpriteShader(gm->shaderProgramNamed<missingnormals_sprites>());
    }

    if (_image->isNormal())
        ((PHNormalImage*)_image)->renderInFramePortionTint(gm,fr,crd,clr);
    
    if (_image->isAnimated())
        _animator->renderInFramePortionTint(gm,fr,crd,clr);
    
    if (nrm)
    {
        if (img->normalMap())
            _image = img;
        else
            gm->popSpriteShader();
    }
}

void PHImageView::rebuildCurvedVAO()
{
    destroyStraightVAO();
    if (curve)
    {
        if (!curveVAO) 
            curveVAO = new PHGLVertexArrayObject(gm);
        
        PHNormalImage * img = (PHNormalImage*)_image;
        PHRect p = img->textureCoordinates(textureCoordinates());
        p.width*=repeatX();
        p.height*=repeatY();

        curve->rebuildVAO(curveVAO, p);
               
    } else {
        destroyCurvedVAO();
    }
}

void PHImageView::curveCallback(PHCurve *sender, void *ud)
{
    VAOneedsRebuilding = true;
}

void PHImageView::setShape(PHCurve *bp)
{
    if (curve)
    {
        curve->release();
        curve->removeCallback((PHDrawable*)this);
    }
    curve = NULL;
    if (bp)
    {
        bp->retain();
        bp->addCallback(PHInv((PHDrawable*)this,PHImageView::curveCallback,NULL));
    }
    curve = bp;
    VAOneedsRebuilding = true;
}

void PHImageView::rebuildStraightVAO()
{
    destroyCurvedVAO();
    if (_image)
    {
        if (_image->isNormal())
        {
            ((PHNormalImage*)_image)->rebuildVAO(this, straightVAO1);
            if (straightVAO2)
            {
                straightVAO2->release();
                straightVAO2 = NULL;
            }
        }
        if (_image->isAnimated())
        {
            _animator->rebuildVAOs(this, straightVAO1, straightVAO2);
            lastAnimFrame = _animator->lastRealFrame();
            animFrame = _animator->currentRealFrame();
        }
    } else
        destroyStraightVAO();
}

bool PHImageView::animatorNeedsVAORebuild()
{
    if (!_image || !_animator || !_image->isAnimated())
        return false;
    return ((_animator->lastRealFrame() != lastAnimFrame) || (_animator->currentRealFrame() != animFrame));
}

void PHImageView::ImageMaterial::renderVAO(PHGLVAO * vao, PHGLUniformStates * uniforms)
{
    PHGLShaderProgram * shader = _imageView->_shader;
    if (!shader)
        shader = _imageView->gm->spriteShader();
    uniforms->apply(shader);
    vao->draw();
}

PHGLUniformStates * PHImageView::additionalUniforms()
{
    if (!_additionalUniforms)
        _additionalUniforms = new PHGLUniformStates();
    return _additionalUniforms;
}

void PHImageView::renderStraight()
{
    if (!_image) return;
    _image->load();
    if (animatorNeedsVAORebuild())
        VAOneedsRebuilding = true;
    loadVAO();
    if (straightVAO1)
    {
        PHGLUniformStates * states = gm->spriteUniformStates();
        states->setNextStates(_additionalUniforms);

        gm->setGLStates(_openGLStates | PHGLTexture0);
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * PHMatrix::translation(_bounds.x, _bounds.y) * PHMatrix::scaling(_bounds.width,_bounds.height));
        gm->updateMatrixUniform();
        if (_image->isNormal())
        {
            gm->setColor(tint);
            gm->updateColorUniform();
            gm->setTextureUniform(((PHNormalImage*)image())->texture());
            _material->renderVAO(straightVAO1, states);
        }
        
        if (_image->isAnimated())
        {
            PHColor t = tint.isValid()?tint:PHWhiteColor;
            float rem = straightVAO2?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
            
            
            if (straightVAO2)
            {
                gm->setColor(t.multipliedAlpha(rem));
                gm->updateColorUniform();
                gm->setTextureUniform(_animator->lastFrameTexture());
                _material->renderVAO(straightVAO2, states);
            }
            
            gm->setColor(t.multipliedAlpha(1-rem));
            gm->updateColorUniform();
            gm->setTextureUniform(_animator->currentFrameTexture());
            _material->renderVAO(straightVAO1, states);
        }
        
        states->setNextStates(NULL);
        gm->setTextureUniform(NULL);
        gm->setModelViewMatrix(om);
    }
}

void PHImageView::renderCurved()
{
    if (!_image) return;
    if (image()->isAnimated()) return;
    image()->load();
    loadVAO();
    
    gm->setColor(tint);
    gm->updateColorUniform();
    gm->setTextureUniform(((PHNormalImage*)image())->texture());
    
    gm->setGLStates(_openGLStates | PHGLTexture0);
    
    if (constrain)
    {
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * PHMatrix::translation(_bounds.x, _bounds.y) * PHMatrix::scaling(_bounds.width,_bounds.height));
        gm->updateMatrixUniform();
        curveVAO->draw();
        _material->renderVAO(curveVAO, gm->spriteUniformStates());
        gm->setModelViewMatrix(om);
    } else {
        gm->updateMatrixUniform();
        _material->renderVAO(curveVAO, gm->spriteUniformStates());
    }

    gm->setTextureUniform(NULL);
}



void PHImageView::draw()
{
    if (!_image) return;
    bool nrm = (gm->renderMode() == PHDeferredView::normalMapRenderMode) && _normalMapping;
    PHImage * img = _image;
    PHGLShaderProgram * shd = _shader;
    if (nrm)
    {
        if (img->normalMap())
            _image = _image->normalMap();
        else
            _shader = gm->shaderProgramNamed<missingnormals_sprites>();
    }

    if (curve)
        renderCurved();
    else
        renderStraight();

    if (nrm)
    {
        if (img->normalMap())
            _image = img;
        else
            _shader = shd;
    }
}

void PHImageView::attachedToGameManager()
{
    PHView::attachedToGameManager();
    if (!pool)
        setAnimatorPool(gm->animatorPool());
}

void PHImageView::loadFromLua(lua_State *L)
{
    if (lua_istable(L, -1))
    {
        PHRect portion = PHWholeRect;
        PHRect frame = PHWholeRect;
        int tag = 0;
        float rot = 0;
        bool flipHoriz = false;
        bool flipVert = false;
        bool constrain = true;
        float repX=1,repY=1;
        PHColor tint = PHInvalidColor;

        PHLuaGetRectField(frame, "pos");
        PHLuaGetRectField(portion , "texCoord");
        PHLuaGetNumberField(tag, "tag");
        PHLuaGetAngleField(rot, "rotation");
        PHLuaGetBoolField(flipHoriz, "horizontallyFlipped");
        PHLuaGetBoolField(flipVert, "verticallyFlipped");
        PHLuaGetNumberField(repX, "repeatX");
        PHLuaGetNumberField(repY, "repeatY");
        PHLuaGetColorField(tint, "tint");
        PHLuaGetBoolField(constrain, "constrainCurveToFrame");
        
        lua_getfield(L, -1, "bezierPath");
        PHBezierPath * bp = PHBezierPath::fromLua(L);
        lua_pop(L,1);
        
        lua_pushstring(L, "alpha");
        lua_gettable(L, -2);
        if (lua_isnumber(L , -1))
        {
            if (tint == PHInvalidColor)
                tint = PHWhiteColor;
            tint.a *= lua_tonumber(L, -1);
        }
        lua_pop(L,1);
        
        /*
        lua_getfield(L, -1, "keyframeAnimations");
        if (lua_istable(L, -1))
        {
            PHLuaForEach(-1)
                PHKeyframeAnimator * anim = PHKeyframeAnimator::fromLua(L);
                if (anim)
                {
                    anim->setAnimatorPool(pool);
                    addCinematicAnimation(anim);
                    anim->playSection("default");
                    anim->release();
                }
                lua_pop(L,1);
            PHLuaForEach_
        }
        lua_pop(L,1);
        */
        
        setTintColor(tint);
        setTextureCoordinates(portion);
        setFrame(frame);
        setRotation(rot);
        setTag(tag);
        setHorizontallyFlipped(flipHoriz);
        setVerticallyFlipped(flipVert);
        setRepeatX(repX);
        setRepeatY(repY);
        setShape(bp);
        setConstrainCurveToFrame(constrain);
        if (bp)
            bp->release();
    }
}

PHImageView * PHImageView::imageFromLua(lua_State * L, PHGameManager * man, PHDirectory * rootDir)
{
    return imageFromLua(L, man, rootDir, man->animatorPool());
}

PHImageView * PHImageView::imageFromLua(lua_State * L, PHGameManager * man, PHDirectory * root, PHAnimatorPool * pool)
{
    PHImageView * img = NULL;
    if (lua_istable(L, -1))
    {
        bool isV = true;
        string fn;
        lua_pushstring(L, "filename");
        lua_gettable(L, -2);
        if ((isV = lua_isstring(L, -1)))
            fn = lua_tostring(L, -1);
        lua_pop(L, 1);
        
        if (isV)
        {
            string clss;
            PHLuaGetStringField(clss, "class");
            
            img = PHImageView::imageFromClass(clss);
            img->setAnimatorPool(pool);
            img->setGameManager(man);
            PHImage * im = NULL;
            if (fn[0] == '/')
                im = man->imageNamed(fn.substr(1));
            else
                im = man->imageNamed(fn, root);
            img->setImage(im);
            img->setOptimizations(true);
            img->loadFromLua(L);
        }
    }
    return img;
}

map<string, PHAllocator> * PHImageView::initMap = NULL;

PH_REGISTERIMAGEVIEW(PHImageView)

PHImageView * PHImageView::imageFromClass(const string & clss)
{
    map<string,PHAllocator>::iterator i = initMap->find(clss);
    if (i==initMap->end())
        return new PHImageView;
    return (PHImageView*)(i->second)();
}

#pragma mark -
#pragma mark scripting

static int PHImageView_width(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    if (!v->image())
        lua_pushnumber(L, 0);
    else
        lua_pushnumber(L, v->image()->width());
    return 1;
}

static int PHImageView_height(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    if (!v->image())
        lua_pushnumber(L, 0);
    else
        lua_pushnumber(L, v->image()->height());
    return 1;
}

PHLuaRectGetter(PHImageView, textureCoordinates);
PHLuaRectSetter(PHImageView, setTextureCoordinates);
PHLuaBoolGetter(PHImageView, constrainCurveToFrame);
PHLuaBoolSetter(PHImageView, setConstrainCurveToFrame);
PHLuaNumberGetter(PHImageView, repeatX);
PHLuaNumberSetter(PHImageView, setRepeatX);
PHLuaNumberGetter(PHImageView, repeatY);
PHLuaNumberSetter(PHImageView, setRepeatY);

static int PHImageView_tint(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    v->tintColor().saveToLua(L);
    return 1;
}

static int PHImageView_setTint(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    v->setTintColor(PHColor::fromLua(L, 2));
    return 0;
}

static int PHImageView_setImage(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    string s = lua_tostring(L, 2);
    PHImage * img;
    if (s.find('/')==s.npos)
        img = v->gameManager()->imageNamed(s);
    else
        img = v->gameManager()->imageNamed(s, v->gameManager()->resourceDirectory()); //DAFUQ???
    v->setImage(img);
    return 0;
}


void PHImageView::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHImageView");
    
    PHLuaAddMethod(PHImageView, width);
    PHLuaAddMethod(PHImageView, height);
    PHLuaAddMethod(PHImageView, setImage);
    PHLuaAddMethod(PHImageView, tint);
    PHLuaAddMethod(PHImageView, setTint);
    PHLuaAddMethod(PHImageView, textureCoordinates);
    PHLuaAddMethod(PHImageView, setTextureCoordinates);
    PHLuaAddMethod(PHImageView, constrainCurveToFrame);
    PHLuaAddMethod(PHImageView, setConstrainCurveToFrame);
    PHLuaAddMethod(PHImageView, repeatX);
    PHLuaAddMethod(PHImageView, setRepeatX);
    PHLuaAddMethod(PHImageView, repeatY);
    PHLuaAddMethod(PHImageView, setRepeatY);

    lua_pop(L,1);
}
