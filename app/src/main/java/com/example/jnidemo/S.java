package com.example.jnidemo;

import android.content.Context;
import android.os.Build;
import android.provider.Settings;

public class S {
    void a(){
        StringBuffer stringBuffer=new StringBuffer();
        stringBuffer.append(9453);
        stringBuffer.toString();

        //String id = Settings.Secure.getString(MainActivity.this.getContentResolver(), Settings.Secure.ANDROID_ID);


        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;

        String name="123";
        name.charAt(0);


        StringBuilder stringBuilder=new StringBuilder("");

    }
}
