#include "WSettings.h"
#include <Foundation/Foundation.h>

#define EPrefsPath @"/var/mobile/Library/Preferences/org.porkholt.livepapers.warp.plist"
#define EPosX @"EPosX"
#define EPosY @"EPosY"
#define EPosZ @"EPosZ"
#define ESize @"ESize"
#define EMapType @"EMapType"
#define EBgType @"EBgType"
#define EBgColor @"EBgColor"
#define ENormalMap @"ENormalMap"
#define EAthmosColor @"EAthmosColor"
#define EColor1 @"EColor1"
#define EColor2 @"EColor2"
#define ERotationSpeed @"ERotationSpeed"
#define ERoll @"ERoll"
#define EPitch @"EPitch"
#define ECamRoll @"ECamRoll"
#define EStars @"EStars"
#define ENight @"ENight"
#define ESpecular @"ESpecular"
#define EStaticSun @"EStaticSun"
#define ESunAngle @"ESunAngle"
#define EVertexShading @"EVertexShading"
#define EFPSType @"EFPSType"

#define getparam(key, type, var) nr = [dict objectForKey:key]; if([nr isKindOfClass:[NSNumber class]]) var = nr. type ## Value; 
#define setparam(key, type, value) [dict setObject:[NSNumber numberWith ## type:value] forKey:key]

#define getparamf(key, var) getparam(key, float, var)
#define getparami(key, var) getparam(key, int, var)
#define getparamb(key, var) getparam(key, bool, var)

#define setparamf(key, var) setparam(key, Float, var)
#define setparami(key, var) setparam(key, Int, var)
#define setparamb(key, var) setparam(key, Bool, var)

#define setparamc(key, value) \
    setparam(key "R", Float, value.r); \
    setparam(key "G", Float, value.g); \
    setparam(key "B", Float, value.b); \
    setparam(key "A", Float, value.a)

#define getparamc(key, value) \
    getparam(key "R", float, value.r); \
    getparam(key "G", float, value.g); \
    getparam(key "B", float, value.b); \
    getparam(key "A", float, value.a)

void WSettings::load()
{
    loadDefaults();
    @autoreleasepool {
        NSDictionary * dict = [NSDictionary dictionaryWithContentsOfFile:EPrefsPath];
        NSNumber * nr;

        getparamf(EPosX, position.x);
        getparamf(EPosY, position.y);
        getparamf(EPosZ, position.z);
        getparamf(ESize, size);
        getparami(EMapType, mapType);
        getparamf(ENormalMap, normalMapStrength);
        getparamf(ERotationSpeed, rotationSpeed);
        getparamc(EAthmosColor, athmosColor);
        getparamc(EColor1, color1);
        getparamc(EColor2, color2);
        getparamf(ERoll, earthRoll);
        getparamf(EPitch, earthPitch);
        getparamf(ECamRoll, cameraRoll);
        getparamb(EStars, stars);
        getparamb(ENight, night);
        getparamb(ESpecular, specular);
        getparami(EBgType, bgType);
        getparamc(EBgColor, bgColor);
        getparamb(EStaticSun, staticSun);
        getparamf(ESunAngle, sunAngle);
        getparamb(EVertexShading, vertexShading);
        getparami(EFPSType, fpsType);
    }
}

void WSettings::save()
{
    @autoreleasepool {
        NSMutableDictionary * dict = [[NSMutableDictionary alloc] init];

        setparamf(EPosX, position.x);
        setparamf(EPosY, position.y);
        setparamf(EPosZ, position.z);
        setparamf(ESize, size);
        setparami(EMapType, mapType);
        setparamf(ENormalMap, normalMapStrength);
        setparamf(ERotationSpeed, rotationSpeed);
        setparamc(EAthmosColor, athmosColor);
        setparamc(EColor1, color1);
        setparamc(EColor2, color2);
        setparamf(ERoll, earthRoll);
        setparamf(EPitch, earthPitch);
        setparamf(ECamRoll, cameraRoll);
        setparamb(EStars, stars);
        setparamb(ENight, night);
        setparamb(ESpecular, specular);
        setparami(EBgType, bgType);
        setparamc(EBgColor, bgColor);
        setparamb(EStaticSun, staticSun);
        setparamf(ESunAngle, sunAngle);
        setparamb(EVertexShading, vertexShading);
        setparami(EFPSType, fpsType);

        [dict writeToFile:EPrefsPath atomically:YES];
        [dict release];
    }
}
