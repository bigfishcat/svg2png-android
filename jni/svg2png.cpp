/* svg2png - Render an SVG image to a PNG image (using cairo)
 *
 * Copyright � 2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl Worth <cworth@isi.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define CAIRO_HAS_PNG_FUNCTIONS 1

#include "libpng/png.h"
#include "cairo/src/cairo.h"
#include "libsvg-cairo/svg-cairo.h"

#define MIN(a, b)     (((a) < (b)) ? (a) : (b))

static svg_cairo_status_t
render_to_png (FILE *svg_file, FILE *png_file, double scale, int width, int height);

static svg_cairo_status_t
svg_to_png (const char * svg_filename, const char * png_filename, double scale, int width, int height);

#include "com_etb_lab_svg2png_Svg2Png.h"
#include <android/log.h>
#include <jni.h>

/*
 * Class:     com_etb_lab_svg2png_Svg2Png
 * Method:    renderSVG
 * Signature: (Ljava/lang/String;Ljava/lang/String;DII)I
 */
JNIEXPORT jint JNICALL Java_com_etb_1lab_svg2png_Svg2Png_renderSVG
  (JNIEnv *env, jclass clazz, jstring svgFileName, jstring pngFileName, jdouble scale, jint width, jint height)
{

    const char *svgFile = env->GetStringUTFChars(svgFileName, 0);
    const char *pngFile = env->GetStringUTFChars(pngFileName, 0);

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "Java_com_etb_1lab_svg2png_Svg2Png_renderSVG %s => %s", svgFile, pngFile);
    jint result = svg_to_png(svgFile, pngFile, scale, width, height);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "Java_com_etb_1lab_svg2png_Svg2Png_renderSVG %s => %s", svgFile, pngFile);

    env->ReleaseStringUTFChars(svgFileName, svgFile);
    env->ReleaseStringUTFChars(pngFileName, pngFile);

    return result;
}

static svg_cairo_status_t
svg_to_png (const char * svg_filename, const char * png_filename, double scale, int width, int height)
{
    FILE *svg_file, *png_file;
    svg_cairo_status_t status;
    svg_file = fopen(svg_filename, "r");
    if (svg_file == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "svg2png", "svg_to_png:  failed to open %s: %s\n",
            svg_filename, strerror(errno));
        return SVG_CAIRO_STATUS_FILE_NOT_FOUND;
    }

    png_file = fopen(png_filename, "w");
    if (png_file == NULL) 
    {
        __android_log_print(ANDROID_LOG_ERROR, "svg2png", "svg_to_png:  failed to open %s: %s\n",
            png_filename, strerror(errno));
        return SVG_CAIRO_STATUS_FILE_NOT_FOUND;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "svg_to_png %s => %s", svg_filename, png_filename);
    status = render_to_png(svg_file, png_file, scale, width, height);
    if (status) 
    {
        __android_log_print(ANDROID_LOG_ERROR, "svg2png", "svg_to_png:  failed to render %s\n",
                svg_filename);
        return status;
    }

    fclose(svg_file);
    fclose(png_file);

    return SVG_CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
write_callback (void *closure, const unsigned char *data, unsigned int length)
{
    size_t written;
    FILE *file = (FILE *)closure;

    written = fwrite (data, 1, length, file);

    if (written == length)
	return CAIRO_STATUS_SUCCESS;
    else
	return CAIRO_STATUS_WRITE_ERROR;
}

static svg_cairo_status_t
write_surface_to_png_file (cairo_surface_t *surface, FILE *file)
{
    cairo_status_t status;

    status = cairo_surface_write_to_png_stream (surface, write_callback, file);

    if (status)
	return SVG_CAIRO_STATUS_IO_ERROR;
    else
	return SVG_CAIRO_STATUS_SUCCESS;
}

static svg_cairo_status_t
render_to_png (FILE *svg_file, FILE *png_file, double scale, int width, int height)
{
    unsigned int svg_width, svg_height;

    svg_cairo_status_t status;
    cairo_t *cr;
    svg_cairo_t *svgc;
    cairo_surface_t *surface;
    double dx = 0, dy = 0;

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: svg_cairo_create\n");
    status = svg_cairo_create (&svgc);
    if (status)
    {
        __android_log_print(ANDROID_LOG_ERROR, "svg2png", "render_to_png: Failed to create svg_cairo_t. Exiting.\n");
	    return status;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: svg_cairo_parse_file\n");
    status = svg_cairo_parse_file (svgc, svg_file);
    if (status)
	    return status;

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: svg_cairo_get_size\n");
    svg_cairo_get_size (svgc, &svg_width, &svg_height);

    if (width < 0 && height < 0)
    {
	    width = (svg_width * scale + 0.5);
	    height = (svg_height * scale + 0.5);
    }
    else if (width < 0)
    {
	    scale = (double) height / (double) svg_height;
	    width = (svg_width * scale + 0.5);
    }
    else if (height < 0) {
	    scale = (double) width / (double) svg_width;
	    height = (svg_height * scale + 0.5);
    }
    else
    {
        scale = MIN ((double) width / (double) svg_width, (double) height / (double) svg_height);
        /* Center the resulting image */
        dx = (width - (int) (svg_width * scale + 0.5)) / 2;
        dy = (height - (int) (svg_height * scale + 0.5)) / 2;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_image_surface_create with width:[%d] and height:[%d]\n", width, height);
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_create\n");
    cr = cairo_create (surface);

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_save\n");
    cairo_save (cr);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_set_operator\n");
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_paint\n");
    cairo_paint (cr);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_restore\n");
    cairo_restore (cr);

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_translate by dx:[%.0f] and dy:[%.0f]\n", (float)dx, (float)dy);
    cairo_translate (cr, dx, dy);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_scale by factor:[%.00f]\n", (float)scale);
    cairo_scale (cr, scale, scale);

    /* XXX: This probably doesn't need to be here (eventually) */
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_set_source_rgb\n");
    cairo_set_source_rgb (cr, 1, 1, 1);

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: svg_cairo_render\n");
    status = svg_cairo_render (svgc, cr);

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: write_surface_to_png_file\n");
    status = write_surface_to_png_file (surface, png_file);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_surface_destroy\n");
    cairo_surface_destroy (surface);
    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: cairo_destroy\n");
    cairo_destroy (cr);
    
    if (status)
	    return status;

    __android_log_print(ANDROID_LOG_DEBUG, "svg2png", "render_to_png: svg_cairo_destroy\n");
    svg_cairo_destroy (svgc);

    return status;
}
