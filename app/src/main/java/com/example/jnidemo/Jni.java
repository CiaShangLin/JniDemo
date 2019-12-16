package com.example.jnidemo;

import android.content.Context;

public class Jni {
    public static native String helloJni();
    public static native StringBuffer getStringBuffer();
    public static native String getIMEI(Context context);
}
