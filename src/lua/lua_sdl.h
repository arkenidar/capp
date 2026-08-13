/* lua_sdl.h -- entry points for the "sdl" Lua module.
   lua_sdl.c registers the core module (window/renderer/event/rect primitives);
   lua_sdl_gfx.c extends the same module table with texture, font and
   touch/mouse bindings built on top of stb_image / stb_truetype. */

#ifndef LUA_SDL_H
#define LUA_SDL_H

#include "lua_compat.h"

int luaopen_sdl(lua_State *L);

/* Call after luaL_requiref(L, "sdl", luaopen_sdl, 1) with the module table
   still on top of the stack: adds the gfx/text/touch functions and
   constants to it in place. */
void lua_sdl_register_gfx(lua_State *L);

#endif /* LUA_SDL_H */
