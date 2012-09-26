/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

package org.porkholt.jniloader;

import android.util.Log;
import android.util.DisplayMetrics;
import android.view.Surface;
import android.view.Display;
import android.app.NativeActivity;
import android.content.Context;
import android.view.WindowManager;

public class JNILoader
{
    NativeActivity activity;

    JNILoader(NativeActivity a)
    {
        activity = a;
        Log.i("Porkholt", "JNILoader loaded");
    }

    float dpi;
    float refreshRate;
    int orientation;

    void load()
    {
        DisplayMetrics metrics = new DisplayMetrics();
        Display display = ((WindowManager)activity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getMetrics(metrics);
        dpi = 160 * metrics.density;
        refreshRate = display.getRefreshRate();
        orientation = display.getOrientation();
    }

};
