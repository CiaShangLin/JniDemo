package com.example.jnidemo

import android.content.Context
import android.os.Bundle
import android.provider.Settings
import android.telephony.TelephonyManager
import android.util.Log
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        try {
            Jni.throwExcetion()
        }catch (e:Exception){
            e.printStackTrace()
        }

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
        Log.d("TAG", "isDebug:" + Jni.isDebug(this.application))
        var id = Settings.Secure.getString(contentResolver, Settings.Secure.ANDROID_ID);
        Log.d("TAG",id)
        var p =packageManager.getApplicationInfo(packageName,0)
        Log.d("DEBUG",p.className)
        Log.d("DEBUG",Jni.getApplication(this).toString())

        Log.d("DEBUG",Jni.getHost())
    }
}
