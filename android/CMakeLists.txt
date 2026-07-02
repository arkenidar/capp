cmake_minimum_required(VERSION 3.22)
project(capp_android C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Android NDK integration for capp_lua
# Builds embedded Lua 5.1 VM (minilua)
# Note: SDL2 requires separate compilation for Android NDK

# Minilua (embedded Lua 5.1 - single file implementation)
add_library(minilua STATIC
    ../src/lua/minilua/minilua.c
)
target_include_directories(minilua PUBLIC ../src/lua/minilua)
target_compile_definitions(minilua PRIVATE _POSIX_C_SOURCE=200809L)
# Minilua uses some functions that may not be available, so treat errors as warnings
target_compile_options(minilua PRIVATE -Wno-error=implicit-function-declaration)
target_link_libraries(minilua PRIVATE m)

# capp_lua native library with embedded Lua VM
# Android version: Lua VM without SDL2 (rendering done by Java/Android)
set(SOURCES
    src/main_android_lua.c
)

add_library(capp_native SHARED ${SOURCES})

target_include_directories(capp_native PRIVATE
    ../src/lua/minilua
)

target_compile_definitions(capp_native PRIVATE CAPP_USE_MINILUA_IMPL)
target_link_libraries(capp_native minilua m log)

# Platform-specific flags
target_compile_options(capp_native PRIVATE -fPIC -O2)
target_link_options(capp_native PRIVATE -Wl,--no-undefined)
