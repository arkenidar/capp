package com.capp.app;

import org.libsdl.app.SDLActivity;
import android.os.Bundle;

public class MainActivity extends SDLActivity {

    static {
        System.loadLibrary("capp_native");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Copy app.lua from assets to files directory for SDL_main to find
        try {
            String appLuaContent = new String(readAsset("app.lua"));
            java.io.File appLuaFile = new java.io.File(getFilesDir(), "app.lua");
            java.io.FileOutputStream fos = new java.io.FileOutputStream(appLuaFile);
            fos.write(appLuaContent.getBytes());
            fos.close();

            android.util.Log.i("capp", "app.lua copied to: " + appLuaFile.getAbsolutePath());
        } catch (Exception e) {
            android.util.Log.e("capp", "Failed to copy app.lua: " + e.getMessage());
        }

        super.onCreate(savedInstanceState);
    }

    private byte[] readAsset(String filename) throws java.io.IOException {
        java.io.InputStream is = getAssets().open(filename);
        int size = is.available();
        byte[] buffer = new byte[size];
        is.read(buffer);
        is.close();
        return buffer;
    }
}
