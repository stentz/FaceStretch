#include "util.h"
#include "imagewarper/GrowCanvas.h"
#include "imagewarper/ManagedWrapper.h"
#include "imagewarper/TranslateCanvas.h"
#include "imagewarper/Warper.h"
#include "imagewarper/WarperCanvas.h"
#include "imagewarper/WarperUtil.h"

using namespace ImageWarper;
ImageData imgData;
ImageData *tempData;
Warper *warper;
WarpedImage* warpedImg;
int width;
int height;

extern "C" {

void JNIFUNCF(JNILibCurve, initWarp, jobject bitmap, jint w, jint h) {


    width = w;
    height = h;
	imgData.Resize(width, height, 4);
	unsigned char* destination = 0;
	AndroidBitmap_lockPixels(env, bitmap, (void**) &destination);
	memcpy(imgData.Data, destination, imgData.ScanWidth * height);
	warper = new Warper(imgData);
//	warper->BeginWarp(pt, 150, WARPER_TRANSLATE);
//
//	for(int i=0; i <1; i++) {
//		warpedImg = warper->UpdateWarp(pt2);
//	}
//	warpedImg = warper->EndWarp(warpedImg);
//
//	tempData =  warper->m_imgOriginal;
	AndroidBitmap_unlockPixels(env, bitmap);
//	jbyte *byte = (jbyte*)tempData->Data;
//	jbyteArray rgbBuf = env->NewByteArray(width*height*4);
//	env->SetByteArrayRegion(rgbBuf, 0, width*height*4, byte);
	//释放
//	(*env)->ReleaseByteArrayElements(env,yuvBuf,arr,0);

//	return rgbBuf;

//	__android_log_print(ANDROID_LOG_FATAL, "wrap", "NativeFilters %d  %d  %d", warpedImg->Image.Width, warpedImg->Image.Height, width);
//	__android_log_print(ANDROID_LOG_FATAL, "LOG", "NativeFilters %d  %d  %d", imgData.ScanWidth, height, width);
}

void JNIFUNCF(JNILibCurve, beginWarp, int type, int radius,int x, int y) {
	ImageWarper::Point currentPoint(x, y);
	__android_log_print(ANDROID_LOG_FATAL, "beginWrap", "NativeFilters %d  %d", x, y);
	int warp_type = 0;
	switch(type) {
	case 0:
		warp_type = WARPER_TRANSLATE;
		break;
	case 1:
		warp_type = WARPER_GROW;
		break;
	case 2:
		warp_type = WARPER_SHRINK;
		break;
	}
	warper->BeginWarp(currentPoint, radius, warp_type);

//	__android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", "begin");
}

void JNIFUNCF(JNILibCurve, updateWarp, int x, int y) {
	ImageWarper::Point currentPoint2(x, y);
//	__android_log_print(ANDROID_LOG_FATAL, "updateWrap", "NativeFilters %d  %d", x, y);
	warpedImg = warper->UpdateWarp(currentPoint2);

//	__android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", "update");
}

jbyteArray JNIFUNCF(JNILibCurve, endWarp, int x, int y) {
	ImageWarper::Point currentPoint3(x, y);

	warpedImg = warper->EndWarp(warpedImg);

	tempData =  warper->m_imgOriginal;
	jbyte *byte = (jbyte*) tempData->Data;
	jbyteArray rgbBuf = env->NewByteArray(width * height * 4);
	env->SetByteArrayRegion(rgbBuf, 0, width * height * 4, byte);

	return rgbBuf;

//	__android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", "end");
}

}
