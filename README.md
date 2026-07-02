# capp

A checkerboard animation demo using SDL2 with both pure C and embedded Lua implementations.

## Quick Start

```bash
cd ~/Dropbox/app/capp && mkdir -p build && cd build && cmake .. && make && ./capp_lua
```

## Features

- **Pure C version** (`capp`) — Direct SDL2 rendering
- **Lua-embedded version** (`capp_lua`) — Lua VM with SDL2 C API bindings
  - Works with any Lua version (PUC-Lua, minilua, LuaJIT, Lua 5.4+)
  - Game logic in Lua, rendering in C
  - Easily extensible C API bindings

## Building

### Prerequisites

- SDL2 development libraries
- Lua (5.1+) development libraries
- CMake 3.10+
- C compiler (gcc/clang)

### Build Steps

```bash
cd ~/Dropbox/app/capp
mkdir build
cd build
cmake ..
make
```

This builds both targets:
- `./capp` — Pure C version
- `./capp_lua` — Lua-embedded version

## Running

```bash
# Pure C version
./capp

# Lua-embedded version
./capp_lua
```

Press **Escape** or close the window to exit.

## Architecture

### Pure C Version
- Single-file implementation in `src/main.c`
- Direct SDL2 API calls
- Simple animation loop with event handling

### Lua-Embedded Version
- **`src/main_embed_lua.c`** — Embeds Lua VM
- **`src/lua_sdl.c`** — SDL2 bindings for Lua (C API)
- **`src/app.lua`** — Application logic in Lua

The embedded Lua version exposes SDL2 functions through a `sdl` module:
- `sdl.init()`, `sdl.quit()`
- `sdl.create_window()`, `sdl.create_renderer()`
- `sdl.set_render_draw_color()`, `sdl.render_clear()`, `sdl.render_present()`
- `sdl.poll_event()`, `sdl.get_ticks()`, `sdl.delay()`
- `sdl.event_new()`, `sdl.rect_new()`

## Development

### Adding Lua Bindings

To add new SDL2 functions to the Lua API:

1. Add a wrapper function in `src/lua_sdl.c`:
   ```c
   static int lua_sdl_my_function(lua_State *L) {
       // Extract args with luaL_checkinteger, luaL_checkstring, etc.
       // Call SDL function
       // Push result with lua_pushinteger, lua_pushstring, etc.
       return 1; // number of return values
   }
   ```

2. Register in the `sdl_funcs` array:
   ```c
   {"my_function", lua_sdl_my_function},
   ```

3. Use in Lua:
   ```lua
   local result = sdl.my_function(arg1, arg2)
   ```

## License

MIT — See [LICENSE](LICENSE)
