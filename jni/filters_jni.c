#include "filters_jni.h"
#include "filters/filters.h"


void Java_com_renren_openGLSkinBeauty_JNILibCurve_gg( JNIEnv* env, jobject obj,jfloatArray sCurve,jfloat control_X,jfloat control_Y) {
	float *curve;
	curve = (*env)->GetFloatArrayElements(env, sCurve, 0);
	getSpline(curve, control_X, control_Y);
}

