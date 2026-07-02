cmake_minimum_required(VERSION 3.10)
project(capp C)

#Cxxdroid-binary: capp_lua

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

find_package(SDL2 REQUIRED)
find_package(Lua QUIET)

option(CAPP_USE_MINILUA "Fall back to vendored minilua when system Lua is not found" ON)
option(CAPP_FORCE_MINILUA "Force use of vendored minilua even if system Lua is found" OFF)

add_executable(capp src/main.c)
target_link_libraries(capp ${SDL2_LIBRARIES})

# Build unified SDL2 app with embedded Lua
if(LUA_FOUND AND NOT CAPP_FORCE_MINILUA)
	message(STATUS "Building SDL2 app with system Lua")
	add_executable(capp_lua src/sdl2_app.c src/lua/lua_sdl.c)
	target_link_libraries(capp_lua ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
	target_include_directories(capp_lua PRIVATE src/lua ${LUA_INCLUDE_DIR})

	# Copy app.lua to build directory
	add_custom_command(TARGET capp_lua POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_SOURCE_DIR}/src/lua/app.lua
			${CMAKE_BINARY_DIR}/app.lua
		COMMENT "Copying app.lua to build directory"
	)
elseif(CAPP_USE_MINILUA)
	message(STATUS "Building SDL2 app with vendored minilua")
	# Create a static library from minilua
	add_library(minilua STATIC src/lua/minilua/minilua.c)
	target_include_directories(minilua PUBLIC src/lua/minilua)
	target_link_libraries(minilua PRIVATE m)

	# Build SDL2 app using minilua
	add_executable(capp_lua src/sdl2_app.c src/lua/lua_sdl.c)
	target_link_libraries(capp_lua ${SDL2_LIBRARIES} minilua)
	target_include_directories(capp_lua PRIVATE src/lua src/lua/minilua)
	target_compile_definitions(capp_lua PRIVATE CAPP_USE_MINILUA_IMPL)

	# Copy app.lua to build directory for easy access
	add_custom_command(TARGET capp_lua POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_SOURCE_DIR}/src/lua/app.lua
			${CMAKE_BINARY_DIR}/app.lua
		COMMENT "Copying app.lua to build directory"
	)
else()
	message(STATUS "System Lua not found and CAPP_USE_MINILUA is OFF; skipping capp_lua target")
endif()
