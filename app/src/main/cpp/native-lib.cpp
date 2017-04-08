#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <pthread.h>

#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "NdkStudy", __VA_ARGS__)


static jmethodID gOnNativeMsg = NULL;
static JavaVM * gVM = NULL;
static jobject gObj = NULL;


jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOG("JNI_OnLoad called");
    gVM = jvm;
    return JNI_VERSION_1_4;
}


JNIEXPORT void JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_nativeGetDark(JNIEnv *env, jobject instance,
                                                           jobject bitmap, jint width,
                                                           jint height) {

    // TODO


}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_nativeInit(JNIEnv *env, jobject instance) {

    // TODO
    LOG("naive init");
    if(NULL == gObj)
    {
        gObj = env->NewGlobalRef(instance);
    }

    if(NULL == gOnNativeMsg)
    {
        jclass clazz = env->GetObjectClass(instance);
        gOnNativeMsg = env->GetMethodID(clazz, "onNativeMsg", "(Ljava/lang/String;)V");
        if(gOnNativeMsg == NULL)
        {
            jclass exclazz = env->FindClass("java/lang/RuntimeExcepion");
            env->ThrowNew(exclazz, "can not find method: onNativeMsg");
        }
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_nativeFree(JNIEnv *env, jobject instance) {

    // TODO
    if(NULL != gObj)
    {
        env->DeleteGlobalRef(gObj);
        gObj = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_nativeWorker(JNIEnv *env, jobject instance, jint id,
                                                          jint iterations) {

    // TODO
    for(int i = 0; i < iterations; ++i)
    {
        char msg[100];
        sprintf(msg, "this is the msg from nativeWorker for the java thread %d and the iterations %d", id, i);
        jstring smsg = env->NewStringUTF(msg);
        env->CallVoidMethod(instance, gOnNativeMsg, smsg); //传给java的字符串应该是jstring类型;
        if(NULL != env->ExceptionOccurred())  //检查是否发生了异常
        {
            break;
        }
        sleep(1);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


