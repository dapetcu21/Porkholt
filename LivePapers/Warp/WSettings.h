#ifndef WSETTINGS_H
#define WSETTINGS_H

#include <Porkholt/Core/PHMain.h>

struct WarpSettings
{
    float speed;
    float starsPerSecond;
    float starLength;
    float starWidth;
    bool wallpaper;
    PHColor background;
    PHColor foreground;
    int warpType;

    void load();
    void save();
    void loadDefaults();
};

#endif
