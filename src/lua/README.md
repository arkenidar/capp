# `sdl` toolkit API

The native engine (`lua_sdl.c` + `lua_sdl_gfx.c`, compiled into every app binary)
exposes one Lua module, `require("sdl")`. It's app-agnostic — game logic lives
in `apps/*.lua`, not here. Add a new game by dropping a `.lua` file in `apps/`
and building with `-DCAPP_APP=<name>` (desktop) or editing the sync task in
`android/app/build.gradle.kts` (Android); see `../../CMakeLists.txt`.

## Core (`lua_sdl.c`)

| Function | Notes |
|---|---|
| `sdl.init(flags)` / `sdl.quit()` | `flags` e.g. `sdl.INIT_VIDEO` |
| `sdl.get_error()` / `sdl.log_error(category, msg)` | |
| `sdl.create_window(title, x, y, w, h, flags) -> window|nil` | |
| `sdl.destroy_window(window)` | |
| `sdl.set_window_bordered(window, bool)` / `set_window_position(window, x, y)` / `set_window_size(window, w, h)` | |
| `sdl.create_renderer(window, index, flags) -> renderer|nil` | |
| `sdl.destroy_renderer(renderer)` | |
| `sdl.set_render_draw_color(renderer, r, g, b, a)` | |
| `sdl.render_clear(renderer)` / `sdl.render_fill_rect(renderer, rect)` / `sdl.render_present(renderer)` | |
| `sdl.rect_new(x, y, w, h) -> rect` | opaque `SDL_Rect` userdata, write-only from Lua |
| `sdl.get_desktop_display_mode(display_index) -> {w, h, refresh_rate, format}|nil` | use to size a fullscreen window to whatever screen it's on |
| `sdl.event_new() -> event` / `sdl.poll_event(event) -> int` | reusable event userdata; loop `while sdl.poll_event(event) ~= 0 do ... end` |
| `sdl.event_type(event) -> int` | compare against `sdl.QUIT`, `sdl.KEYDOWN`, `sdl.MOUSEBUTTONDOWN`, `sdl.FINGERDOWN` |
| `sdl.event_key_scancode(event) -> int|nil` | only for `KEYDOWN`/`KEYUP`; compare against `sdl.SCANCODE_ESCAPE` |
| `sdl.get_ticks()` / `sdl.delay(ms)` | ms since init; never block the loop for long waits, poll `get_ticks()` instead |
| `sdl.get_asset_path(relative_path) -> path` | resolves a path under the bundled assets dir; passthrough on desktop, prefixes Android's internal storage dir on APK builds — use this for any file your app loads (fonts, images) |

Constants: `INIT_VIDEO`, `LOG_CATEGORY_APPLICATION`, `WINDOWPOS_CENTERED`,
`WINDOW_SHOWN`, `RENDERER_ACCELERATED`, `RENDERER_PRESENTVSYNC`, `QUIT`,
`KEYDOWN`, `SCANCODE_ESCAPE`, `FALSE`.

## Graphics extensions (`lua_sdl_gfx.c`, stb_image / stb_truetype)

| Function | Notes |
|---|---|
| `sdl.load_texture(renderer, path) -> texture, w, h` or `nil, err` | decodes PNG/JPG/BMP/etc via stb_image |
| `sdl.render_copy(renderer, texture, dst_rect)` | draws the whole texture stretched into `dst_rect` |
| `sdl.destroy_texture(texture)` | |
| `sdl.load_font(renderer, path, pixel_height) -> font` or `nil, err` | rasterizes a TTF/OTF via stb_truetype into one atlas texture per call; call once per size you need, not per frame |
| `sdl.render_text(renderer, font, text, x, y, r, g, b, a)` | `x, y` is the baseline of the first glyph (ASCII 32-126 only); no ascent/descent accessor is exposed, so vertical centering needs a caller-side fudge factor (`apps/memory_game.lua` uses ~0.35×pixel_height) |
| `sdl.text_width(font, text) -> px` | for centering, without drawing |
| `sdl.destroy_font(font)` | |
| `sdl.event_mouse_button(event) -> button, x, y` or `nil` | only for `MOUSEBUTTONDOWN`/`UP`; pixel coords |
| `sdl.event_finger(event, window) -> x, y` or `nil` | only for `FINGERDOWN`/`UP`/`MOTION`; SDL reports touch as 0..1 normalized, this converts to pixels using the window's current size |

Constants: `MOUSEBUTTONDOWN`, `FINGERDOWN`.

Neither module does resource cleanup automatically (no `__gc`) — match every
`create_*`/`load_*` with the corresponding `destroy_*`, same convention as
the existing window/renderer calls.
