/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHFile.h>
#define PNG_DEBUG 4
#include <png.h>
#include <errno.h>
#include <arpa/inet.h>

PHGLTexture::PHGLTexture(PHGameManager * gameManager) : tex(0), gm(gameManager), wS(repeat), wT(repeat), wR(repeat), min(linear), mag(linear), mipMap(false), bound(false)
{
    PHGL::glGenTextures(1, &tex);
}

PHGLTexture::~PHGLTexture()
{
    PHGL::glDeleteTextures(1, &tex);
    gm->destroyTexture(this);
}

void PHGLTexture::_bind()
{
    PHGL::glBindTexture(target, tex);
}

void PHGLTexture::bind()
{
    gm->bindTexture(this);
}

void PHGLTexture::bind(int tmu)
{
    gm->setActiveTexture(tmu);
    gm->bindTexture(this);
}

#define bind_begin PHGLTexture * old = gm->boundTexture(); gm->bindTexture(this)
#define bind_end gm->bindTexture(old)

GLenum PHGLTexture::openGLEnumFromWrapType(enum wrapType t)
{
    static const GLenum thing[] = { 
#ifdef GL_CLAMP
        GL_CLAMP, 
#else 
        0,
#endif
#ifdef GL_CLAMP_TO_BORDER
        GL_CLAMP_TO_BORDER,
#else
        0,
#endif
#ifdef GL_CLAMP_TO_EDGE
        GL_CLAMP_TO_EDGE, 
#else
        0,
#endif
#ifdef GL_REPEAT
        GL_REPEAT
#else
        0
#endif
    };
    return thing[t];
}

GLenum PHGLTexture::openGLEnumFromAAFilter(enum aaFilter t)
{
    static const GLenum thing[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR };
    return thing[t];
}

#ifdef GL_TEXTURE_WRAP_S
void PHGLTexture::setWrapS(enum wrapType t)
{
    bind_begin;
    wS = t;
    PHGL::glTexParameteri(target, GL_TEXTURE_WRAP_S, openGLEnumFromWrapType(t));
    bind_end;
}
#endif

#ifdef GL_TEXTURE_WRAP_T
void PHGLTexture::setWrapT(enum wrapType t)
{
    bind_begin;
    wT = t;
    PHGL::glTexParameteri(target, GL_TEXTURE_WRAP_T, openGLEnumFromWrapType(t));
    bind_end;
}
#endif

#ifdef GL_TEXTURE_WRAP_R
void PHGLTexture::setWrapR(enum wrapType t)
{
    bind_begin;
    wR = t;
    PHGL::glTexParameteri(target, GL_TEXTURE_WRAP_R, openGLEnumFromWrapType(t));
    bind_end;
}
#endif

void PHGLTexture::setMinFilter(enum aaFilter t)
{
    bind_begin;
    min = t;
    PHGL::glTexParameteri(target, GL_TEXTURE_MIN_FILTER, openGLEnumFromAAFilter(t));
    updateMipMap();
    bind_end;
}

void PHGLTexture::setMagFilter(enum aaFilter t)
{
    bind_begin;
    mag = t;
    PHGL::glTexParameteri(target, GL_TEXTURE_MAG_FILTER, openGLEnumFromAAFilter(t));
    updateMipMap();
    bind_end;
}

bool PHGLTexture::supportsNPOT(PHGameManager * gm)
{
    return (gm->hasCapability(PHGLCapabilityNPOT) || gm->hasCapability(PHGLCapabilityAppleLimitedNPOT));
}

void PHGLTexture::updateMipMap()
{
    mipMap = (
            (min == nearestMipmapLinear) ||
            (min == linearMipmapLinear) ||
            (min == nearestMipmapNearest) ||
            (min == linearMipmapNearest) ||
            (mag == nearestMipmapLinear) ||
            (mag == linearMipmapLinear) ||
            (mag == nearestMipmapNearest) ||
            (mag == linearMipmapNearest)
                     );
}

#ifdef GL_TEXTURE_1D
void PHGLTexture1D::setData(uint8_t * data, size_t width, enum pixelFormat f)
{
    bind_begin;
    PHGLClearError();
    PHGL::glTexParameteri(target, GL_GENERATE_MIPMAP, hasMipMap()?GL_TRUE:GL_FALSE);
    PHGLCheckError();
    PHGL::glTexImage1D(target, 0, PHGLInternalFormats[f], width, 0, PHGLFormats[f], PHGLTypes[f], data);
    PHGLCheckError();
    w = width;
    pfmt = f;
    bind_end;
}
#endif

#ifdef GL_TEXTURE_2D
void PHGLTexture2D::setData(uint8_t *data, size_t width, size_t height, enum pixelFormat f)
{
    bind_begin;
    PHGLClearError();
    PHGL::glTexParameteri(target, GL_GENERATE_MIPMAP, hasMipMap()?GL_TRUE:GL_FALSE);
    PHGLCheckError();
    PHGL::glTexImage2D(target, 0, PHGLInternalFormats[f], width, height, 0, PHGLFormats[f], PHGLTypes[f], data);
    PHGLCheckError();
    w = width;
    h = height;
    pfmt = f;
    bind_end;
}

PHRect PHGLTexture2D::loadFromData(uint8_t * buf, size_t w, size_t h, size_t bw, size_t bh, enum pixelFormat fmt, bool aa)
{
    if (gm->hasCapability(PHGLCapabilityAppleLimitedNPOT))
    {
        bool pots = ((w & ~(w ^ (w-1))) == 0) && ((h & ~(h ^ (h-1))) == 0);
        if (!pots)
        {
            aa = false;
        }
    }
    
    bind_begin;
    setWrapS(clampToEdge);
    setWrapT(clampToEdge);
    setMinFilter(aa?linearMipmapNearest:linear);
    setMagFilter(linear);
    setData(buf, bw, bh, fmt);
    bind_end;
    
    return PHRect(0, 0, w/ph_float(bw), h/ph_float(bh));
}

PHRect PHGLTexture2D::loadFromFile(PHStream * fd, bool antialiasing)
{
    size_t size, width, height, bufW, bufH;
    enum pixelFormat format;
    uint8_t * b = dataFromFile(fd, width, height, bufW, bufH, !supportsNPOT(gm), size, format);
    
    PHRect r = loadFromData(b, width, height, bufW, bufH, format, antialiasing);
    
    delete[] b;
    return r;
}

PHRect PHGLTexture2D::loadFromFile(const string & fname, bool antialiasing)
{
    size_t size, width, height, bufW, bufH;
    enum pixelFormat format;
    uint8_t * b = dataFromFile(fname, width, height, bufW, bufH, !supportsNPOT(gm), size, format);
    
    PHRect r = loadFromData(b, width, height, bufW, bufH, format, antialiasing);
    
    delete[] b;
    return r;
}
#endif

#ifdef GL_TEXTURE_3D
void PHGLTexture3D::setData(uint8_t *data, size_t width, size_t height, size_t depth, enum pixelFormat f)
{
    bind_begin;
    PHGLCheckError();
    PHGL::glTexParameteri(target, GL_GENERATE_MIPMAP, hasMipMap()?GL_TRUE:GL_FALSE);
    PHGLCheckError();
    PHGL::glTexImage3D(target, 0, PHGLInternalFormats[f], width, height, depth, 0, PHGLFormats[f], PHGLTypes[f], data);
    PHGLCheckError();
    w = width;
    h = height;
    d = depth;
    pfmt = f;
    bind_end;
}
#endif

#ifdef GL_TEXTURE_CUBE_MAP
void PHGLTextureCubeMap::setData(uint8_t *data, size_t width, size_t height, enum cubemapFace face, enum pixelFormat f)
{
    bind_begin;
    static const GLenum faces[] = {
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    };
    PHGLClearError();
    PHGL::glTexParameteri(target, GL_GENERATE_MIPMAP, hasMipMap()?GL_TRUE:GL_FALSE);
    PHGLCheckError();
    PHGL::glTexImage2D(faces[face], 0, PHGLInternalFormats[f], width, height, 0, PHGLFormats[f], PHGLTypes[f], data);
    PHGLCheckError();
    bind_end;
}
#endif

uint8_t * PHGLTexture::dataFromFile(const string & fname, size_t & width, size_t & height, size_t & bufWidth, size_t & bufHeight, bool powerOfTwo, size_t & size, enum pixelFormat & format)
{
    PHStream * fd = NULL;
    PHAutoreleasePool ap;
    fd = PHInode::fileAtFSPath(fname);
    uint8_t * r;
    r = dataFromFile(fd, width, height, bufWidth, bufHeight, powerOfTwo, size, format);
    return r;
}

static void PHPNGReadData(png_structp png_ptr, png_bytep outBytes,
                          png_size_t byteCountToRead)
{
    PHFile * fd = (PHFile*)(void*)(png_get_io_ptr(png_ptr));
    try {
        fd->read(outBytes, byteCountToRead);
    } catch (string ex) {
        png_error(png_ptr, ex.c_str());
    }
}

uint8_t * PHGLTexture::dataFromFile(PHStream * fd, size_t & width, size_t & height, size_t & bufWidth, size_t & bufHeight, bool powerOfTwo, size_t & size, enum pixelFormat & format)
{
    uint8_t * buffer = NULL;
    
	png_byte color_type;
	png_byte bit_depth;
	
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep * row_pointers;
	int number_of_passes;
    
	uint8_t header[8];
	
    if (!fd->openedForReading())
        fd->open(PHStream::Read);
    fd->read(header, 8);
	if (png_sig_cmp(header, 0, 8))
	{
        throw string("Not a PNG file");
	}
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
	{
        throw string("Can't create PNG read struct");
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		throw string("Can't create PNG info struct");
	}
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
        throw string("PNG error");
	}
	
    png_set_read_fn(png_ptr, (void*)fd, PHPNGReadData);
	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);
	
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	number_of_passes = png_set_interlace_handling(png_ptr);
    if (!powerOfTwo)
    {
        bufWidth = width;
        bufHeight = height;
    } else
    {
        size_t actWidth = 1;
        size_t actHeight = 1;
        while (actWidth<width)
            actWidth<<=1;
        while (actHeight<height)
            actHeight<<=1;
        bufHeight = actHeight;
        bufWidth = actWidth;
    }
	
	png_read_update_info(png_ptr, info_ptr);
	
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		throw string("PNG error");
	}
	
	row_pointers = new png_bytep[height];
	for (size_t i=0; i<height; i++)
		row_pointers[i] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];
	
	png_read_image(png_ptr, row_pointers);
	
	size_t rowsize = png_get_rowbytes(png_ptr,info_ptr);
	size_t actrowsize = rowsize/width*bufWidth;
	size = actrowsize*bufHeight;
	
    buffer = new uint8_t[size];
	for (size_t i=0; i<height; i++)
	{
		memcpy(buffer+(height-i-1)*actrowsize, row_pointers[i],rowsize);
		delete[] row_pointers[i];
	}
	delete[] row_pointers;
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	
    
    format = InvalidFormat;
    switch (color_type) {
        case PNG_COLOR_TYPE_RGBA:
            if (bit_depth == 8)
                format = RGBA8;
            if (bit_depth == 16)
                format = RGBA16;
            break;
        case PNG_COLOR_TYPE_RGB:
            if (bit_depth == 8)
                format = RGB8;
            if (bit_depth == 16)
                format = RGB16;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if (bit_depth == 8)
                format = L8;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (bit_depth == 8)
                format = LA8;
    }
    
	if (format == InvalidFormat)
	{
		delete[] buffer;
        buffer = NULL;
        throw string("Unsupported color format");
	}
    
    if (bit_depth == 16)
    {
        size_t sz = size>>1;
        uint16_t * b = (uint16_t *)buffer;
        for (size_t i = 0; i<sz; i++)
            b[i] = htons(b[i]);
    }
    
    return buffer;
}
