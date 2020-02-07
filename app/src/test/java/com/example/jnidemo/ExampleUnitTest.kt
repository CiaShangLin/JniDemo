package com.example.jnidemo

import org.junit.Test

import org.junit.Assert.*

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
class ExampleUnitTest {
    @Test
    fun first() {
        var appName: String = "wqeqwe"
        var versionName: String = "2.17.0"
        var versionCode: Int = 181

        var number = 0
        appName.forEach {
            number += it.toInt()
        }

        versionName.forEach {
            number += it.toInt()
        }
        number += versionCode

        println("-----first-----")
        println("number=$number")
        println(Integer.toHexString(number))
        println("---------------")

    }

    @Test
    fun second() {

        var versionName: String = "2.17.0"
        var versionCode: Int = 181

        var number = ""
        versionName.forEach {
            if (it != '.')
                number += it.toInt()
        }
        var a:String=(number.toInt()+versionCode).toString()

        println("-----second-----")
        println("number=$a")
        println(Integer.toHexString(a.toInt()))
        println("---------------")
    }


}
