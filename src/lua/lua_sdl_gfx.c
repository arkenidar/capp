/* lua_sdl_gfx.c -- texture (stb_image), font (stb_truetype) and touch/mouse
   bindings, extending the "sdl" Lua module registered by lua_sdl.c.
   Kept in its own translation unit so the core SDL bindings in lua_sdl.c
   stay untouched by this stb-dependent extension. */

#include "lua_compat.h"
#include "lua_sdl.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb_image.h"
#include "stb_truetype.h"

#define SDL_WINDOW_METATABLE "SDL_Window"
#define SDL_RENDERER_METATABLE "SDL_Renderer"
#define SDL_EVENT_METATABLE "SDL_Event"
#define SDL_RECT_METATABLE "SDL_Rect"
#define SDL_TEXTURE_METATABLE "SDL_Texture"
#define SDL_FONT_METATABLE "SDL_Font"

/* ASCII 32..126 (95 printable chars) baked into a single atlas texture. */
#define FONT_FIRST_CHAR 32
#define FONT_NUM_CHARS 95
#define FONT_ATLAS_SIZE 1024

typedef struct {
    SDL_Texture *atlas;
    stbtt_bakedchar cdata[FONT_NUM_CHARS];
} LuaFont;

/* ---- textures (stb_image) ---- */

static int lua_sdl_load_texture(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    const char *path = luaL_checkstring(L, 2);

    int w, h, channels;
    unsigned char *pixels = stbi_load(path, &w, &h, &channels, 4);
    if (!pixels) {
        lua_pushnil(L);
        lua_pushstring(L, stbi_failure_reason());
        return 2;
    }

    SDL_Texture *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC, w, h);
    if (!texture) {
        stbi_image_free(pixels);
        lua_pushnil(L);
        lua_pushstring(L, SDL_GetError());
        return 2;
    }
    SDL_UpdateTexture(texture, NULL, pixels, w * 4);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    stbi_image_free(pixels);

    SDL_Texture **udata = lua_newuserdata(L, sizeof(SDL_Texture *));
    *udata = texture;
    luaL_setmetatable(L, SDL_TEXTURE_METATABLE);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 3;
}

static int lua_sdl_render_copy(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    SDL_Texture **texture = luaL_checkudata(L, 2, SDL_TEXTURE_METATABLE);
    SDL_Rect *dst = luaL_checkudata(L, 3, SDL_RECT_METATABLE);
    SDL_RenderCopy(*renderer, *texture, NULL, dst);
    return 0;
}

static int lua_sdl_destroy_texture(lua_State *L) {
    SDL_Texture **texture = luaL_checkudata(L, 1, SDL_TEXTURE_METATABLE);
    if (*texture) {
        SDL_DestroyTexture(*texture);
        *texture = NULL;
    }
    return 0;
}

/* ---- fonts (stb_truetype) ---- */

static int lua_sdl_load_font(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    const char *path = luaL_checkstring(L, 2);
    lua_Number pixel_height = luaL_checknumber(L, 3);

    FILE *f = fopen(path, "rb");
    if (!f) {
        lua_pushnil(L);
        lua_pushfstring(L, "could not open font file: %s", path);
        return 2;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *ttf_buffer = malloc((size_t)size);
    fread(ttf_buffer, 1, (size_t)size, f);
    fclose(f);

    unsigned char *bitmap = malloc((size_t)FONT_ATLAS_SIZE * FONT_ATLAS_SIZE);
    LuaFont *font = lua_newuserdata(L, sizeof(LuaFont));
    font->atlas = NULL;

    int result = stbtt_BakeFontBitmap(ttf_buffer, 0, (float)pixel_height,
                                       bitmap, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE,
                                       FONT_FIRST_CHAR, FONT_NUM_CHARS, font->cdata);
    free(ttf_buffer);

    if (result == 0) {
        free(bitmap);
        lua_pop(L, 1); /* drop the half-built font userdata */
        lua_pushnil(L);
        lua_pushstring(L, "stbtt_BakeFontBitmap: font atlas too small for requested pixel_height");
        return 2;
    }

    /* Expand the single-channel coverage bitmap into RGBA (white, alpha =
     * coverage) so render_text can tint it per-call via texture color mod. */
    unsigned char *rgba = malloc((size_t)FONT_ATLAS_SIZE * FONT_ATLAS_SIZE * 4);
    for (int i = 0; i < FONT_ATLAS_SIZE * FONT_ATLAS_SIZE; i++) {
        rgba[i * 4 + 0] = 255;
        rgba[i * 4 + 1] = 255;
        rgba[i * 4 + 2] = 255;
        rgba[i * 4 + 3] = bitmap[i];
    }
    free(bitmap);

    SDL_Texture *atlas = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_STATIC,
                                            FONT_ATLAS_SIZE, FONT_ATLAS_SIZE);
    if (!atlas) {
        free(rgba);
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_pushstring(L, SDL_GetError());
        return 2;
    }
    SDL_UpdateTexture(atlas, NULL, rgba, FONT_ATLAS_SIZE * 4);
    SDL_SetTextureBlendMode(atlas, SDL_BLENDMODE_BLEND);
    free(rgba);

    font->atlas = atlas;
    luaL_setmetatable(L, SDL_FONT_METATABLE);
    return 1;
}

static int lua_sdl_render_text(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    LuaFont *font = luaL_checkudata(L, 2, SDL_FONT_METATABLE);
    const char *text = luaL_checkstring(L, 3);
    float x = (float)luaL_checknumber(L, 4);
    float y = (float)luaL_checknumber(L, 5);
    Uint8 r = (Uint8)luaL_checkinteger(L, 6);
    Uint8 g = (Uint8)luaL_checkinteger(L, 7);
    Uint8 b = (Uint8)luaL_checkinteger(L, 8);
    Uint8 a = (Uint8)luaL_checkinteger(L, 9);

    SDL_SetTextureColorMod(font->atlas, r, g, b);
    SDL_SetTextureAlphaMod(font->atlas, a);

    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (*p < FONT_FIRST_CHAR || *p >= FONT_FIRST_CHAR + FONT_NUM_CHARS) {
            continue;
        }
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font->cdata, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE,
                            *p - FONT_FIRST_CHAR, &x, &y, &q, 1);

        SDL_Rect src = {
            (int)(q.s0 * FONT_ATLAS_SIZE), (int)(q.t0 * FONT_ATLAS_SIZE),
            (int)((q.s1 - q.s0) * FONT_ATLAS_SIZE), (int)((q.t1 - q.t0) * FONT_ATLAS_SIZE)
        };
        SDL_Rect dst = {
            (int)q.x0, (int)q.y0, (int)(q.x1 - q.x0), (int)(q.y1 - q.y0)
        };
        SDL_RenderCopy(*renderer, font->atlas, &src, &dst);
    }
    return 0;
}

static int lua_sdl_text_width(lua_State *L) {
    LuaFont *font = luaL_checkudata(L, 1, SDL_FONT_METATABLE);
    const char *text = luaL_checkstring(L, 2);

    float width = 0.0f;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (*p < FONT_FIRST_CHAR || *p >= FONT_FIRST_CHAR + FONT_NUM_CHARS) {
            continue;
        }
        width += font->cdata[*p - FONT_FIRST_CHAR].xadvance;
    }
    lua_pushnumber(L, width);
    return 1;
}

static int lua_sdl_destroy_font(lua_State *L) {
    LuaFont *font = luaL_checkudata(L, 1, SDL_FONT_METATABLE);
    if (font->atlas) {
        SDL_DestroyTexture(font->atlas);
        font->atlas = NULL;
    }
    return 0;
}

/* ---- touch / mouse input ---- */

static int lua_sdl_event_mouse_button(lua_State *L) {
    SDL_Event *event = luaL_checkudata(L, 1, SDL_EVENT_METATABLE);
    if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
        lua_pushinteger(L, event->button.button);
        lua_pushinteger(L, event->button.x);
        lua_pushinteger(L, event->button.y);
        return 3;
    }
    lua_pushnil(L);
    return 1;
}

/* SDL reports finger coordinates normalized to [0,1]; convert to pixels
 * using the current window size (Android's window matches the screen, so
 * this adapts automatically to whatever the device's resolution is). */
static int lua_sdl_event_finger(lua_State *L) {
    SDL_Event *event = luaL_checkudata(L, 1, SDL_EVENT_METATABLE);
    SDL_Window **window = luaL_checkudata(L, 2, SDL_WINDOW_METATABLE);
    if (event->type == SDL_FINGERDOWN || event->type == SDL_FINGERUP ||
        event->type == SDL_FINGERMOTION) {
        int w, h;
        SDL_GetWindowSize(*window, &w, &h);
        lua_pushnumber(L, event->tfinger.x * w);
        lua_pushnumber(L, event->tfinger.y * h);
        return 2;
    }
    lua_pushnil(L);
    return 1;
}

static const luaL_Reg sdl_gfx_funcs[] = {
    {"load_texture", lua_sdl_load_texture},
    {"render_copy", lua_sdl_render_copy},
    {"destroy_texture", lua_sdl_destroy_texture},
    {"load_font", lua_sdl_load_font},
    {"render_text", lua_sdl_render_text},
    {"text_width", lua_sdl_text_width},
    {"destroy_font", lua_sdl_destroy_font},
    {"event_mouse_button", lua_sdl_event_mouse_button},
    {"event_finger", lua_sdl_event_finger},
    {NULL, NULL}
};

void lua_sdl_register_gfx(lua_State *L) {
    /* sdl module table must already be on top of the stack. */
    luaL_setfuncs(L, sdl_gfx_funcs, 0);

    lua_pushinteger(L, SDL_MOUSEBUTTONDOWN);
    lua_setfield(L, -2, "MOUSEBUTTONDOWN");

    lua_pushinteger(L, SDL_FINGERDOWN);
    lua_setfield(L, -2, "FINGERDOWN");

    luaL_newmetatable(L, SDL_TEXTURE_METATABLE);
    lua_pop(L, 1);

    luaL_newmetatable(L, SDL_FONT_METATABLE);
    lua_pop(L, 1);
}
