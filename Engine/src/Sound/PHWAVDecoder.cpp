/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHWAVDecoder.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHSerialization.h>

#define to32(x) PHLEToH32(*((const uint32_t*)(x)))
#define to32A(x) (*((const uint32_t*)(x))) 

void PHWAVDecoder::_loadHeader()
{
    string fmterr("Not a valid WAVE file");
    file->open(PHFile::Read);
    size_t fsz = file->size();
    if (fsz < 12) 
        throw fmterr;
    uint8_t ch[12];
    file->read(ch, 12);
    if ((to32A(ch) != to32A("RIFF")) || (to32A(ch+8) != to32A("WAVE")))
        throw fmterr;
    size_t chsz = to32(ch+4) + 8;
    if (chsz > fsz)
        throw fmterr;

    size_t off = 12;
    size_t dlen;

    while (off < chsz)
    {
        uint8_t buf[8];
        file->read(buf, 8);
        uint32_t sig = to32A(buf);
        size_t chsiz = to32(buf + 4);
        if (chsiz + off + 8 > chsz)
            throw fmterr;
        if (sig == to32A("fmt "))
        {
            if (chsiz < 16)
                throw fmterr;
            uint8_t b[16];
            file->read(b, 16);
            if (chsiz != 16)
                file->seek(chsiz - 16);
           
            uint16_t audiofmt = PHLEToH16(*((uint16_t*)(b)));
            if (audiofmt != 1)
               throw "This implementation only supports uncompressed PCM WAVE files";
            chan = PHLEToH16(*((uint16_t*)(b+2)));
            freq = PHLEToH32(*((uint32_t*)(b+4)));
            uint16_t bps = PHLEToH16(*((uint16_t*)(b+14)));
            switch (bps)
            {
                case 8:
                   fmt = U8;
                   break;
                case 16:
                    fmt = U16;
                    break;
                default:
                    throw "Invalid sample format";
            };
        } else if (sig == to32A("data"))
        {
            dlen = chsiz;
            doff = off+8;
            file->seek(chsiz);
        } else
            file->seek(chsiz);
        off += chsiz + 8;
    }

    len = dlen / formatSampleLength() / chan;
}

void PHWAVDecoder::_loadData(uint8_t * data, size_t start, size_t len)
{
    size_t bps = formatSampleLength() * chan;
    file->setPointer(doff + bps * start);
#ifdef __BIG_ENDIAN__ 
    if (fmt == U16)
    {
        return;
    }
#endif
    file->read(data, len * bps);
}
