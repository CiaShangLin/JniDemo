#include <Jni.h>


//導入這三個就可以用Android的Log,CMakeLists.txt那裡也有導入一些東西要注意
//不過這裡的LOG輸出法是C的printf在輸入字串的時候不能用jstring要用char*
//希望有人可以提供更好用的,不然型別轉來轉去很累
#include <android/log.h>

#define TAG    "jni-log"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

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
    const char* c =env->GetStringUTFChars(str, JNI_FALSE);

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