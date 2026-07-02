cmake_minimum_required(VERSION 3.22)
project(capp_android C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Android NDK integration for capp_lua with SDL2
set(SDL2_PATH "${ANDROID_SDL2_PATH}" CACHE PATH "SDL2 source path")

# Add SDL2 Android native build support
if(EXISTS "${SDL2_PATH}/CMakeLists.txt")
    add_subdirectory("${SDL2_PATH}" SDL2)
else()
    message(WARNING "SDL2_PATH not found at ${SDL2_PATH}")
endif()

# Minilua (embedded Lua 5.1 - single file implementation)
add_library(minilua STATIC
    ../src/lua/minilua/minilua.c
)
target_include_directories(minilua PUBLIC ../src/lua/minilua)
target_compile_definitions(minilua PRIVATE _POSIX_C_SOURCE=200809L)
# Minilua uses some functions that may not be available, so treat errors as warnings
target_compile_options(minilua PRIVATE -Wno-error=implicit-function-declaration)
target_link_libraries(minilua PRIVATE m)

# capp_lua native library with embedded Lua VM and SDL2
# Uses unified sdl2_app.c that works on both desktop and Android
set(SOURCES
    ../src/sdl2_app.c
    ../src/lua/lua_sdl.c
)

add_library(capp_native SHARED ${SOURCES})

target_include_directories(capp_native PRIVATE
    ../src/lua
    ../src/lua/minilua
)

target_compile_definitions(capp_native PRIVATE CAPP_USE_MINILUA_IMPL)
target_link_libraries(capp_native minilua m log SDL2::SDL2)

# Include SDL2 headers
target_include_directories(capp_native PRIVATE
    ${SDL2_PATH}/include
)

# Platform-specific flags
target_compile_options(capp_native PRIVATE -fPIC -O2)
target_link_options(capp_native PRIVATE -Wl,--no-undefined)
