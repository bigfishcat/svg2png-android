
# libjpeg stuff
LIBJPEG_SOURCES = libjpeg/jaricom.c libjpeg/jcapimin.c libjpeg/jcapistd.c libjpeg/jcarith.c \
	libjpeg/jccoefct.c libjpeg/jccolor.c \
        libjpeg/jcdctmgr.c libjpeg/jchuff.c libjpeg/jcinit.c libjpeg/jcmainct.c \
	libjpeg/jcmarker.c libjpeg/jcmaster.c \
        libjpeg/jcomapi.c libjpeg/jcparam.c libjpeg/jcprepct.c libjpeg/jcsample.c \
	libjpeg/jctrans.c libjpeg/jdapimin.c \
        libjpeg/jdapistd.c libjpeg/jdarith.c libjpeg/jdatadst.c libjpeg/jdatasrc.c \
	libjpeg/jdcoefct.c libjpeg/jdcolor.c \
        libjpeg/jddctmgr.c libjpeg/jdhuff.c libjpeg/jdinput.c libjpeg/jdmainct.c \
	libjpeg/jdmarker.c libjpeg/jdmaster.c \
        libjpeg/jdmerge.c libjpeg/jdpostct.c libjpeg/jdsample.c libjpeg/jdtrans.c \
	libjpeg/jerror.c libjpeg/jfdctflt.c \
        libjpeg/jfdctfst.c libjpeg/jfdctint.c libjpeg/jidctflt.c libjpeg/jidctfst.c \
	libjpeg/jidctint.c libjpeg/jquant1.c \
        libjpeg/jquant2.c libjpeg/jutils.c libjpeg/jmemmgr.c libjpeg/jmemnobs.c

# zlib stuff
ZLIB_SOURCES = zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c zlib/gzclose.c zlib/gzlib.c zlib/gzread.c \
        zlib/gzwrite.c zlib/infback.c zlib/inffast.c zlib/inflate.c zlib/inftrees.c zlib/trees.c zlib/uncompr.c zlib/zutil.c

# libpng stuff
LIBPNG_SOURCES = libpng/png.c libpng/pngset.c libpng/pngget.c libpng/pngrutil.c \
        libpng/pngtrans.c libpng/pngwutil.c \
        libpng/pngread.c libpng/pngrio.c libpng/pngwio.c libpng/pngwrite.c libpng/pngrtran.c \
        libpng/pngwtran.c libpng/pngmem.c libpng/pngerror.c libpng/pngpread.c

# libexpat stuff
LIBEXPAT_SOURCES = libexpat/expat/xmlparse.c libexpat/expat/xmlrole.c libexpat/expat/xmltok.c libexpat/expat/xmltok_impl.c libexpat/expat/xmltok_ns.c


# libsvg stuff
LIBSVG_EXTRA_SOURCES = libsvg/svg_parser_expat.c libsvg/strhmap_cc.cc

LIBSVG_SOURCES = \
	libsvg/svg.c \
	libsvg/svg_ascii.c \
	libsvg/svg_attribute.c \
	libsvg/svg_color.c \
	libsvg/svg_element.c \
	libsvg/svg_gradient.c \
	libsvg/svg_group.c \
	libsvg/svg_length.c \
	libsvg/svg_paint.c \
	libsvg/svg_parser.c \
	libsvg/svg_pattern.c \
	libsvg/svg_image.c \
	libsvg/svg_path.c \
	libsvg/svg_str.c \
	libsvg/svg_style.c \
	libsvg/svg_text.c \
	libsvg/svg_transform.c \
	$(LIBSVG_EXTRA_SOURCES)

include $(CLEAR_VARS)
# package it
LOCAL_CPP_EXTENSION := .cc

LOCAL_MODULE    := libsvg
LOCAL_CFLAGS += -DLIBSVG_EXPAT -DCONFIG_DIR=\"/\" -Ijni/libexpat/ -Ijni/libexpat/expat/ -Ijni/libpng/ -Ijni/libjpeg/ -Ijni/libsvg/ -DHAVE_CONFIG_H -DHAVE_EXPAT_CONFIG_H -Wall

LOCAL_SRC_FILES := \
$(LIBJPEG_SOURCES) $(LIBPNG_SOURCES) $(ZLIB_SOURCES) $(LIBEXPAT_SOURCES) $(LIBSVG_SOURCES)

include $(BUILD_STATIC_LIBRARY)