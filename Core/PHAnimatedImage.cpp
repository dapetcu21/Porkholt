//
//  PHAnimatedImage.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/9/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHAnimatedImage.h"
#include "PHLua.h"
#include "PHFileManager.h"
#include "PHImageAnimator.h"

#define PNG_DEBUG 4
#include <png.h>
#include <sstream>

lua_State * PHAnimatedImage::L = NULL;
PHMutex * PHAnimatedImage::luaMutex = new PHMutex;

PHAnimatedImage::PHAnimatedImage(const string & s) : PHImage(s), thread(NULL), defaultSection(0), path(s)
{
    luaMutex->lock();
    if (!L)
    {
        L = lua_open();
        luaL_openlibs(L);
        PHLuaSeedRandom(L);
        PHLuaLoadFile(L, PHFileManager::resourcePath()+"/scripts/animations.lua");
    }
    string initPath = s+"/info.lua";
    antialiasing = PHFileManager::fileExists(s+".aa");
    if (!PHFileManager::fileExists(initPath))
    {
        luaMutex->unlock();
        throw PHIOError;
    }
    if (!PHLuaLoadFile(L, initPath))
    {
        cleanupLua();
        luaMutex->unlock();
        throw PHLuaError;
    }
    loadFromLua();
    cleanupLua();
    luaMutex->unlock();
#ifdef PHIMAGE_ASYNCHRONEOUS_LOADING
    thread = new PHThread;
    thread->setFunction(this, (PHCallback)&PHAnimatedImage::loadImages, NULL);
    thread->start();
#else
    loadImages(NULL,NULL);
#endif
}

void PHAnimatedImage::loadFromLua()
{
    lua_newtable(L);
    PHLuaSetHardRef(L, this);
    int functions = 0;
    
    lua_getglobal(L, "def");
    if (lua_istable(L, -1))
    {
        lua_pushboolean(L, true);
        lua_setfield(L, -2, "default");
    }
    lua_pop(L, 1);
    
    int max = -1;
    
    lua_getglobal(L, "anim");
    if (lua_istable(L, -1))
    {
        int n = 0;
        PHLuaGetNumberField(n, "n");
        for (int i=0; i<n; i++)
        {
            lua_pushnumber(L, i);
            lua_gettable(L, -2);
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "default");
                if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
                    defaultSection = i;
                lua_pop(L,1);
                
                section * sec = new section;

                PHLuaGetStringField(sec->name, "nm");
                
                int m = 0;
                PHLuaGetNumberField(m, "n");
                for (int j=0; j<m; j++)
                {
                    lua_pushnumber(L, j);
                    lua_gettable(L, -2);
                    if (lua_istable(L, -1))
                    {
                        frame frm;
                        frm.frameno = 0;
                        frm.duration = 1.0f;
                        frm.type = 0;
                        frm.fade = false;
                        
                        PHLuaGetNumberField(frm.frameno, "fno");
                        PHLuaGetBoolField(frm.fade, "fd");
                        
                        lua_getfield(L, -1, "d");
                        if (lua_isnumber(L, -1))
                            frm.duration = lua_tonumber(L, -1);
                        else
                        if (lua_isfunction(L, -1))
                        {
                            frm.type = 1;
                            frm.fnumber = functions;
                            PHLuaGetHardRef(L, this);
                            lua_pushnumber(L, functions++);
                            lua_pushvalue(L, -3);
                            lua_settable(L, -3);
                            lua_pop(L,1);
                        }
                        else
                        if (lua_isstring(L, -1))
                        {
                            frm.type = 2;
                            frm.sectionName = new string(lua_tostring(L,-1));
                        }
                        lua_pop(L,1);
                        
                        if (frm.frameno > max)
                            max = frm.frameno;
                        
                        sec->frames.push_back(frm);
                    }
                    lua_pop(L, 1);
                }
                sections.push_back(sec);
            }
            lua_pop(L,1);
        }
    }
    lua_pop(L,1);
    
    nfrm = max+1;
    
    for (int i=0; i<sections.size(); i++)
    {
        section * sec = sections[i];
        for (int i=0; i<sec->frames.size(); i++)
        {
            if (sec->frames[i].type == 2)
            {
                string * s = sec->frames[i].sectionName;
                sec->frames[i].section = sectionNo(*s);
                delete s;
            }
        }
        frame f = sec->frames.back();;
        f.duration = 0;
        f.fade = false;
        f.type = 0;
        sec->frames.push_back(f);
    }
}

void PHAnimatedImage::cleanupLua()
{
    lua_pushnil(L);
    lua_setglobal(L, "nametable");
    lua_pushnil(L);
    lua_setglobal(L, "anim");
    lua_pushnil(L);
    lua_setglobal(L, "pntr");
    lua_pushnil(L);
    lua_setglobal(L, "def");
//    lua_gc(L, LUA_GCCOLLECT, 0); not quite necessary
}

void PHAnimatedImage::loadImages(PHObject *sender, void *ud)
{
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->lock();
#endif
    _width = _height = 0;
    int maxtexwidth = 1024;
    int maxtexheight = 1024;
    for (int i=0; i<nfrm; i++)
    {
        int w,h;
        
        png_byte color_type;
        png_byte bit_depth;
        
        png_structp png_ptr;
        png_infop info_ptr;
        png_bytep * row_pointers;
        int number_of_passes;
        
        uint8_t header[8];
        
        stringstream s;
        s<<path<<"/"<<i<<".png";
        FILE * fp = fopen(s.str().c_str(), "rb");
        if (!fp)
        {
            continue;
        }
        
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
        {
            fclose(fp);
            continue;
        }
        
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        
        if (!png_ptr)
        {
            fclose(fp);
            continue;
        }
        
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr,NULL,NULL);
            continue;
        }
        
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
            continue;
        }
        
        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);
        
        png_read_info(png_ptr, info_ptr);
        
        w = png_get_image_width(png_ptr, info_ptr);
        h = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        if (!_width && !_height)
        {
            _width = w;
            _height = h;
            clr = color_type;
            bdepth = bit_depth;
            cols = maxtexwidth / _width;
            rows = maxtexheight / _height;
            ipt = cols * rows;
        } else {
            if (_width != w ||
                _height != h ||
                clr != color_type ||
                bdepth != bit_depth)
            {
                fclose(fp);
                png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
                continue;
            }
        }
        number_of_passes = png_set_interlace_handling(png_ptr);
        
        png_read_update_info(png_ptr, info_ptr);
        
        
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
            continue;
        }
        
        int nt = i/ipt;
        if (textures.size()<=nt)
            textures.resize(nt+1);
        if (!textures[nt].buffer)
        {
            int c = cols;
            int r = rows;
            if (nfrm-nt*ipt<ipt)
            {
                int m = nfrm-nt*ipt;
                r = m/cols;
                if (m%cols)
                    r++;
                if (r<=1)
                    c = m;
            }
            c*=_width;
            r*=_height;
            int aw = 1;
            while (aw<c)
                aw<<=1;
            int ah = 1;
            while (ah<r)
                ah<<=1;
            textures[nt].awidth = aw;
            textures[nt].aheight = ah;
            textures[nt].rowsize = aw*png_get_rowbytes(png_ptr,info_ptr)/_width;
            textures[nt].size = textures[nt].rowsize * ah;
            textures[nt].buffer = new uint8_t[textures[nt].size];
        }
        
        int p = i-nt*ipt;
        int r = p/cols;
        int c = p%cols;
        uint8_t * buffer = textures[nt].buffer;
        size_t rowsize = textures[nt].rowsize;
        size_t height_offset = r*_height*rowsize;
        size_t width_offset = c*png_get_rowbytes(png_ptr,info_ptr);
        
        row_pointers = new png_bytep[_height];
        for (int j=0; j<_height; j++)
            row_pointers[j] = buffer + height_offset + rowsize*j + width_offset;
        
        png_read_image(png_ptr, row_pointers);
        
        delete[] row_pointers;
        fclose(fp);
        png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
    }
    if (bdepth != 8)
    {
        int scale = (bdepth>>3);
        for (int i=0; i<textures.size(); i++)
        {
            uint8_t * buffer = textures[i].buffer;
            size_t size = textures[i].size/scale;
            for (int j=0; j<size; j++)
                buffer[j] = buffer[j*scale];
        }
    }
    
    PHThread::mainThread()->executeOnThread(this, (PHCallback)&PHAnimatedImage::loadTextures, NULL, false);
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->unlock();
#endif
}

void PHAnimatedImage::loadTextures(PHObject *sender, void *ud)
{
    if (loaded) return;
    
    if (thread)
    {
        thread->join();
        thread->release();
    }
    
    GLint format = -1;
    if (clr == PNG_COLOR_TYPE_RGB)
        format = GL_RGB;
    if (clr == PNG_COLOR_TYPE_RGBA)
        format = GL_RGBA;
    if (clr == PNG_COLOR_TYPE_GRAY)
        format = GL_LUMINANCE;
    if (clr == PNG_COLOR_TYPE_GRAY_ALPHA)
        format = GL_LUMINANCE_ALPHA;
    if (format != -1)
    {
        for (int i=0; i<textures.size(); i++)
            if (textures[i].buffer)
            {
                glGenTextures(1,&textures[i].texid);
                glBindTexture(GL_TEXTURE_2D, textures[i].texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, antialiasing?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                if (antialiasing) 
                    glTexParameterf(GL_TEXTURE_2D,GL_GENERATE_MIPMAP, GL_TRUE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glTexImage2D(GL_TEXTURE_2D, 0, format, textures[i].awidth, textures[i].aheight, 0, 
                             format, GL_UNSIGNED_BYTE, textures[i].buffer);	
                delete [] textures[i].buffer;
            }
    } else
        for (int i=0; i<textures.size(); i++)
            delete [] textures[i].buffer;
    
    loaded = true;
}

int PHAnimatedImage::sectionNo(const string & sectionName)
{
    for (int i=0; i<sections.size(); i++)
        if (sections[i]->name == sectionName)
            return i;
    return -1;
}
    
PHAnimatedImage::~PHAnimatedImage()
{
    PHLuaDeleteHardRef(L, this);
    for (int i=0; i<sections.size(); i++)
        delete sections[i];
}

PHImageAnimator * PHAnimatedImage::newAnimator()
{
    return new PHImageAnimator(this);
}