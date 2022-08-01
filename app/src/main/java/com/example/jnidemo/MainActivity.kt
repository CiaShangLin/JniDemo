package com.example.jnidemo

import android.content.Context
import android.content.SharedPreferences
import android.content.pm.ApplicationInfo
import android.media.MediaDrm
import android.os.Bundle
import android.os.Process
import android.provider.Settings
import android.telephony.TelephonyManager
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import java.io.BufferedReader
import java.io.File
import java.io.FileReader
import java.io.IOException
import java.util.*
import kotlin.math.abs


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
        } catch (e: Exception) {
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
        Log.d("TAG", id)
        var p = packageManager.getApplicationInfo(packageName, 0)
        Log.d("TAG", p.className)
        Log.d("TAG", Jni.getApplication(this).toString())
        Log.d("TAG", Jni.getHost())

        Log.d("TAG", Jni.getModel())

        Log.d("TAG", "isMultiApp:" + Jni.isMultiApp(this).toString())

        Jni.sharedCommit(this)
        Log.d("DEBUG","${Jni.shared_get(this)}")
        Log.d("DEBUG","getDrmDeviceID:${getDrmDeviceID()}")
        Jni.drmTest(this)
    }

    fun getDrmDeviceID(): String? {
        val wideVineUuid = UUID(-0x121074568629b532L, -0x5c37d8232ae2de13L)
        val wvDrm = MediaDrm(wideVineUuid)
        val wideVineId = wvDrm.getPropertyByteArray(MediaDrm.PROPERTY_DEVICE_UNIQUE_ID)
        val str = StringBuilder("drm")
        wideVineId.forEach {
            val num = abs(it.toInt())
            if (num in 48..57) {
                str.append(num.toChar())
            } else if (num in 65..90) {
                str.append(num.toChar())
            } else if (num in 97..122) {
                str.append(num.toChar())
            }
        }
        return str.toString()
    }
}

