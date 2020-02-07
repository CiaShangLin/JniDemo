package com.example.jnidemo

import android.content.pm.ApplicationInfo
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import java.io.File


class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        Log.d("TAG", "helloJni:" + Jni.helloJni())
        Log.d("TAG", "getStringBuffer:" + Jni.getStringBuffer().toString())
        Log.d("TAG", "getIMEI:" + Jni.getIMEI(this))
        Log.d("TAG", "getDeviceName:" + Jni.getDeviceName())
        Log.d("TAG", "getVersionName:" + Jni.getVersionName(this))
        Log.d("TAG", "getKeySha1:" + Jni.getKeySha1(this))
        Log.d("TAG", "getVersionCode:" + Jni.getVersionCode(this))
        Log.d("TAG", "DexSize:" + Jni.DexSize(this))
        Log.d("TAG", "versionCypto:" + Jni.versionCypto(this))
        Log.d("TAG", "versionCypto2:" + Jni.versionCypto2(this))

        Log.d("TAG",application.applicationInfo.flags.toString())
        Log.d("TAG", ApplicationInfo.FLAG_DEBUGGABLE.toString())
        var isDebug:Boolean= (ApplicationInfo.FLAG_DEBUGGABLE and application.applicationInfo.flags)!=0
        Log.d("TAG", isDebug.toString())



    }
}
