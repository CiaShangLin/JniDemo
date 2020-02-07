package com.example.jnidemo;

import android.content.Context;

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

}
