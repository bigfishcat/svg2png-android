LOCAL_PATH := $(call my-dir)

include jni/pixman.mk
include jni/cairo.mk
include jni/libsvg.mk
include jni/libsvg-cairo.mk
include $(CLEAR_VARS)

LOCAL_MODULE    := svg2png
LOCAL_CFLAGS    := -O2 --std=c99 -I. -Ijni/pixman/pixman -Ijni/cairo/src -Ijni/cairo-extra -Ijni/pixman-extra -Ijni/libsvg -Ijni/libpng -Ijni/libsvg-cairo -Wno-missing-field-initializers
LOCAL_LDLIBS    := -lm -llog -landroid
LOCAL_SRC_FILES := svg2png.c
LOCAL_STATIC_LIBRARIES := android_native_app_glue libcairo libpixman libexpat libsvg libsvg-cairo cpufeatures

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
$(call import-module,android/native_app_glue)
