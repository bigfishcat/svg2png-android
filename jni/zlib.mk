ZLIB_SOURCES = \
	zlib/adler32.c \
	zlib/compress.c \
	zlib/crc32.c \
	zlib/deflate.c \
	zlib/gzclose.c \
	zlib/gzlib.c \
	zlib/gzread.c \
    zlib/gzwrite.c \
    zlib/infback.c \
    zlib/inffast.c \
    zlib/inflate.c \
    zlib/inftrees.c \
    zlib/trees.c \
    zlib/uncompr.c \
    zlib/zutil.c

ZLIB_CFLAGS:=                     \
    -DPACKAGE_VERSION="\"libexpat\""  \
    -DHAVE_STDINT_H                   \
    -DHAVE_UINT64_T                   \


include $(CLEAR_VARS)

LOCAL_MODULE    := zlib
LOCAL_CFLAGS    := -O2 $(ZLIB_CFLAGS) -Ijni/zlib -Wno-missing-field-initializers -Wno-attributes
LOCAL_SRC_FILES := $(ZLIB_SOURCES)

include $(BUILD_STATIC_LIBRARY)