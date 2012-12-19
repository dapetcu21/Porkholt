#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>

#ifdef DEBUG
#include <dirent.h>
#endif


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Porkholt", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Porkholt", __VA_ARGS__))

#define LIB_PATH "/data/data/@PH_BUNDLE_ID@/lib/"

void * load_lib(const char * l)
{
    void * handle = dlopen(l, RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
        LOGE("dlopen(\"%s\"): %s", l, dlerror());
        exit(1);
    }
    return handle;
}

void ANativeActivity_onCreate(ANativeActivity * app, void * ud, size_t udsize)
{
    LOGI("Loaded boostrap");

#ifdef DEBUG
    LOGI("Contents of \"%s\":", LIB_PATH);
    DIR *d;
    struct dirent *dir;
    d = opendir(LIB_PATH);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            LOGI(dir->d_name);
        }
        closedir(d);
    }
#endif

    load_lib(LIB_PATH "libgnustl_shared.so");
    load_lib(LIB_PATH "libpng15.so");
    load_lib(LIB_PATH "libluajit.so");
    load_lib(LIB_PATH "libopenal.so");
    load_lib(LIB_PATH "libogg.so");
    load_lib(LIB_PATH "libvorbis.so");
    load_lib(LIB_PATH "libvorbisfile.so");
    load_lib(LIB_PATH "libPorkholt.so");
    void (*main)(ANativeActivity*, void*, size_t) = dlsym(load_lib(LIB_PATH "lib@PH_APP_TARGET@.so"), "ANativeActivity_onCreate");
    if (!main)
    {
        LOGE("undefined symbol ANativeActivity_onCreate");
        exit(1);
    }
    main(app, ud, udsize);
}
