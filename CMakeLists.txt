cmake_minimum_required(VERSION 3.10)
project(capp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

find_package(SDL2 REQUIRED)
find_package(Lua REQUIRED)

add_executable(capp src/main.c)
target_link_libraries(capp ${SDL2_LIBRARIES})

add_executable(capp_lua src/lua/main_embed_lua.c src/lua/lua_sdl.c)
target_link_libraries(capp_lua ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
target_include_directories(capp_lua PRIVATE ${LUA_INCLUDE_DIR})
