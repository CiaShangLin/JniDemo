#include <Jni.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_helloJni(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("Hello Jni");
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getIMEI(JNIEnv *env, jclass clazz, jobject context) {

    jclass setting_class = env->FindClass("android/provider/Settings$Secure");
    jmethodID getString = env->GetStaticMethodID(setting_class, "getString",
                                                 "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");

    jclass context_class = env->GetObjectClass(context);
    jmethodID getContentResolver = env->GetMethodID(context_class, "getContentResolver",
                                                    "()Landroid/content/ContentResolver;");
    jobject contentResolver = env->CallObjectMethod(context, getContentResolver);

    jstring android_id = env->NewStringUTF("android_id");
    jstring IMEI = (jstring) env->CallStaticObjectMethod(setting_class, getString, contentResolver,
                                                         android_id);

    env->DeleteLocalRef(android_id);
    env->DeleteLocalRef(setting_class);
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(contentResolver);
    return IMEI;

}