#include <jni.h>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_capp_app_MainActivity_init(JNIEnv *env, jobject obj) {
    // Initialize native code here
}
