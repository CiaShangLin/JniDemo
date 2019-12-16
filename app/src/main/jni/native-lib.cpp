#include <Jni.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_helloJni(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("Hello Jni");
}