LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := renrenFilter
LOCAL_SRC_FILES := filters/filter_curve.c\
				   filters_jni.c \
				   imagewarper/util.cpp \
				   imagewarper/GrowCanvas.cpp \
				   imagewarper/ManagedWrapper.cpp \
				   imagewarper/TranslateCanvas.cpp \
				   imagewarper/WarperCanvas.cpp \
				   imagewarper/WarperUtil.cpp \
				   imagewarper/Warper.cpp 
LOCAL_LDLIBS    := -llog -ljnigraphics
LOCAL_CFLAGS     = -Wno-psabi

include $(BUILD_SHARED_LIBRARY)