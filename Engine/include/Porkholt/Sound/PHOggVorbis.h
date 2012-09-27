/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHOGGVORBIS_H
#define PHOGGVORBIS_H

#include <Porkholt/Sound/PHAsyncDecoder.h>

struct OggVorbis_File;

class PHOggVorbis : public PHAsyncDecoder 
{
    OggVorbis_File * ov;
    size_t fp, fsz;
    size_t ns;
    protected:
        void _loadHeader();
        void _loadData(uint8_t * data, size_t firstSample, size_t length);
        void _releaseFileHandle();

        int fclose();
        size_t fread(void * d, size_t s, size_t nmemb);
        int fseek(int64_t d, int t);
        long ftell();

        friend int ph_close(void * f);
        friend size_t ph_read(void * d, size_t s, size_t nmemb, void * f);
        friend int ph_seek(void * f, int64_t d, int t);
        friend long ph_tell(void * f);

};

#endif
