#include "ESettings.h"

void ESettings::loadDefaults()
{
    position = PHVector3(0, 0, -5);
    size = 1.0f;
    mapType = 0;
    normalMapStrength = 0.0;
    rotationSpeed = 0.5;
    athmosColor = PHColor(0, 158.0 / 255.0, 219.0 / 255.0, 0.6f);
    color1 = PHWhiteColor;
    color2 = PHBlackColor;

    earthRoll = 0;
    earthPitch = 0;
    cameraRoll = 0;

    stars = true;
    specular = true;
    night = true;
    bgType = 0;
    bgColor = PHBlackColor;

    staticSun = false;
    sunAngle = 0.0f;

    vertexShading = false;
    fpsType = 1;
}

#ifndef PH_LIVEPAPERS
void ESettings::load()
{
    loadDefaults();
}

void ESettings::save()
{
}
#endif
