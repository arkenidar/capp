package com.capp.app;

import org.libsdl.app.SDLActivity;
import android.os.Bundle;

public class MainActivity extends SDLActivity {

    static {
        System.loadLibrary("capp_native");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Copy the whole assets/ bundle (app.lua, fonts/*, ...) into the
        // writable files dir for SDL_main and sdl.get_asset_path() to find.
        // Copies raw bytes, not through a Java String, so binary assets
        // (fonts, images) survive intact.
        try {
            copyAssetTree("", getFilesDir());
        } catch (Exception e) {
            android.util.Log.e("capp", "Failed to copy assets: " + e.getMessage());
        }

        super.onCreate(savedInstanceState);
    }

    /* Only ever called with a path already known to be a directory (see the
     * grandchildren check below), so entries is never a leaf file itself. */
    private void copyAssetTree(String assetPath, java.io.File destDir) throws java.io.IOException {
        String[] entries = getAssets().list(assetPath);
        for (String entry : entries) {
            String childAssetPath = assetPath.isEmpty() ? entry : assetPath + "/" + entry;
            String[] grandchildren = getAssets().list(childAssetPath);
            if (grandchildren != null && grandchildren.length > 0) {
                java.io.File childDestDir = new java.io.File(destDir, entry);
                childDestDir.mkdirs();
                copyAssetTree(childAssetPath, childDestDir);
            } else {
                copyAssetFile(childAssetPath, new java.io.File(destDir, entry));
            }
        }
    }

    private void copyAssetFile(String assetPath, java.io.File destFile) throws java.io.IOException {
        try (java.io.InputStream is = getAssets().open(assetPath);
             java.io.FileOutputStream os = new java.io.FileOutputStream(destFile)) {
            byte[] buffer = new byte[8192];
            int read;
            while ((read = is.read(buffer)) != -1) {
                os.write(buffer, 0, read);
            }
        }
        android.util.Log.i("capp", "Copied asset " + assetPath + " to " + destFile.getAbsolutePath());
    }
}
