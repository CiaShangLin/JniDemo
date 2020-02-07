#include <Jni.h>
#include <string>
#include <string.h>   //C的字串函數庫
#include <malloc.h>   //分配記憶體


jstring getVersionName(JNIEnv *env, jobject context){
    jclass context_class = env->GetObjectClass(context);
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                          "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context, methodId);   //取得PackageManager

    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring) env->CallObjectMethod(context, methodId);   //取得PackageName

    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getPackageInfo",
                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name,
                                                 0);  //取得PackageInfo

    jclass package_info_class = env->GetObjectClass(package_info);
    jfieldID jfieldId = env->GetFieldID(package_info_class, "versionName", "Ljava/lang/String;");
    jstring versionName = (jstring) env->GetObjectField(package_info, jfieldId);

    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(package_manager);
    env->DeleteLocalRef(package_info_class);
    env->DeleteLocalRef(package_info);

    return versionName;
}

jint getVersionCode(JNIEnv *env, jobject context){
    jclass context_class = env->GetObjectClass(context);
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                          "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context, methodId);   //取得PackageManager

    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring) env->CallObjectMethod(context, methodId);   //取得PackageName

    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getPackageInfo",
                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name,
                                                 0);  //取得PackageInfo

    jclass package_info_class = env->GetObjectClass(package_info);
    jfieldID jfieldId = env->GetFieldID(package_info_class, "versionCode", "I");
    jint versionCode = env->GetIntField(package_info, jfieldId);

    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(package_manager);
    env->DeleteLocalRef(package_info_class);
    env->DeleteLocalRef(package_info);

    return versionCode;
}