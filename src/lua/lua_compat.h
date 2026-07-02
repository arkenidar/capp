/* lua_compat.h -- Compatibility layer for both system Lua and minilua

   This header allows the same Lua code to compile with either:
   - System-installed Lua (lua.h, lauxlib.h, lualib.h)
   - Vendored minilua (single minilua.h header)
*/

#ifndef LUA_COMPAT_H
#define LUA_COMPAT_H

#ifdef CAPP_USE_MINILUA_IMPL
/* Using minilua - single header includes everything */
#include "minilua.h"
#else
/* Using system Lua - standard includes */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif

#endif /* LUA_COMPAT_H */
