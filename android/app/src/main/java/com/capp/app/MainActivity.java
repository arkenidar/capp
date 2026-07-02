package com.capp.app;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("capp_native");
    }

    private native void initLua(String assetsPath);
    public native double runLuaFrame(long ticks_ms);
    private native void closeLua();

    private GameView gameView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Create game view (renders Lua output)
        gameView = new GameView(this);
        setContentView(gameView);

        // Copy app.lua from assets to files directory so native code can access it
        try {
            String appLuaContent = new String(readAsset("app.lua"));
            java.io.File appLuaFile = new java.io.File(getFilesDir(), "app.lua");
            java.io.FileOutputStream fos = new java.io.FileOutputStream(appLuaFile);
            fos.write(appLuaContent.getBytes());
            fos.close();

            String appLuaPath = appLuaFile.getAbsolutePath();
            android.util.Log.i("capp_lua", "app.lua copied to: " + appLuaPath);

            // Initialize Lua VM with path to copied app.lua
            initLua(appLuaPath);
        } catch (Exception e) {
            android.util.Log.e("capp_lua", "Failed to copy app.lua: " + e.getMessage());
        }
    }

    private byte[] readAsset(String filename) throws java.io.IOException {
        java.io.InputStream is = getAssets().open(filename);
        int size = is.available();
        byte[] buffer = new byte[size];
        is.read(buffer);
        is.close();
        return buffer;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (gameView != null) {
            gameView.onResume();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (gameView != null) {
            gameView.onPause();
        }
    }

    @Override
    protected void onDestroy() {
        if (gameView != null) {
            gameView.stop();
        }
        closeLua();
        super.onDestroy();
    }
}
