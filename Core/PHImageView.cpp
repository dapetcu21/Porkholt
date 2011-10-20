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

#define GL_INVALID_INDEX 0xffffffff

#define PHIMAGEVIEW_INIT _image(NULL), _animator(NULL), coords(PHWholeRect), tint(PHInvalidColor), pool(PHAnimatorPool::mainAnimatorPool()), curve(NULL), arraysVBO(GL_INVALID_INDEX), indexesVBO(GL_INVALID_INDEX), VBOneedsRebuilding(false), constrain(true)

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
}

PHImageView::~PHImageView()
{
    if (curve)
        curve->release();
    curve = NULL;
    rebuildCurvedVBO();
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
            ((PHNormalImage*)_image)->renderInFramePortionTint(fr,crd,clr);
        
        if (_image->isAnimated())
            _animator->renderInFramePortionTint(fr,crd,clr);
    }
}

GLfloat * PHImageView::interleavedArrayFromAnchorList(const void * ud, int & n)
{
    const vector<PHBezierPath::anchorPoint> * anchors = (const vector<PHBezierPath::anchorPoint>*)ud;
    n = anchors->size();
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
    int n = anchors->size();
    if (!stride)
        stride = 2*sizeof(GLfloat);
    
    for (int i=0; i<n; i++)
        if (anchors->at(i).tag>0 && anchors->at(i).tag<=4)
            corners[(anchors->at(i).tag)-1] = i;
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
                PHNormalImage * img = (PHNormalImage*)_image;
                x*=(double)img->width()/(double)img->actualWidth();
                y*=(double)img->height()/(double)img->actualHeight();
            }
            PHLog("point: (%lf,%lf) txtcoord: (%lf,%lf)",anchors->at(i).point.x,anchors->at(i).point.y,x,y);
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
        if (arraysVBO == GL_INVALID_INDEX)
            glGenBuffers(1, &arraysVBO);
        if (indexesVBO == GL_INVALID_INDEX)
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
        if (arraysVBO!=GL_INVALID_INDEX)
            glDeleteBuffers(1, &arraysVBO);
        if (indexesVBO!=GL_INVALID_INDEX)
            glDeleteBuffers(1, &indexesVBO);
        arraysVBO = indexesVBO = GL_INVALID_INDEX;
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

void PHImageView::draw()
{
    if (curve)
        renderCurved();
    else
        renderInFramePortionTint(_bounds, coords, tint);
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root)
{
    return imageFromLua(L, root, PHAnimatorPool::mainAnimatorPool());
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
            PHRect portion = PHWholeRect;
            PHRect frame = PHWholeRect;
            string clss;
            int tag = 0;
            double rot = 0;
            bool flipHoriz = false;
            bool flipVert = false;
            bool constrain = true;
            PHColor tint = PHInvalidColor;
            PHLuaGetStringField(clss, "class");
            PHLuaGetRectField(frame, "pos");
            PHLuaGetRectField(portion , "texCoord");
            PHLuaGetNumberField(tag, "tag");
            PHLuaGetAngleField(rot, "rotation");
            PHLuaGetBoolField(flipHoriz, "horizontallyFlipped");
            PHLuaGetBoolField(flipVert, "verticallyFlipped");
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
            
            img = PHImageView::imageFromClass(clss);
            img->setAnimatorPool(pool);
            img->setImage(PHImage::imageFromPath(filename));
            img->setTintColor(tint);
            img->setTextureCoordinates(portion);
            img->setFrame(frame);
            img->setRotation(rot);
            img->setOptimizations(true);
            img->setTag(tag);
            img->setHorizontallyFlipped(flipHoriz);
            img->setVerticallyFlipped(flipVert);
            img->setBezierPath(bp);
            img->setConstrainCurveToFrame(constrain);
            if (bp)
                bp->release();
        }
    }
    return img;
}

PHImageView * PHImageView::imageFromClass(const string & clss)
{
    if (clss=="PHTrailImageView")
        return new PHTrailImageView();
    return new PHImageView();
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

    lua_pop(L,1);
}
