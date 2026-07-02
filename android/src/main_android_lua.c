#include <jni.h>
#include "minilua.h"
#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "capp_lua"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static lua_State *L = NULL;
static const char *app_lua_path = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnLoad - capp_lua Android with minilua");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnUnload");
    if (L) {
        lua_close(L);
        L = NULL;
    }
    if (app_lua_path) {
        free((void *)app_lua_path);
        app_lua_path = NULL;
    }
}

JNIEXPORT void JNICALL Java_com_capp_app_MainActivity_initLua(JNIEnv *env, jobject obj, jstring assetsPath) {
    LOGI("Initializing Lua VM with app.lua");

    if (L) {
        LOGE("Lua VM already initialized");
        return;
    }

    // Create Lua state
    L = luaL_newstate();
    if (!L) {
        LOGE("Failed to create Lua state");
        return;
    }

    // Open standard libraries (math, table, string, etc.)
    luaL_openlibs(L);
    LOGI("Lua state created and standard libraries loaded");

    // Get assets path from Java
    const char *assets_path = (*env)->GetStringUTFChars(env, assetsPath, NULL);
    if (!assets_path) {
        LOGE("Failed to get assets path");
        lua_close(L);
        L = NULL;
        return;
    }

    // Store path (Java already passes the full path to app.lua)
    app_lua_path = (const char *)malloc(strlen(assets_path) + 1);
    strcpy((char *)app_lua_path, assets_path);

    LOGI("Loading app.lua from: %s", app_lua_path);

    // Load and execute app.lua
    int load_result = luaL_dofile(L, app_lua_path);
    if (load_result != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        LOGE("Failed to load app.lua: %s (error code: %d)", error, load_result);
        lua_pop(L, 1);
        lua_close(L);
        L = NULL;
    } else {
        LOGI("app.lua loaded successfully!");

        // Verify update function exists
        lua_getglobal(L, "update");
        if (lua_isfunction(L, -1)) {
            LOGI("update() function found in app.lua");
        } else {
            LOGE("update() function NOT found in app.lua");
        }
        lua_pop(L, 1);
    }

    (*env)->ReleaseStringUTFChars(env, assetsPath, assets_path);
}

JNIEXPORT jdouble JNICALL Java_com_capp_app_MainActivity_runLuaFrame(JNIEnv *env, jobject obj, jlong ticks_ms) {
    if (!L) {
        LOGE("Lua state is NULL");
        return 0.0;
    }

    // Call Lua update function with current time in milliseconds
    lua_getglobal(L, "update");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, (lua_Integer)ticks_ms);
        int call_result = lua_pcall(L, 1, 1, 0);
        if (call_result != LUA_OK) {
            const char *error = lua_tostring(L, -1);
            LOGE("Lua error in update: %s (error code: %d)", error, call_result);
            lua_pop(L, 1);
            return 0.0;
        }

        // Get return value (animation shift)
        double shift = 0.0;
        if (lua_isnumber(L, -1)) {
            shift = lua_tonumber(L, -1);
        } else {
            LOGE("update() did not return a number, got type: %s", lua_typename(L, lua_type(L, -1)));
        }
        lua_pop(L, 1);


        return shift;
    } else {
        LOGE("update() function not found or not callable");
        lua_pop(L, 1);
        return 0.0;
    }
}

JNIEXPORT void JNICALL Java_com_capp_app_MainActivity_closeLua(JNIEnv *env, jobject obj) {
    LOGI("Closing Lua VM");
    if (L) {
        lua_close(L);
        L = NULL;
    }
}
