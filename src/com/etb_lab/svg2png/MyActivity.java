package com.etb_lab.svg2png;

import android.app.Activity;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.Display;
import android.widget.ImageView;

import java.io.*;

public class MyActivity extends Activity {
    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        ImageView image = (ImageView) findViewById(R.id.image);
        try {
            String src = saveImageToData();
            String dst = src + ".png";

            Display display = getWindowManager().getDefaultDisplay();
            int width = display.getWidth();
            int height = display.getHeight();

            int result = Svg2Png.renderSVG(src, dst, 1, width, height);
            if (result == 0)
                image.setImageBitmap(BitmapFactory.decodeFile(dst));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private String saveImageToData() throws IOException {
        File dataDir = getExternalFilesDir(null);
        if (dataDir == null) {
            dataDir = getFilesDir();
        }

        File of = new File(dataDir, "image.svg");
        if (!of.exists()) {
            InputStream is = getResources().openRawResource(R.raw.image);
            OutputStream os = new FileOutputStream(of);
            copyFile(is, os);
        }

        return of.getAbsolutePath();
    }

    public void copyFile(InputStream in, OutputStream out) throws IOException {
        // Transfer bytes from in to out
        byte[] buf = new byte[1024];
        int len;
        while ((len = in.read(buf)) > 0) {
            out.write(buf, 0, len);
        }
        in.close();
        out.flush();
        out.close();
    }
}
