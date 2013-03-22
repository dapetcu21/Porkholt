#ifndef ESETTINGS_H
#define ESETTINGS_H

#include <Porkholt/Core/PHMain.h>

struct ESettings
{
    PHVector3 position;
    float size;
    int mapType;
    float normalMapStrength;
    PHColor athmosColor;
    PHColor color1, color2;
    PHColor bgColor;
    int bgType;
    float rotationSpeed;

    float earthRoll;
    float earthPitch;
    float cameraRoll; 

    bool stars;
    bool specular;
    bool night;

    bool staticSun;
    float sunAngle;

    int fpsType;
    bool vertexShading;

    void load();
    void save();
    void loadDefaults();
};

#endif
