package com.etb_lab.svg2png;

public class Svg2Png {
    static {
        System.loadLibrary("svg2png");
    }

    public native static int renderSVG(String svgFileName, String pngFileName, double scale, int width, int height);
}
