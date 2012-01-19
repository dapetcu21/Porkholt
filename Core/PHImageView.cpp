/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"
#include "PHImage.h"
#include "PHImageView.h"
#include "PHTrailImageView.h"
#include "PHFileManager.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHBezierPath.h"
#include "PHParticleView.h"
#include "PHKeyframeAnimator.h"

#define PHIMAGEVIEW_INIT _image(NULL), _animator(NULL), coords(PHWholeRect), tint(PHInvalidColor), pool(PHAnimatorPool::currentAnimatorPool()), curve(NULL), arraysVBO(0), indexesVBO(0), VBOneedsRebuilding(true), constrain(true), _repeatX(1), _repeatY(1), lastAnimFrame(-1), animFrame(-1)

PHImageView::PHImageView() : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
}

PHImageView::PHImageView(PHImage * image) : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
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
    VBOneedsRebuilding = true;
}

PHImageView::~PHImageView()
{
    if (curve)
        curve->release();
    curve = NULL;
    if (arraysVBO)
        glDeleteBuffers(1, &arraysVBO);
    if (indexesVBO)
        glDeleteBuffers(1, &indexesVBO);
    if (_animator)
        _animator->release();
	if (_image)
		_image->release();
}

void PHImageView::renderInFramePortionTint(const PHRect & fr, const PHRect & crd, const PHColor & clr)
{
    if (_image)
    {
        if (_image->isNormal())
            ((PHNormalImage*)_image)->renderInFramePortionTint(_gameManager,fr,crd,clr);
        
        if (_image->isAnimated())
            _animator->renderInFramePortionTint(_gameManager,fr,crd,clr);
    }
}

GLfloat * PHImageView::interleavedArrayFromAnchorList(const void * ud, int & n)
{
    const vector<PHBezierPath::anchorPoint> * anchors = (const vector<PHBezierPath::anchorPoint>*)ud;
    n = (int)anchors->size();
    GLfloat * v = new GLfloat[n*4];
    for (int i=0; i<n; i++)
    {
        v[(i<<2)] = anchors->at(i).point.x;
        v[(i<<2)+1] = anchors->at(i).point.y;
    }
    textureCoordinatesFromAnchorList(v+2,sizeof(GLfloat)*4,ud);
    return v;
}

void PHImageView::textureCoordinatesFromAnchorList(GLfloat * buffer, size_t stride, const void * ud)
{
    const vector<PHBezierPath::anchorPoint> * anchors = (const vector<PHBezierPath::anchorPoint>*)ud;
    int i,nx;
    nx = i = 0;
    int corners[4] = {0,0,0,0};
    int n = (int)anchors->size();
    if (!stride)
        stride = 2*sizeof(GLfloat);
    
    for (int i=0; i<n; i++)
        if (anchors->at(i).tag>0 && anchors->at(i).tag<=4)
            corners[(anchors->at(i).tag)-1] = i;
    
    PHNormalImage * img = (PHNormalImage*)_image;
    int _width = img->width();
    int _height = img->height();
    int actWidth = img->actualWidth();
    int actHeight = img->actualHeight();
    double sX = ((double)_width/(double)actWidth)*repeatX();
    double sY = ((double)_height/(double)actHeight)*repeatY();
    double sx = 0.5f/actWidth;
    double sy = 0.5f/actHeight;
    PHRect p = textureCoordinates();
    p.width*=sX;
    p.height*=sY;
    p.x*=sX;
    p.y*=sX;
    p.x+=sx;
    p.y+=sy;
    p.width-=sx*2;
    p.height-=sx*2;
    
    for (int k=0; k<4; k++)
    {
        double len = 0;
        int tag;
        for (i = nx = corners[k];((tag = anchors->at(i).tag),(tag<=0 || tag>4 || tag==k+1)); i=nx)
        {
            nx++;
            if (nx>=n)
                nx-=n;
            len+=(anchors->at(nx).point-anchors->at(i).point).length();
        }
        double l = 0;
        for (i = nx = corners[k];((tag = anchors->at(i).tag),(tag<=0 || tag>4 || tag==k+1)); i=nx)
        {
            nx++;
            if (nx>=n)
                nx-=n;
            double d = l/len;
            double x,y;
            switch (k+1) {
                case 1:
                    y = 1;
                    x = d;
                    break;
                case 2:
                    x = 1;
                    y = 1-d;
                    break;
                case 3:
                    x = 1-d;
                    y = 0;
                    break;
                case 4:
                    x = 0;
                    y = d;
                    break;
                default:
                    x = 0;
                    y = 1;
                    break;
            }
            if (_image && _image->isNormal())
            {
                x = p.x+p.width*x;
                y = p.y+p.height*y;
            }
            //PHLog("point: (%lf,%lf) txtcoord: (%lf,%lf)",anchors->at(i).point.x,anchors->at(i).point.y,x,y);
            buffer[0] = x;
            buffer[1] = y;
            buffer = (GLfloat*)(((char*)buffer)+stride);
            l+=(anchors->at(nx).point-anchors->at(i).point).length();
        }
    }
}

void PHImageView::rebuildCurvedVBO()
{
    if (curve)
    {
        if (arraysVBO == 0)
            glGenBuffers(1, &arraysVBO);
        if (indexesVBO == 0)
            glGenBuffers(1, &indexesVBO);
        const vector<PHBezierPath::anchorPoint> * anchors = PHBezierPath::tesselate(curve->calculatedVertices());
        GLfloat * arr = interleavedArrayFromAnchorList(anchors,nVertices);
        GLushort * indexes = PHBezierPath::triangulate(*anchors,nIndexes);
        delete anchors;
        glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
        glBufferData(GL_ARRAY_BUFFER, nVertices*4*sizeof(GLfloat), arr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndexes*sizeof(GLushort), indexes, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        delete arr;
        delete indexes;
        
    } else {
        if (arraysVBO)
            glDeleteBuffers(1, &arraysVBO);
        if (indexesVBO)
            glDeleteBuffers(1, &indexesVBO);
        arraysVBO = indexesVBO = 0;
    }
}

void PHImageView::bezierCallback(PHBezierPath *sender, void *ud)
{
    VBOneedsRebuilding = true;
}

void PHImageView::setBezierPath(PHBezierPath *bp)
{
    if (curve)
    {
        curve->release();
        curve->removeCallback(this);
    }
    curve = NULL;
    if (bp)
    {
        bp->retain();
        bp->addCallback(PHInv(this,PHImageView::bezierCallback,NULL));
    }
    curve = bp;
    VBOneedsRebuilding = true;
}

void PHImageView::renderCurved()
{
    if (!_image) return;
    if (image()->isAnimated()) return;
    image()->load();
    loadVBO();
    
    glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexesVBO);
    
    PHGLSetColor(tint);
    PHGLSetStates(PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture);
    
    glVertexPointer(2, GL_FLOAT, 4*sizeof(GLfloat), NULL);
    glTexCoordPointer(2, GL_FLOAT, 4*sizeof(GLfloat), (GLfloat*)NULL+2);
    ((PHNormalImage*)image())->bindToTexture();
    
    if (constrain)
    {
        glPushMatrix();
        glTranslatef(_bounds.x, _bounds.y, 0);
        glScalef(_bounds.width, _bounds.height, 1);
    }
    glDrawElements(GL_TRIANGLES, nIndexes, GL_UNSIGNED_SHORT, NULL);
    if (constrain)
        glPopMatrix();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PHImageView::rebuildStraightVBO()
{
    if (_image)
    {
        if (_image->isNormal())
        {
            ((PHNormalImage*)_image)->rebuildVBO(this,arraysVBO,params1);
            if (indexesVBO)
            {
                glDeleteBuffers(1, &indexesVBO);
                indexesVBO = false;
            }
        }
        if (_image->isAnimated())
        {
            _animator->rebuildVBOs(this,arraysVBO,params1,indexesVBO,params2);
            lastAnimFrame = _animator->lastRealFrame();
            animFrame = _animator->currentRealFrame();
        }
    }
}

bool PHImageView::animatorNeedsVBORebuild()
{
    if (!_image || !_animator || !_image->isAnimated())
        return false;
    return ((_animator->lastRealFrame() != lastAnimFrame) || (_animator->currentRealFrame() != animFrame));
}

void PHImageView::renderStraight()
{
    //TODO: use VBOs
    if (!_image) return;
    _image->load();
    if (animatorNeedsVBORebuild())
        VBOneedsRebuilding = true;
    loadVBO();
    
    if (_image->isNormal() && arraysVBO)
    {
        glPushMatrix();
        glTranslatef(_bounds.x, _bounds.y, 0);
        glScalef(_bounds.width, _bounds.height, 1);
        
        glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
        PHGLSetColor(tint);
        PHGLSetStates(PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture);
        glVertexPointer(params1.vertexSize, params1.vertexType, params1.vertexStride, ((uint8_t*)NULL)+params1.vertexOffset);
        glTexCoordPointer(params1.texCoordSize, params1.texCoordType, params1.texCoordStride, ((uint8_t*)NULL)+params1.texCoordOffset);
        ((PHNormalImage*)image())->bindToTexture();
        glDrawArrays(params1.renderMode, 0, params1.nElements);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        glPopMatrix();
    }
    
    if (_image->isAnimated() && arraysVBO)
    {
        glPushMatrix();
        glTranslatef(_bounds.x, _bounds.y, 0);
        glScalef(_bounds.width, _bounds.height, 1);
        
        PHColor t = tint.isValid()?tint:PHWhiteColor;
        double rem = indexesVBO?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
        glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
        PHGLSetColor(t*(1-rem));
        PHGLSetStates(PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture);
        glVertexPointer(params1.vertexSize, params1.vertexType, params1.vertexStride, ((uint8_t*)NULL)+params1.vertexOffset);
        glTexCoordPointer(params1.texCoordSize, params1.texCoordType, params1.texCoordStride, ((uint8_t*)NULL)+params1.texCoordOffset);
        _animator->bindCurrentFrameToTexture();
        glDrawArrays(params1.renderMode, 0, params1.nElements);
        
        if (indexesVBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, indexesVBO);
            PHGLSetColor(t*rem);
            PHGLSetStates(PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture);
            glVertexPointer(params2.vertexSize, params2.vertexType, params2.vertexStride, ((uint8_t*)NULL)+params2.vertexOffset);
            glTexCoordPointer(params2.texCoordSize, params2.texCoordType, params2.texCoordStride, ((uint8_t*)NULL)+params2.texCoordOffset);
            _animator->bindLastFrameToTexture();
            glDrawArrays(params2.renderMode, 0, params2.nElements);
        }
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        glPopMatrix();
    }
    //renderInFramePortionTint(_bounds, coords, tint);
}

void PHImageView::draw()
{
    if (curve)
        renderCurved();
    else
        renderStraight();
}

void PHImageView::loadFromLua(lua_State *L)
{
    if (lua_istable(L, -1))
    {
        PHRect portion = PHWholeRect;
        PHRect frame = PHWholeRect;
        int tag = 0;
        double rot = 0;
        bool flipHoriz = false;
        bool flipVert = false;
        bool constrain = true;
        double repX=1,repY=1;
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
        
        setTintColor(tint);
        setTextureCoordinates(portion);
        setFrame(frame);
        setRotation(rot);
        setTag(tag);
        setHorizontallyFlipped(flipHoriz);
        setVerticallyFlipped(flipVert);
        setRepeatX(repX);
        setRepeatY(repY);
        setBezierPath(bp);
        setConstrainCurveToFrame(constrain);
        if (bp)
            bp->release();
    }
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root)
{
    return imageFromLua(L, root, PHAnimatorPool::currentAnimatorPool());
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root, PHAnimatorPool * pool)
{
    PHImageView * img = NULL;
    if (lua_istable(L, -1))
    {
        bool isV = true;
        string filename = root + "/"; 
        string fn;
        lua_pushstring(L, "filename");
        lua_gettable(L, -2);
        if ((isV = lua_isstring(L, -1)))
            fn = lua_tostring(L, -1);
        lua_pop(L, 1);
        
        if (fn[0]=='/')
        {
            filename = PHFileManager::resourcePath() + "/img";
        }
        filename = filename + fn;
        
        if (isV)
        {
            string clss;
            PHLuaGetStringField(clss, "class");
            
            img = PHImageView::imageFromClass(clss);
            img->setAnimatorPool(pool);
            img->setImage(PHImage::imageFromPath(filename));
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
        img = PHImage::imageNamed(s);
    else
        img = PHImage::imageFromPath(s);
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
