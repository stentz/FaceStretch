#ifndef FILTERS_H
#define FILTERS_H

#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <android/bitmap.h>

#define LOG(msg...) __android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", msg)

#define JNIFUNCF(cls, name, vars...) com_renren_openGLSkinBeauty_ ## cls ## _ ## name(JNIEnv* env, jobject obj, vars)

#endif // FILTERS_H
