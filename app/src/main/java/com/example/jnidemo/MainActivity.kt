package com.example.jnidemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

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
    }
}
