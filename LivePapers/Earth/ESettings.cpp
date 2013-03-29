#include "ESettings.h"

void ESettings::loadDefaults()
{
    position = PHVector3(0, 0, -5);
    size = 1.0f;
    mapType = 0;
    normalMapStrength = 0.0;
    rotationSpeed = 0.13;
    athmosColor = PHColor(0.41f, 0.70f, 0.96f);
    color1 = PHWhiteColor;
    color2 = PHBlackColor;

    earthRoll = -0.313f;
    earthPitch = -1.25f;
    cameraRoll = 1.378f;

    stars = true;
    specular = true;
    night = true;
    bgType = 2;
    bgColor = PHBlackColor;

    staticSun = true;
    sunAngle = 1.75f;

    vertexShading = false;
    fpsType = 2;
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
