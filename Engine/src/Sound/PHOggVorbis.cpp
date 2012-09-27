/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Sound/PHOggVorbis.h>
#include <vorbis/vorbisfile.h>
#include <Porkholt/IO/PHFile.h>

int ph_close(void * f)
{
    return ((PHOggVorbis*)f)->fclose();
}

long ph_tell(void * f)
{
    return ((PHOggVorbis*)f)->ftell();
}

int ph_seek(void * f, ogg_int64_t d, int t)
{ return ((PHOggVorbis*)f)->fseek(d, t);
}

size_t ph_read(void * d, size_t s, size_t nm, void * f)
{
    return ((PHOggVorbis*)f)->fread(d, s, nm);
}

size_t PHOggVorbis::fread(void * d, size_t size, size_t nmemb)
{
    if (!nmemb || !size) return 0;
    size_t r = min((fsz-fp)/size, nmemb);
    try { 
        r = file->read((uint8_t*)d, r * size)/size; 
        if (r<0) r=0;
        fp = fp + r;
        return r;
    }
    catch (...) { return 0; }
}

long PHOggVorbis::ftell()
{
    return fp;
}

int PHOggVorbis::fclose()
{
    //file->close();
    //this is done by PHAsyncDecoder anyway
    return 0;
}

int PHOggVorbis::fseek(int64_t d, int t)
{
    try {
        switch (t)
        {
            case SEEK_SET:
                file->setPointer(d);
                return fp = d;
            case SEEK_CUR:
                file->seek(d);
                return fp += d;
            case SEEK_END:
                file->setPointer(fsz+d);
                return fp = fsz + d;
        }
    } catch (...) {
        return -1;
    }
}

void PHOggVorbis::_loadHeader()
{
    ov = new OggVorbis_File;
    ov_callbacks cbs;
    cbs.close_func = &ph_close;
    cbs.read_func = &ph_read;
    cbs.seek_func = &ph_seek;
    cbs.tell_func = &ph_tell;
    file->open(PHFile::Read);
    fp = 0;
    fsz = file->size();
    if(ov_open_callbacks(this, ov, NULL, 0, cbs))
    {
        PHLog("Not an Ogg-Vorbis file: \"%s\"", file->path().c_str());
    }

    vorbis_info * vi = ov_info(ov, -1);
    chan = vi->channels;
    freq = vi->rate;
    len = ov_pcm_total(ov, -1);
    PHLog("ch %d freq %d len %d", chan, freq, len);
    ns = 0;
    fmt = U16;
}

#ifdef __BIG_ENDIAN__ 
#define end 1
#else
#define end 0
#endif

void PHOggVorbis::_loadData(uint8_t * data, size_t firstSample, size_t length)
{
    if (ns != firstSample)
        ov_pcm_seek(ov, firstSample);

    int cs;
    int fl = formatSampleLength()*chan;
    ns = firstSample+length;
    do {
        long r = ov_read(ov, (char*)data, length*fl, end, 2, 1, &cs);
        data+=r;
        length -= r/fl;
    }
    while(length);
}

void PHOggVorbis::_releaseFileHandle()
{
    ov_clear(ov);
}

