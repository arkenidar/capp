#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <SDL2/SDL.h>
#include <stdio.h>

extern int luaopen_sdl(lua_State *L);

int main(int argc, char *argv[]) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_requiref(L, "sdl", luaopen_sdl, 1);
    lua_pop(L, 1);

    if (luaL_dofile(L, "src/lua/app.lua") != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}
