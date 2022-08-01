package com.example.jnidemo;

import android.app.Application;
import android.content.Context;
import android.content.ContextWrapper;

import androidx.annotation.Keep;


public class Jni {
    public static native String helloJni();

    public static native StringBuffer getStringBuffer();

    public static native String getIMEI(Context context);

    public static native String getDeviceName();

    public static native String getVersionName(Context context);

    public static native String getKeySha1(Context context);

    public static native int getVersionCode(Context context);

    public static native String DexSize(Context context);

    public static native String versionCypto(Context context);

    public static native String versionCypto2(Context context);

    public static native boolean isDebug(Application application);

    public static native void firebaseNDKCrash();

    public static native boolean getApplication(Context context);

    public static native void throwExcetion();

    public static native String getHost();

    public static native String getModel();

    public static native boolean isMultiApp(Context context);

    public static native void sharedCommit(Context context);

    public static native boolean shared_get(Context context);

    public static native void drmTest(Context context);
}
