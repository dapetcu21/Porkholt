/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHDRAWABLECOORDINATES_H
#define PHDRAWABLECOORDINATES_H

#include <Porkholt/Core/PHMain.h>

class PHDrawable;
class PHDrawableCoordinates : public PHObject
{
    private:
        map<PHDrawable*, PHPositionalVector> coord;
    public:
        PHDrawableCoordinates(const PHPositionalVector & screenSpace, PHDrawable * d = NULL);

        void propagateUpstream(PHDrawable * d, PHDrawable * until = NULL);
        PHPoint pointInView(PHDrawable * d);
        void clearCache();

        PHPositionalVector positionInDrawable(PHDrawable*);
        void setPositionInDrawable(PHDrawable*, const PHPositionalVector &);
};

#endif
