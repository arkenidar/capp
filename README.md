# capp

A checkerboard animation demo using SDL2 with both pure C and embedded Lua implementations.

## Quick Start

```bash
cd ~/Dropbox/app/capp && mkdir -p build && cd build && cmake .. && make && ./capp_lua
```

## Features

- **Pure C version** (`capp`) — Direct SDL2 rendering
- **Lua-embedded version** (`capp_lua`) — Lua VM with SDL2 C API bindings
  - Works with system-installed Lua (5.1+), falls back to vendored minilua if unavailable
  - Game logic in Lua, rendering in C
  - Easily extensible C API bindings

## Building

### Prerequisites

- SDL2 development libraries
- Lua (5.1+) development libraries (optional — minilua will be used as a fallback if not found)
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
- `./capp_lua` — Lua-embedded version (if Lua is found, or minilua is available)

#### Lua Variant Selection

By default, the build will:
1. Use system-installed Lua if `find_package(Lua)` succeeds
2. Fall back to the vendored minilua if system Lua is not found (and `CAPP_USE_MINILUA=ON`)
3. Skip building `capp_lua` if neither system Lua nor minilua are available (and `CAPP_USE_MINILUA=OFF`)

To explicitly control this behavior:

```bash
# Use only system Lua, disable minilua fallback
cmake -DCAPP_USE_MINILUA=OFF ..

# Force use of vendored minilua even if system Lua is found
cmake -DCAPP_FORCE_MINILUA=ON ..
```

The `capp_lua` target will automatically use whichever Lua implementation is selected. System Lua takes priority unless `CAPP_FORCE_MINILUA=ON` is set.

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
