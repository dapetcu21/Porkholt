#import "EPrefsViewController.h"

extern "C" void * __attribute__((visibility("default"))) LPInitPrefsViewController(NSDictionary * info)
{
    return [[EPrefsViewController alloc] init];
}

@implementation EPrefsViewController

enum prefs
{
    EPosX,
    EPosY,
    EPosZ,
    ESize,
    EMapType,
    ENormalMap,
    EAthmosColor,
    EAthmosAlpha,
    EColor1,
    EColor2,
    ERotationSpeed,
    ERoll,
    EPitch,
    ECamRoll,
    EStars,
    ESpecular,
    EBgType,
    EBgColor,
    ENight,
    EStaticSun,
    ESunAngle,
    EVertexShading,
    EFPSType,
};

#define getparam(key, type, var) [self prefWithTag:key]. type ## Value = s.var
#define setparam(key, type, var) case key: s.var = pref. type ## Value; break

#define getparamf(key, var) getparam(key, float, var)
#define getparami(key, var) getparam(key, int, var)
#define getparamb(key, var) getparam(key, bool, var)

#define setparamf(key, var) setparam(key, float, var)
#define setparami(key, var) setparam(key, int, var)
#define setparamb(key, var) setparam(key, bool, var)

#define getparamc(key, var) [self prefWithTag:key].objectValue = [UIColor colorWithRed:s.var.r green:s.var.g blue:s.var.b alpha:s.var.a]
#define setparamc(key, var) case key: { UIColor * c = (UIColor*)pref.objectValue; CGFloat r,g,b,a; [c getRed:&r green:&g blue:&b alpha:&a]; s.var = PHColor(r,g,b,a); break; }

#define hid(key) [self prefWithTag:key].hidden

-(void)setHiddens
{
    hid(EColor1) = s.mapType != 1;
    hid(EColor2) = s.mapType != 1;
    hid(EBgColor) = s.bgType != 0;
    hid(ESunAngle) = !s.staticSun;
}

-(void)loadPreferences
{
    s.load();
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
    getparamb(ESpecular, specular);
    getparami(EBgType, bgType);
    getparamc(EBgColor, bgColor);
    getparamb(ENight, night);
    getparamb(EStaticSun, staticSun);
    getparamf(ESunAngle, sunAngle);
    getparamb(EVertexShading, vertexShading);
    getparami(EFPSType, fpsType);

    getparamf(EAthmosAlpha, athmosColor.a);

    self.animateTransitions = NO;
    [self setHiddens];
    self.animateTransitions = YES;
}

-(void)savePreferences
{
    s.save();
}

-(void)resetToDefaults
{
    s.loadDefaults();
    s.save();
    [self loadPreferences];
}

-(void)loadView
{
    prefSlider(EPosX, @"Position X", @"%.2f", -5, 5);
    prefSlider(EPosY, @"Position Y", @"%.2f", -5, 5);
    prefSlider(EPosZ, @"Position Z", @"%.2f", -10, 0);
    prefSlider(ESize, @"Earth size", @"%.2f", 0, 2);
    prefSlider(ERoll, @"Earth roll", @"%.2f", -M_PI, M_PI);
    prefSlider(EPitch, @"Earth pitch", @"%.2f", -M_PI, M_PI);
    prefSlider(ECamRoll, @"Camera roll", @"%.2f", -M_PI, M_PI);
    prefSlider(ERotationSpeed, @"Rotation speed", @"%.2f", 0, 2);
    prefSegment(EStaticSun, @"Time of day", @"Clock", @"Static");
    prefSlider(ESunAngle, @"Static time of day", @"%.2f", -M_PI, M_PI); //TO DO: Date picker

    prefSpacer_();
    prefColor(EAthmosColor, @"Atmosphere color");
    prefSlider(EAthmosAlpha, @"Atmosphere opacity", @"%.2f", 0, 1);
    prefSwitch(EMapType, @"Alternative map");
    prefColor(EColor1, @"Map foreground");
    prefColor(EColor2, @"Map background");
    prefSegment(EBgType, @"Background", @"Color", @"Wallpaper");
    prefColor(EBgColor, @"Background color");

    prefSpacer_();
    prefSwitch(ENight, @"Night lights");
    prefSwitch(EStars, @"Stars");
    prefSwitch(ESpecular, @"Specular highlights");
    prefSlider(ENormalMap, @"Normal map strength", @"%.2f", 0, 1);
    prefSegment(EFPSType, @"Frames per second", @"20", @"30", @"60");
    prefSegment(EVertexShading, @"Optimize", @"Quality", @"Speed");

    [super loadView];
    [self loadPreferences];
}

-(void)prefDidChange:(LPPref*)pref
{
    switch(pref.tag)
    {
        setparamf(EPosX, position.x);
        setparamf(EPosY, position.y);
        setparamf(EPosZ, position.z);
        setparamf(ESize, size);
        setparami(EMapType, mapType);
        setparamf(ENormalMap, normalMapStrength);
        setparamf(ERotationSpeed, rotationSpeed);
        setparamc(EColor1, color1);
        setparamc(EColor2, color2);
        setparamf(ERoll, earthRoll);
        setparamf(EPitch, earthPitch);
        setparamf(ECamRoll, cameraRoll);
        setparamb(EStars, stars);
        setparamb(ESpecular, specular);
        setparamb(ENight, night);
        setparami(EBgType, bgType);
        setparamc(EBgColor, bgColor);
        setparamb(EStaticSun, staticSun);
        setparamf(ESunAngle, sunAngle);
        setparamb(EVertexShading, vertexShading);
        setparami(EFPSType, fpsType);

        case EAthmosColor:
        {
            UIColor * c = (UIColor*)pref.objectValue;
            CGFloat r,g,b,a;
            [c getRed:&r green:&g blue:&b alpha:&a];
            s.athmosColor.r = r;
            s.athmosColor.g = g;
            s.athmosColor.b = b;
            break;
        }
        case EAthmosAlpha:
            s.athmosColor.a = pref.floatValue;
            break;
    }
    [self setHiddens];
}

@end



