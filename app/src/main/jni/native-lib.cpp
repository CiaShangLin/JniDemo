#include <Jni.h>


//導入這三個就可以用Android的Log,CMakeLists.txt那裡也有導入一些東西要注意
//不過這裡的LOG輸出法是C的printf在輸入字串的時候不能用jstring要用char*
//希望有人可以提供更好用的,不然型別轉來轉去很累
#include <android/log.h>

#define TAG    "jni-log"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

#include <string.h>   //C的字串函數庫
#include <malloc.h>   //分配記憶體

#include <string>     //C++的字串
#include <array>

#include <random>

#include "Version.cpp"

using namespace std;  //C++

static jboolean IS_DEV = false;

//jstring to char*
char *Jstring2CStr(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

//char轉jstring
jstring charToJstring(JNIEnv *env, const char *src) {
    jsize len = strlen(src);
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("UTF-8");
    jmethodID mid = env->GetMethodID(clsstring, "<init>", "([BLjava/lang/String;)V");
    jbyteArray barr = env->NewByteArray(len);
    env->SetByteArrayRegion(barr, 0, len, (jbyte *) src);

    return (jstring) env->NewObject(clsstring, mid, barr, strencode);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_helloJni(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("Hello Jni");
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_jnidemo_Jni_getStringBuffer(JNIEnv *env, jclass clazz) {

//    //Java
//    StringBuffer stringBuffer=new StringBuffer();
//    stringBuffer.append(9453);
//    stringBuffer.toString();

    //取得StringBuffer的class
    jclass StringBuffer_Class = env->FindClass("java/lang/StringBuffer");

    //取得建構值方法 <init>是建構值固定寫法紅字不用理他
    // (傳入的參數簽名)這裡剛好沒有要傳入的參數
    // V這個是建構值固定回傳的簽名,這裡不是回傳Ljava/lang/StringBuffer;喔 這裡很容易CallVoidMethod的V搞混
    jmethodID StringBuffer_init = env->GetMethodID(StringBuffer_Class, "<init>", "()V");

    //取得StringBuffer物件 由於沒有要傳入參數所以後面就不用寫了
    jobject stringBuffer = env->NewObject(StringBuffer_Class, StringBuffer_init);

    //取得append方法
    //I=int Ljava/lang/StringBuffer;=StringBuffer
    //這裡特別注意只要是object型別的一定要加L跟; 這個很多文章不講然後根本不知道
    jmethodID append = env->GetMethodID(StringBuffer_Class, "append",
                                        "(I)Ljava/lang/StringBuffer;");

    //呼叫append方法
    //env->Call<Type>Method要用對對應的方法不然他會報noSuchMethod之類的錯誤
    //Call方法有非常多分類 自己可以env->看看
    stringBuffer = env->CallObjectMethod(stringBuffer, append, 9453);

    //多寫一個toString的方法 Java的字串在Jni裡面算是object
    //如果你少加一個分號或L就會這個錯JNI DETECTED ERROR IN APPLICATION:
    // JNI CallObjectMethodV called with pending exception java.lang.NoSuchMethodError:
    // no non-static method "Ljava/lang/StringBuffer;.toString()Ljava/lang/String"
    //就是這麼白痴然後一堆文章不打清楚
    jmethodID toString = env->GetMethodID(StringBuffer_Class, "toString", "()Ljava/lang/String;");
    //這裡是強制轉型
    jstring str = (jstring) env->CallObjectMethod(stringBuffer, toString);

    //這個是JNI預設的轉型方法jstring to const char*
    const char *c = env->GetStringUTFChars(str, JNI_FALSE);

    //這個%s不吃jstring
    LOGD("str:%s", c);

    //這個是釋放資源,因為JNI不會自動釋放資源,除了回傳的那個JAVA會自己GC
    //這個還滿複雜的建議google補充
    //通常是class和object是DeleteLocalRef
    //而Get通常會有對應的Release
    env->DeleteLocalRef(StringBuffer_Class);


    return stringBuffer;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getIMEI(JNIEnv *env, jclass clazz, jobject context) {
    //  取得IMEI = deviceID
    //  String id = Settings.Secure.getString(MainActivity.this.getContentResolver(), Settings.Secure.ANDROID_ID);


    //這裡特別注意Secure是Settings裡面的Class所以是用$來連接而不是/
    jclass Secure_Class = env->FindClass("android/provider/Settings$Secure");

    //要傳入ContentResolver和String 回傳String ;不是分隔號是因為object所以一定要加
    //如果傳入是int跟string回傳是double 那就會是 "(ILjava/lang/String;)D"
    //如果傳入是string跟int回傳是double 那就會是 "(Ljava/lang/String;I)D"傳入的順序不能有錯
    jmethodID getString = env->GetStaticMethodID(Secure_Class, "getString",
                                                 "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");

    //取得Class的另一種方法
    jclass context_class = env->GetObjectClass(context);
    //沒有傳入參數 回傳ContentResolver
    jmethodID getContentResolver = env->GetMethodID(context_class, "getContentResolver",
                                                    "()Landroid/content/ContentResolver;");

    jobject contentResolver = env->CallObjectMethod(context, getContentResolver);

    //這個是new一個jstring的方法 還有其他方法可以去google找找
    //這裡算是偷懶 理論上要用jFieldID取得 這裡我是直接回傳這個值
    jstring android_id = env->NewStringUTF("android_id");

    //用jFieldID取得 最後"Ljava/lang/String;"＝要回傳的型別
    //jfieldID android_id=env->GetStaticFieldID(Secure_Class,"ANDROID_ID","Ljava/lang/String;");
    //jstring id=(jstring)env->GetStaticObjectField(Secure_Class,android_id);

    //getString是靜態方法所以是CallStaticObjectMethod 傳入contentResolver和android_id對應著上面的getString
    jstring IMEI = (jstring) env->CallStaticObjectMethod(Secure_Class, getString, contentResolver,
                                                         android_id);

    env->DeleteLocalRef(Secure_Class);
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(contentResolver);


    return IMEI;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getDeviceName(JNIEnv *env, jclass clazz) {

//    JAVA
//    String manufacturer = Build.MANUFACTURER;
//    String model = Build.MODEL;
//    String str = manufacturer+" "+model;

    //取得Class
    jclass build = env->FindClass("android/os/Build");

    //取得靜態常數fieldID
    jfieldID MANUFACTURER_fieldID = env->GetStaticFieldID(build, "MANUFACTURER",
                                                          "Ljava/lang/String;");
    //取得靜態常數並且將jobject轉型成jstring
    jstring MANUFACTURER = (jstring) env->GetStaticObjectField(build, MANUFACTURER_fieldID);
    //LOGD("MANUFACTURER : %s", env->GetStringUTFChars(MANUFACTURER, 0));

    jfieldID MODEL_fieldID = env->GetStaticFieldID(build, "MODEL", "Ljava/lang/String;");
    jstring MODEL = (jstring) env->GetStaticObjectField(build, MODEL_fieldID);
    //LOGD("MODEL : %s", env->GetStringUTFChars(MODEL, 0));

    //偷懶的方法直接取得String的Class
    jclass string_class = env->GetObjectClass(MODEL);
    //String的startWith方法
    jmethodID startWith = env->GetMethodID(string_class, "startsWith", "(Ljava/lang/String;)Z");
    //這裡特別注意JAVA呼叫起來會是這樣,MODEL.startWith(MANUFACTURER);
    //第一個是傳入MODEL,而不是傳入class喔
    //由於startWith是回傳boolean所以你要使用對應Call的方法,否則會拋出例外
    jboolean result = env->CallBooleanMethod(MODEL, startWith, MANUFACTURER);
    //LOGD("result:%d", result);

    env->DeleteLocalRef(build);
    env->DeleteLocalRef(string_class);

    //如果要使用C++的字串,記得要導入<string>和宣告std
    //字串拼接有幾種方式
    //1.用C++的string,但是拼接時字串的型別要是char*或是const char*
    //2.char*或是const char*後面直接+好就可
    //這裡的jstring並不能向JAVA依樣直接用+號
    std::string deviceName = "";
    deviceName =
            deviceName + " " + Jstring2CStr(env, MODEL) + " " + Jstring2CStr(env, MANUFACTURER);

    return charToJstring(env, deviceName.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getVersionName(JNIEnv *env, jclass clazz, jobject context) {
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

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getKeySha1(JNIEnv *env, jclass clazz, jobject context_object) {
    //這個是取得app的sha-1 Key,網路上分享看到的程式碼
    //剛好這個有用到byteArray之類的可以介紹
    const char hexcode2[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D',
                             'E', 'F'};

    jclass context_class = env->GetObjectClass(context_object);

    //Java Reflection to get PackageManager
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                          "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context_object, methodId);
    if (package_manager == NULL) {
        LOGD("package_manager is NULL!!!");
        return NULL;
    }

    //Java Reflection to get package name
    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring) env->CallObjectMethod(context_object, methodId);
    if (package_name == NULL) {
        LOGD("package_name is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(context_class);

    //Java Reflection to get PackageInfo
    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getPackageInfo",
                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    env->DeleteLocalRef(pack_manager_class);
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name, 0x40);
    if (package_info == NULL) {
        LOGD("getPackageInfo() is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(package_manager);

    //Get signature information
    jclass package_info_class = env->GetObjectClass(package_info);
    //這裡 [ 代表他是一個Array陣列的型別
    jfieldID fieldId = env->GetFieldID(package_info_class, "signatures",
                                       "[Landroid/content/pm/Signature;");
    env->DeleteLocalRef(package_info_class);
    //這裡可以看到轉型成jobjectArray
    jobjectArray signature_object_array = (jobjectArray) env->GetObjectField(package_info, fieldId);
    if (signature_object_array == NULL) {
        LOGD("signature is NULL!!!");
        return NULL;
    }

    //好像只有jobject的array可以用GetObjectArrayElement來取得元素,這個等價於signature_object_array[0]
    //通常如果是jbyteArray[0]你會取得的型別是_jbyteArray這個非常神奇的型別
    //所以你要先轉型成jbyte* env->GetByteArrayElements(jbyteArray,JNI_FALSE or JNI_TRUE) true或false自行google差異
    //當你轉型成jbyte*時就可以直接拿元素了 jbyte*[index]
    //以上的型別可以替換成Boolean int...之類的
    jobject signature_object = env->GetObjectArrayElement(signature_object_array, 0);
    env->DeleteLocalRef(package_info);

    //Convert signature information to Sha1
    jclass signature_class = env->GetObjectClass(signature_object);
    methodId = env->GetMethodID(signature_class, "toByteArray", "()[B");
    env->DeleteLocalRef(signature_class);
    jbyteArray signature_byte = (jbyteArray) env->CallObjectMethod(signature_object, methodId);
    jclass byte_array_input_class = env->FindClass("java/io/ByteArrayInputStream");
    methodId = env->GetMethodID(byte_array_input_class, "<init>", "([B)V");
    jobject byte_array_input = env->NewObject(byte_array_input_class, methodId, signature_byte);
    jclass certificate_factory_class = env->FindClass("java/security/cert/CertificateFactory");
    methodId = env->GetStaticMethodID(certificate_factory_class, "getInstance",
                                      "(Ljava/lang/String;)Ljava/security/cert/CertificateFactory;");
    jstring x_509_jstring = env->NewStringUTF("X.509");
    jobject cert_factory = env->CallStaticObjectMethod(certificate_factory_class, methodId,
                                                       x_509_jstring);
    methodId = env->GetMethodID(certificate_factory_class, "generateCertificate",
                                ("(Ljava/io/InputStream;)Ljava/security/cert/Certificate;"));
    jobject x509_cert = env->CallObjectMethod(cert_factory, methodId, byte_array_input);
    env->DeleteLocalRef(certificate_factory_class);
    jclass x509_cert_class = env->GetObjectClass(x509_cert);
    methodId = env->GetMethodID(x509_cert_class, "getEncoded", "()[B");
    jbyteArray cert_byte = (jbyteArray) env->CallObjectMethod(x509_cert, methodId);
    env->DeleteLocalRef(x509_cert_class);
    jclass message_digest_class = env->FindClass("java/security/MessageDigest");
    methodId = env->GetStaticMethodID(message_digest_class, "getInstance",
                                      "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    jstring sha1_jstring = env->NewStringUTF("SHA1");
    jobject sha1_digest = env->CallStaticObjectMethod(message_digest_class, methodId, sha1_jstring);
    methodId = env->GetMethodID(message_digest_class, "digest", "([B)[B");
    jbyteArray sha1_byte = (jbyteArray) env->CallObjectMethod(sha1_digest, methodId, cert_byte);
    env->DeleteLocalRef(message_digest_class);

    //Convert to char
    jsize array_size = env->GetArrayLength(sha1_byte);
    jbyte *sha1 = env->GetByteArrayElements(sha1_byte, NULL);
    char *hex_sha = new char[array_size * 2 + 1];
    for (int i = 0; i < array_size; ++i) {
        hex_sha[2 * i] = hexcode2[((unsigned char) sha1[i]) / 16];
        hex_sha[2 * i + 1] = hexcode2[((unsigned char) sha1[i]) % 16];
    }
    hex_sha[array_size * 2] = '\0';

    return env->NewStringUTF(hex_sha);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_Jni_getVersionCode(JNIEnv *env, jclass clazz, jobject context) {
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
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_versionCypto(JNIEnv *env, jclass clazz, jobject context) {
    int number = 0;
    jstring appName = env->NewStringUTF("AvNight");
    jstring versionName = getVersionName(env, context);
    jint versionCode = getVersionCode(env, context);

    jclass String_Class = env->FindClass("java/lang/String");
    jmethodID charAt = env->GetMethodID(String_Class, "charAt", "(I)C");


    int appNameLength = env->GetStringUTFLength(appName);
    for (int i = 0; i < appNameLength; i++) {
        char c = env->CallCharMethod(appName, charAt, i);
        number += (int) c;
    }

    int versionLength = env->GetStringUTFLength(versionName);
    for (int i = 0; i < versionLength; i++) {
        char c = env->CallCharMethod(versionName, charAt, i);
        number += (int) c;
    }

    number += versionCode;
    jclass Int_Class = env->FindClass("java/lang/Integer");
    jmethodID toHexString = env->GetStaticMethodID(Int_Class, "toHexString",
                                                   "(I)Ljava/lang/String;");

    jstring str = (jstring) env->CallStaticObjectMethod(Int_Class, toHexString, number);

    env->DeleteLocalRef(String_Class);
    env->DeleteLocalRef(Int_Class);

    return str;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_versionCypto2(JNIEnv *env, jclass clazz, jobject context) {

    jstring versionName = getVersionName(env, context);
    jint versionCode = getVersionCode(env, context);

    jclass StringBuilder_Class = env->FindClass("java/lang/StringBuilder");
    jmethodID init = env->GetMethodID(StringBuilder_Class, "<init>", "()V");
    jmethodID append = env->GetMethodID(StringBuilder_Class, "append",
                                        "(I)Ljava/lang/StringBuilder;");
    jmethodID toString = env->GetMethodID(StringBuilder_Class, "toString",
                                          "()Ljava/lang/String;");

    jobject stringBuilder = env->NewObject(StringBuilder_Class, init);

    jclass String_Class = env->FindClass("java/lang/String");
    jmethodID charAt = env->GetMethodID(String_Class, "charAt", "(I)C");

    int versionLength = env->GetStringUTFLength(versionName);
    for (int i = 0; i < versionLength; i++) {
        char c = env->CallCharMethod(versionName, charAt, i);
        if (c != '.') {
            stringBuilder = env->CallObjectMethod(stringBuilder, append, (int) c);
        }
    }
    stringBuilder = env->CallObjectMethod(stringBuilder, append, versionCode);


    jclass Int_Class = env->FindClass("java/lang/Integer");
    jmethodID toHexString = env->GetStaticMethodID(Int_Class, "toHexString",
                                                   "(I)Ljava/lang/String;");
    jmethodID parseInt = env->GetStaticMethodID(Int_Class, "parseInt", "(Ljava/lang/String;)I");

    jstring j = (jstring) env->CallObjectMethod(stringBuilder, toString);
    int number = env->CallStaticIntMethod(Int_Class, parseInt, j);
    jstring hex = (jstring) env->CallStaticObjectMethod(Int_Class, toHexString, number);

    env->DeleteLocalRef(StringBuilder_Class);
    env->DeleteLocalRef(String_Class);
    env->DeleteLocalRef(Int_Class);
    env->DeleteLocalRef(stringBuilder);

    return hex;
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_DexSize(JNIEnv *env, jclass clazz, jobject context) {

    jclass context_class = env->GetObjectClass(context);
    jmethodID getPackageCodePath = env->GetMethodID(context_class, "getPackageCodePath",
                                                    "()Ljava/lang/String;");
    jstring packageCodePath = (jstring) env->CallObjectMethod(context, getPackageCodePath);

    jclass zipFile_class = env->FindClass("java/util/zip/ZipFile");
    jmethodID zipFile_init = env->GetMethodID(zipFile_class, "<init>",
                                              "(Ljava/lang/String;)V");   //V=Void 建構值是Ｖ
    jobject zipFile = env->NewObject(zipFile_class, zipFile_init,
                                     packageCodePath);                //對應要用NewObject

    jmethodID getEntry = env->GetMethodID(zipFile_class, "getEntry",
                                          "(Ljava/lang/String;)Ljava/util/zip/ZipEntry;");  //有Ｌ結尾就要加;號 L=Object
    jstring name = env->NewStringUTF("classes.dex");
    jobject zipEntry = env->CallObjectMethod(zipFile, getEntry, name);

    jclass zipEntry_class = env->GetObjectClass(zipEntry);
    jmethodID getSize = env->GetMethodID(zipEntry_class, "getSize", "()J");
    jlong size = env->CallLongMethod(zipEntry, getSize);
    //LOGD("getSize:%ld",size);

    jclass long_class = env->FindClass("java/lang/Long");
    jmethodID toHexString = env->GetStaticMethodID(long_class, "toHexString",
                                                   "(J)Ljava/lang/String;");
    jstring sizeToHex = (jstring) env->CallStaticObjectMethod(long_class, toHexString, size);
    //LOGD("getSizeToHex=%s", Jstring2CStr(env, sizeToHex));


    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(zipFile_class);
    env->DeleteLocalRef(zipFile);
    env->DeleteLocalRef(zipEntry);
    env->DeleteLocalRef(zipEntry_class);
    env->DeleteLocalRef(long_class);

    return sizeToHex;

}


extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {


    JNIEnv *env;

    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }


    return JNI_VERSION_1_6;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_Jni_isDebug(JNIEnv *env, jclass clazz, jobject application) {


    jclass ApplicationInfo_Class = env->FindClass("android/content/pm/ApplicationInfo");
    jfieldID FLAG_DEBUGGABLE = env->GetStaticFieldID(ApplicationInfo_Class, "FLAG_DEBUGGABLE", "I");
    int d = env->GetStaticIntField(ApplicationInfo_Class, FLAG_DEBUGGABLE);
    LOGD("FLAG_DEBUGGABLE=%d", d);

    jclass context_Class = env->FindClass("android/content/ContextWrapper");
    jmethodID getApplicationInfo = env->GetMethodID(context_Class, "getApplicationInfo",
                                                    "()Landroid/content/pm/ApplicationInfo;");
    jobject applicationInfo = env->CallObjectMethod(application, getApplicationInfo);

    jfieldID flags = env->GetFieldID(ApplicationInfo_Class, "flags", "I");
    int f = env->GetIntField(applicationInfo, flags);
    LOGD("FLAG=%d", f);

    jboolean isDebug = ((f & d) != 0);

    return isDebug;

}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnidemo_Jni_firebaseNDKCrash(JNIEnv *env, jclass clazz) {

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_Jni_getApplication(JNIEnv *env, jclass clazz, jobject context) {
    jclass context_class = env->GetObjectClass(context);
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                          "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context, methodId);   //取得PackageManager

    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring) env->CallObjectMethod(context, methodId);   //取得PackageName

    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getApplicationInfo",
                                "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name,
                                                 0);  //取得PackageInfo

    jclass package_info_class = env->GetObjectClass(package_info);
    jfieldID jfieldId = env->GetFieldID(package_info_class, "className", "Ljava/lang/String;");
    jstring className = (jstring) env->GetObjectField(package_info, jfieldId);

    LOGD("%s", Jstring2CStr(env, className));
    jboolean app =
            strcmp(Jstring2CStr(env, className), "com.example.jnidemo.JniDemoApplication") == 0;

//    env->DeleteLocalRef(context_class);
//    env->DeleteLocalRef(package_manager);
//    env->DeleteLocalRef(package_info_class);
//    env->DeleteLocalRef(package_info);


    return app;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnidemo_Jni_throwExcetion(JNIEnv *env, jclass clazz) {
    jclass newExcCls = env->FindClass("java/lang/IllegalArgumentException");
    env->ThrowNew(newExcCls, "123");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getHost(JNIEnv *env, jclass clazz) {
    //取得Class
    jclass apiServiceManager_class = env->FindClass("com/example/jnidemo/ApiServiceManager");

    //取得靜態變數INSTANCE
    jfieldID INSTANCE_ID = env->GetStaticFieldID(apiServiceManager_class, "INSTANCE",
                                                 "Lcom/example/jnidemo/ApiServiceManager;");

    //取得靜態變數實體
    jobject INSTANCE = env->GetStaticObjectField(apiServiceManager_class, INSTANCE_ID);

    //取得方法
    jmethodID getHostMethID = env->GetMethodID(apiServiceManager_class, "getHOST",
                                               "()Ljava/lang/String;");

    //呼叫方法
    jstring host = (jstring) env->CallObjectMethod(INSTANCE, getHostMethID);

    //釋放
    env->DeleteLocalRef(apiServiceManager_class);
    env->DeleteLocalRef(INSTANCE);

    return host;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_Jni_getModel(JNIEnv *env, jclass clazz) {

    jclass modelClass = env->FindClass("com/example/jnidemo/Model");
    jfieldID jfieldId = env->GetStaticFieldID(modelClass, "model", "Ljava/lang/String;");
    jstring str = (jstring) env->GetStaticObjectField(modelClass, jfieldId);

    return str;
}


static array<string, 9> virtualPkgs = {
        "com.bly.dkplat",
        "dkplugin.pke.nnp",
        "com.by.chaos",
        "com.lbe.parallel",
        "com.excelliance.dualaid",
        "com.lody.virtual",
        "com.qihoo.magic",
        "multi.parallel.dualspace.cloner",
        "com.polar.apps.dual.multi.accounts"
};

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_Jni_isMultiApp(JNIEnv *env, jclass clazz, jobject context) {

    bool isMultiApp = false;

    jclass contextClass = env->GetObjectClass(context);
    jmethodID filesDir = env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
    jobject file = env->CallObjectMethod(context, filesDir);

    jclass fileClass = env->FindClass("java/io/File");
    jmethodID getPath = env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");

    jstring path = (jstring) env->CallObjectMethod(file, getPath);

    const char *cPath = env->GetStringUTFChars(path, NULL);

    if (cPath == NULL) {
        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(file);
        env->DeleteLocalRef(fileClass);
        return false;
    }

    for (int i = 0; i < virtualPkgs.size(); i++) {
        if (strstr(cPath, virtualPkgs[i].c_str()) != NULL) {
            isMultiApp = true;
            env->DeleteLocalRef(contextClass);
            env->DeleteLocalRef(file);
            env->DeleteLocalRef(fileClass);
            env->ReleaseStringUTFChars(path, cPath);
            return isMultiApp;
        }
    }

    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(file);
    env->DeleteLocalRef(fileClass);
    env->ReleaseStringUTFChars(path, cPath);
    return isMultiApp;
}

