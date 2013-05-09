#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/bitmap.h>

#define LOG(msg...) __android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", msg)


#define JNIFUNCF(cls, name, vars...) Java_com_hito_face_stretch_ ## cls ## _ ## name(JNIEnv* env, jobject obj, vars)

//extern "C" {}

//void JNIFUNCF(NativeWarp, initWarp, jobject bitmap, jint w, jint h);
//namespace ImageWarper {
////
//ImageData imgData;
//Warper *warper;
//WarpedImage* warpedImg;
//ImageWarper::Point pt(200, 200);
//ImageWarper::Point pt2(250, 250);
//}
