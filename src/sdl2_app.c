#include "lua_compat.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

/* CAPP_ANDROID_APK is defined only by android/CMakeLists.txt, which links
 * against SDL2's Android JNI glue (Activity, nativeInit -> SDL_main).
 * Plain __ANDROID__ is not enough: Termux's clang also targets bionic and
 * defines it, but a Termux binary is a normal executable with no JNI
 * bridge, so the Android-only SDL calls below have nothing to call into. */
#ifdef CAPP_ANDROID_APK
#include <android/log.h>
#include <SDL2/SDL_system.h>
#define LOG_TAG "capp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(fmt, ...) fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif

extern int luaopen_sdl(lua_State *L);

/* Get the path to app.lua
 * Desktop / Termux: looks in build directory and source directory
 * Android APK: uses SDL_AndroidGetInternalStoragePath()
 */
static const char* get_app_lua_path(void) {
    static char path[512] = {0};

#ifdef CAPP_ANDROID_APK
    const char *files_dir = SDL_AndroidGetInternalStoragePath();
    snprintf(path, sizeof(path), "%s/app.lua", files_dir);
    LOGI("Using Android app.lua path: %s", path);
#else
    /* Try build directory first (when run via cmake build) */
    FILE *f = fopen("app.lua", "r");
    if (f) {
        fclose(f);
        return "app.lua";
    }

    /* Try source directory */
    f = fopen("src/lua/app.lua", "r");
    if (f) {
        fclose(f);
        return "src/lua/app.lua";
    }

    LOGE("app.lua not found in current directory or src/lua/");
    return NULL;
#endif

    return path;
}

#ifdef CAPP_ANDROID_APK
int SDL_main(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
    LOGI("Starting capp SDL2 application");

    lua_State *L = luaL_newstate();
    if (!L) {
        LOGE("Failed to create Lua state");
        return 1;
    }

    luaL_openlibs(L);
    LOGI("Lua state initialized with standard libraries");

    /* Register SDL module */
    luaL_requiref(L, "sdl", luaopen_sdl, 1);
    lua_pop(L, 1);
    LOGI("SDL module registered");

    /* Load and execute app.lua */
    const char *app_lua_path = get_app_lua_path();
    if (!app_lua_path) {
        LOGE("Could not determine app.lua path");
        lua_close(L);
        return 1;
    }

    LOGI("Loading app.lua from: %s", app_lua_path);

    if (luaL_dofile(L, app_lua_path) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        LOGE("Lua error: %s", error);
        lua_close(L);
        return 1;
    }

    LOGI("App completed successfully");
    lua_close(L);
    return 0;
}
