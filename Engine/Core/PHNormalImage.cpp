//
//  PHNormalImage.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/9/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHNormalImage.h"
#define PNG_DEBUG 4
#include <png.h>
#include <sstream>
#include "PHFileManager.h"
#include "PHImageView.h"
#include "PHGameManager.h"

PHNormalImage::PHNormalImage(const string & path, PHGameManager * gm): PHImage(path,gm), texid(-1), thread(NULL)
{
    fp = NULL;
    if (PHGameManager::isGloballyHD())
        fp = fopen((path+".hd").c_str(), "rb");
    if (!fp)
        fp = fopen(path.c_str(), "rb");
	if (!fp)
		throw PHIOError + ": " + path;
    antialiasing = PHFileManager::fileExists(path+".aa");
#ifdef PHIMAGE_ASYNCHRONEOUS_LOADING
    thread = new PHThread;
    thread->setFunction(PHInv(this,PHNormalImage::loadFromFile, NULL));
    thread->start();
#else
    loadFromFile(this, NULL);
#endif  
}

void PHNormalImage::loadFromFile(PHObject *sender, void *ud)
{
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->lock();
#endif
    buffer = NULL;
    
	png_byte color_type;
	png_byte bit_depth;
	
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep * row_pointers;
	int number_of_passes;
    
	uint8_t header[8];
	
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose(fp);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
	{
		fclose(fp);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,NULL,NULL);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);
	
	_width = png_get_image_width(png_ptr, info_ptr);
	_height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	number_of_passes = png_set_interlace_handling(png_ptr);
    if (PHGLHasCapability(PHGLCapabilityNPOT) || PHGLHasCapability(PHGLCapabilityAppleLimitedNPOT))
    {
        actWidth = _width;
        actHeight = _height;
    } else
    {
        actWidth = 1;
        actHeight = 1;
        while (actWidth<_width)
            actWidth<<=1;
        while (actHeight<_height)
            actHeight<<=1;
    }
	
	png_read_update_info(png_ptr, info_ptr);
	
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
	
	row_pointers = new png_bytep[_height];
	for (int i=0; i<_height; i++)
		row_pointers[i] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];
	
	png_read_image(png_ptr, row_pointers);
	
	size_t rowsize = png_get_rowbytes(png_ptr,info_ptr);
	size_t actrowsize = rowsize/_width*actWidth;
	size_t size = actrowsize*actHeight;
	
    buffer = new uint8_t[size];
	for (int i=0; i<_height; i++)
	{
		memcpy(buffer+i*actrowsize, row_pointers[i],rowsize);
		delete[] row_pointers[i];
	}
	delete[] row_pointers;
	fclose(fp);
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	
	format = -1;
	if (color_type == PNG_COLOR_TYPE_RGB)
		format = GL_RGB;
    if (color_type == PNG_COLOR_TYPE_GRAY)
        format = GL_LUMINANCE;
    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        format = GL_LUMINANCE_ALPHA;
	if (color_type == PNG_COLOR_TYPE_RGBA)
		format = GL_RGBA;
	if (format == -1)
	{
		delete[] buffer;
        buffer = NULL;
#ifdef PHIMAGE_ORDERED_LOADING
        loadingMutex->unlock();
#endif
		return;
	}
    
    if (bit_depth != 8)
    {
        int scale = (bit_depth>>3);
        for (int i=0; i<size/scale; i++)
            buffer[i] = buffer[i*scale];
    }
    
    PHThread::mainThread()->executeOnThread(PHInv(this, PHNormalImage::loadToTexture, NULL), false);
#ifdef PHIMAGE_ORDERED_LOADING
    loadingMutex->unlock();
#endif
}

void PHNormalImage::loadToTexture(PHObject * sender, void * ud)
{	
    if (loaded) return;
    
    if (thread)
    {
        thread->join();
        thread->release();
    }
    
	glGenTextures(1,&texid);
	bindToTexture(0);
    
    bool repeat = true;
    if (PHGLHasCapability(PHGLCapabilityAppleLimitedNPOT))
    {
        bool pots = true;
        int s = _width;
        while (s && !(s&1))
            s>>=1;
        if (s!=1)
            pots = false;
        if (pots)
        {
            s = _height;
            while (s && !(s&1))
                s>>=1;
            if (s!=1)
                pots = false;
        }
        if (!pots)
        {
            antialiasing = false;
            repeat = false;
        }
    }
    
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, antialiasing?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (antialiasing) 
		glTexParameterf(GL_TEXTURE_2D,GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, actWidth, actHeight, 0, 
				 format, GL_UNSIGNED_BYTE, buffer);	
	delete[] buffer;
    loaded = true;
}

PHNormalImage::~PHNormalImage()
{
	glDeleteTextures(1, &texid);
}

void PHNormalImage::bindToTexture(int tx)
{
    glActiveTexture(GL_TEXTURE0+tx);
    glBindTexture(GL_TEXTURE_2D,texid);
}

PHRect PHNormalImage::textureCoordinates(const PHRect & port)
{
    ph_float xc = 0.5f/actWidth;
    ph_float yc = 0.5f/actHeight;
	ph_float xC = (ph_float)_width/actWidth;
	ph_float yC = (ph_float)_height/actHeight;
	
    return PHRect(xC*port.x+xc,yC*(port.y+port.height)-yc,xC*port.width-2*xc,-yC*port.height+2*yc);
}

void PHNormalImage::renderInFramePortionTint(PHGameManager * _gameManager, const PHRect & frm,const PHRect & port,const PHColor & tint)
{
    load();
    
	const GLfloat squareVertices[] = {
        frm.x,			frm.y,
        frm.x+frm.width,frm.y,
        frm.x,			frm.y+frm.height,
        frm.x+frm.width,frm.y+frm.height,
    };
	
    ph_float xc = 0.5f/actWidth;
    ph_float yc = 0.5f/actHeight;
	ph_float xC = (ph_float)_width/actWidth;
	ph_float yC = (ph_float)_height/actHeight;
	
	const GLfloat squareTexCoords[] = {
        xC*port.x+xc				, yC*(port.y+port.height)-yc,
		xC*(port.x+port.width)-xc	, yC*(port.y+port.height)-yc,
		xC*port.x+xc				, yC*port.y+yc,
		xC*(port.x+port.width)-xc	, yC*port.y+yc,
    };
    
    int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture;
    PHGLSetStates(states);
    bindToTexture(0);
    PHGLSetColor(tint);
    _gameManager->applySpriteShader();
    if (_gameManager->useShaders())
    {
        glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
        glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords);
    } else {
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHNormalImage::rebuildVBO(PHImageView * imageView, GLuint & vbo, VBOParams & params)
{
    if (!vbo)
        glGenBuffers(1, &vbo);
    PHImage::buildImageVBO(vbo,
                           params,
                           PHPoint(imageView->repeatX(),imageView->repeatY()),
                           imageView->textureCoordinates(),
                           PHRect(0,0,(ph_float)_width/actWidth,(ph_float)_height/actHeight),
                           PHPoint(0.5f/actWidth,0.5f/actHeight)
                           );
}