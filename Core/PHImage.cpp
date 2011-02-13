/*
 *  PHImage.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHImage.h"
#define PNG_DEBUG 4
#include <png.h>
#include <sstream>

map<string,PHImage*> PHImage::images;

PHImage::PHImage(const string & path)
{
//	size_t size;
//	uint8_t * buf = PHFileManager::singleton()->loadFile(path, size);
	
	glGenTextures(1,&texid);

	png_byte color_type;
	png_byte bit_depth;
	
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep * row_pointers;
	int number_of_passes;

	uint8_t header[8];
	
	FILE *fp = fopen(path.c_str(), "rb");
	if (!fp)
		throw PHIOError;
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose(fp);
		throw PHInvalidFileFormat;
	}
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
	{
		fclose(fp);
		throw PHInvalidFileFormat;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		throw PHInvalidFileFormat;
	}
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		throw PHInvalidFileFormat;
	}
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);
	
	_width = png_get_image_width(png_ptr, info_ptr);
	_height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	number_of_passes = png_set_interlace_handling(png_ptr);
	
	png_read_update_info(png_ptr, info_ptr);
	
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		throw PHInvalidFileFormat;
	}
	
	row_pointers = new png_bytep[_height];
	for (int i=0; i<_height; i++)
		row_pointers[i] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];
	
	png_read_image(png_ptr, row_pointers);
	
	size_t rowsize = png_get_rowbytes(png_ptr,info_ptr);
	size_t size = rowsize*_height;
	uint8_t * buffer = new uint8_t[size];
	for (int i=0; i<_height; i++)
	{
		memcpy(buffer+i*rowsize, row_pointers[i],rowsize);
		delete[] row_pointers[i];
	}
	delete[] row_pointers;
	fclose(fp);
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	
	GLint format = -1;
	if (color_type == PNG_COLOR_TYPE_RGB)
		format = GL_RGB;
	if (color_type == PNG_COLOR_TYPE_RGBA)
	{
		format = GL_RGBA;
	}
	if ((format == -1) || (bit_depth != 8))
	{
		delete[] buffer;
		throw PHInvalidFileFormat;
	}
	bindToTexture();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, 
				 format, GL_UNSIGNED_BYTE, buffer);
	for (int y=0; y<_height; y+=5)
	{
		for (int x=0; x<_height; x+=5)
			printf("%c",buffer[y*rowsize+x*4+3]?'X':' ');
		printf("\n");
	}
	
	//PHLog("%d",glGetError());
	
	delete[] buffer;
}

void PHImage::bindToTexture()
{
	glBindTexture(GL_TEXTURE_2D,texid);
}

PHImage * PHImage::imageFromPath(const string & path)
{
	PHImage * img;
	map<string,PHImage*>::iterator i = images.find(path);
	if (i==images.end())
	{
		try {
			img = new PHImage(path);
		} catch (string ex)
		{
			PHLog(ex);
			return NULL;
		}
		images[path] = img;
	} else {
		img = images[path];
	}
	img->retain();
	return img;
}

PHImage * PHImage::imageNamed(const string & string)
{
	return PHImage::imageFromPath(PHFileManager::singleton()->resourcePath()+"/img/"+string+".png");
}
void PHImage::clearImages()
{
	map<string,PHImage*>::iterator i;
	map<string,PHImage*> tmp;
	for (i = images.begin(); i!=images.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	images = tmp;
}