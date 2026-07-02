cmake_minimum_required(VERSION 3.10)
project(capp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

find_package(SDL2 REQUIRED)
find_package(Lua QUIET)

option(CAPP_USE_MINILUA "Fall back to vendored minilua when system Lua is not found" ON)
option(CAPP_FORCE_MINILUA "Force use of vendored minilua even if system Lua is found" OFF)

add_executable(capp src/main.c)
target_link_libraries(capp ${SDL2_LIBRARIES})

# Build capp_lua with system Lua if found, otherwise optionally use vendored minilua
if(LUA_FOUND AND NOT CAPP_FORCE_MINILUA)
	message(STATUS "Building capp_lua with system Lua")
	add_executable(capp_lua src/lua/main_embed_lua.c src/lua/lua_sdl.c)
	target_link_libraries(capp_lua ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
	target_include_directories(capp_lua PRIVATE src/lua ${LUA_INCLUDE_DIR})
elseif(CAPP_USE_MINILUA)
	message(STATUS "Building capp_lua with vendored minilua")
	# Create a static library from minilua
	add_library(minilua STATIC src/lua/minilua/minilua.c)
	target_include_directories(minilua PUBLIC src/lua/minilua)
	target_link_libraries(minilua PRIVATE m)  # Link against libm for math functions

	# Build capp_lua using minilua
	add_executable(capp_lua src/lua/main_embed_lua.c src/lua/lua_sdl.c)
	target_link_libraries(capp_lua ${SDL2_LIBRARIES} minilua)
	target_include_directories(capp_lua PRIVATE src/lua src/lua/minilua)
	target_compile_definitions(capp_lua PRIVATE CAPP_USE_MINILUA_IMPL)
else()
	message(STATUS "System Lua not found and CAPP_USE_MINILUA is OFF; skipping capp_lua target")
endif()
