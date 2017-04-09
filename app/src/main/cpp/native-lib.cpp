#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <pthread.h>
#include <android/bitmap.h>

#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "lyhNdkStudy", __VA_ARGS__)
#define DEBUG true

typedef struct PartDarkParam{
    u_char * rgba;
    int start; //表示的是以像素位的索引，而不是对于RGBA来说以char为单位的索引。
    int end;
    u_char * out;
};


static jmethodID gOnNativeMsg = NULL;
static JavaVM * gVM = NULL;
static jobject gObj = NULL;


jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOG("JNI_OnLoad called");
    gVM = jvm;
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_lisan_ndkstudy_MainActivity_nativeGetDark(JNIEnv *env, jobject instance,
                                                           jobject bitmap, jint width,
                                                           jint height) {

    // TODO
    LOG("nativeGetDark start");
    void * getPartDark(void * args);
    char * rgba1 = 0;
    u_char * out = (u_char *)malloc(sizeof(u_char) * width * height);
    int lr = AndroidBitmap_lockPixels(env, bitmap, (void **)&rgba1);
    u_char * rgba = (u_char *)rgba1;
    LOG("lockpixle   lr:%d   rgba:%d", lr, rgba);
    int unit = height >> 2;
    pthread_t *pthread = new pthread_t[5];
        PartDarkParam  *param = new PartDarkParam[5];
    for(int i = 0; i < 5; ++i)
    {
        param[i].rgba = rgba;
        param[i].out = out;
        param[i].start = i * width * unit;
        param[i].end = (i + 1) * width * unit;
        if(i == 4)
        {
            param[i].end = width * height;
        }
        LOG("pthread  i:%d, start:%d   end:%d", i, param[i].start, param[i].end);
        int cr = pthread_create(&(pthread[i]), NULL, getPartDark, &(param[i]));
    }
    for(int i = 0; i < 5; i++)
    {
        pthread_join(pthread[i], NULL);
    }
    LOG("nativeGetDark end");
    AndroidBitmap_unlockPixels(env, bitmap);

}

extern "C"
void * getPartDark(void * args)
{
    JNIEnv *env  = NULL;
    gVM->AttachCurrentThread(&env, NULL);

    PartDarkParam * param = (PartDarkParam *)args;
    u_char min;
    u_char * rgba = param->rgba;
    u_char * out = param->out;
    int end = param->end;
#if DEBUG
    LOG("getpartdart    start  start:%d  end:%d   jout:%d    rgba:%d",  param->start, param->end, param->out, param->rgba);
#endif
    for(int i = param->start, j = param->start * 4; i < end; ++i, j += 2)
    {
        min = rgba[j];
        if(min > rgba[++j]) min = rgba[j];
        if(min > rgba[++j]) min = rgba[j];
        out[i] = min;
    }
    gVM->DetachCurrentThread();
#if DEBUG
    LOG("getpartdark finish");
#endif
    return NULL;
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


