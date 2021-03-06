/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTEXTURECANVAS_H
#define PHTEXTURECANVAS_H

#include <Porkholt/Core/PHDrawable.h>
#include <Porkholt/Core/PHGLFBOAttachment.h>

class PHGLTexture2D;
class PHGLFramebuffer;

class PHTextureCanvas : public PHDrawable
{
protected:
    PHGLFramebuffer * fbo;  
    bool changed;

    bool dit;
    enum PHGLFBOAttachment::pixelFormat dfmt;
    vector<enum PHGLFBOAttachment::pixelFormat> cfmt;
    PHGLTexture2D * dtex;
    vector<PHGLTexture2D*> ctex;
    
    int ow, oh, w, h;
    bool fs;
    int cmask;

    int _renderMode;

    PHColor ccolor;
    float cdepth;
    int cstencil;

    PHGLFBOAttachment * rebuildAttachment(PHGLFBOAttachment * f, enum PHGLFBOAttachment::pixelFormat fmt, bool isTex, int w, int h, PHGLTexture2D ** cache);
    
    PHInvocation cb;

public:
    int renderMode() { return _renderMode; }
    void setRenderMode(int r) { _renderMode = r; }

    void setOnRenderCallback(const PHInvocation & inv) { cb = inv; }

    PHTextureCanvas(PHGameManager * gm);
    ~PHTextureCanvas();

    void setColorFormat(enum PHGLFBOAttachment::pixelFormat fmt, int color_attachment = 0);
    void setDepthFormat(enum PHGLFBOAttachment::pixelFormat fmt);

    PHGLFBOAttachment::pixelFormat colorFormat(int color_attachment = 0) 
    { 
        if (color_attachment >= cfmt.size()) 
            return PHGLFBOAttachment::None; 
        return cfmt[color_attachment];
    }
    PHGLFBOAttachment::pixelFormat depthFormat() { return dfmt; }

    void setDepthIsTexture(bool v);
    bool depthIsTexture() { return dit; }

    PHGLTexture2D * colorTexture(int color_attachment = 0);
    PHGLTexture2D * depthTexture();
    void setColorTexture(PHGLTexture2D * tex, int color_attachment = 0);
    void setDepthTexture(PHGLTexture2D * tex);

    bool isFullScreenTexture() { return !w && !h; }
    void setFullScreenTexture() { w = h = 0; }
    void setTextureSize(int _w, int _h) { w = _w; h = _h; }
    void setTextureSize(const PHSize & sz) { w = int(sz.width); h = int(sz.height); }
    PHSize textureSize() { return PHSize(w, h); }

    void setClearMask(int mask) { cmask = mask; }
    int clearMask() { return cmask; }

    void setClearColor(const PHColor & c) { ccolor = c; }
    void setDepthClearValue(float val) { cdepth = val; }
    void setStencilClearValue(int val) { cstencil = val; }

    virtual void render();
};

#endif
