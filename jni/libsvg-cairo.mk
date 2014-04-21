LIBSVG_CAIRO_SOURCES = \
	libsvg-cairo/svg_cairo.c \
	libsvg-cairo/svg-cairo.h \
	libsvg-cairo/svg-cairo-internal.h \
	libsvg-cairo/svg_cairo_sprintf_alloc.c \
	libsvg-cairo/svg_cairo_state.c


LIBSVG_CAIRO_CFLAGS:=                 \
    -DPACKAGE_VERSION="\"svg-cairo\"" \
    -DHAVE_STDINT_H                   \
    -DHAVE_UINT64_T                   \


include $(CLEAR_VARS)

LOCAL_MODULE    := libsvg-cairo
LOCAL_CFLAGS    := -O2 $(LIBSVG_CAIRO_CFLAGS) -Ijni/libsvg -Ijni/libsvg-cairo -Ijni/pixman/pixman -Ijni/cairo/src -Ijni/cairo-extra -Ijni/pixman-extra -Wno-missing-field-initializers -Wno-attributes
LOCAL_SRC_FILES := $(LIBSVG_CAIRO_SOURCES)
LOCAL_STATIC_LIBRARIES := libcairo libpixman libexpat libsvg

include $(BUILD_STATIC_LIBRARY)