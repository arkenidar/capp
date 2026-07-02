#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <SDL2/SDL.h>

#define SDL_WINDOW_METATABLE "SDL_Window"
#define SDL_RENDERER_METATABLE "SDL_Renderer"
#define SDL_EVENT_METATABLE "SDL_Event"
#define SDL_RECT_METATABLE "SDL_Rect"

static int lua_sdl_init(lua_State *L) {
    int flags = luaL_checkinteger(L, 1);
    int result = SDL_Init(flags);
    lua_pushinteger(L, result);
    return 1;
}

static int lua_sdl_quit(lua_State *L) {
    SDL_Quit();
    return 0;
}

static int lua_sdl_get_error(lua_State *L) {
    lua_pushstring(L, SDL_GetError());
    return 1;
}

static int lua_sdl_log_error(lua_State *L) {
    int category = luaL_checkinteger(L, 1);
    const char *msg = luaL_checkstring(L, 2);
    SDL_LogError(category, "%s", msg);
    return 0;
}

static int lua_sdl_create_window(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int w = luaL_checkinteger(L, 4);
    int h = luaL_checkinteger(L, 5);
    Uint32 flags = luaL_checkinteger(L, 6);

    SDL_Window *window = SDL_CreateWindow(title, x, y, w, h, flags);

    if (window) {
        SDL_Window **udata = lua_newuserdata(L, sizeof(SDL_Window*));
        *udata = window;
        luaL_setmetatable(L, SDL_WINDOW_METATABLE);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_sdl_destroy_window(lua_State *L) {
    SDL_Window **window = luaL_checkudata(L, 1, SDL_WINDOW_METATABLE);
    if (*window) {
        SDL_DestroyWindow(*window);
        *window = NULL;
    }
    return 0;
}

static int lua_sdl_create_renderer(lua_State *L) {
    SDL_Window **window = luaL_checkudata(L, 1, SDL_WINDOW_METATABLE);
    int index = luaL_checkinteger(L, 2);
    Uint32 flags = luaL_checkinteger(L, 3);

    SDL_Renderer *renderer = SDL_CreateRenderer(*window, index, flags);

    if (renderer) {
        SDL_Renderer **udata = lua_newuserdata(L, sizeof(SDL_Renderer*));
        *udata = renderer;
        luaL_setmetatable(L, SDL_RENDERER_METATABLE);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_sdl_destroy_renderer(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    if (*renderer) {
        SDL_DestroyRenderer(*renderer);
        *renderer = NULL;
    }
    return 0;
}

static int lua_sdl_poll_event(lua_State *L) {
    SDL_Event *event = luaL_checkudata(L, 1, SDL_EVENT_METATABLE);
    int result = SDL_PollEvent(event);
    lua_pushinteger(L, result);
    return 1;
}

static int lua_sdl_get_ticks(lua_State *L) {
    lua_pushinteger(L, SDL_GetTicks());
    return 1;
}

static int lua_sdl_delay(lua_State *L) {
    Uint32 ms = luaL_checkinteger(L, 1);
    SDL_Delay(ms);
    return 0;
}

static int lua_sdl_set_render_draw_color(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    Uint8 r = luaL_checkinteger(L, 2);
    Uint8 g = luaL_checkinteger(L, 3);
    Uint8 b = luaL_checkinteger(L, 4);
    Uint8 a = luaL_checkinteger(L, 5);

    SDL_SetRenderDrawColor(*renderer, r, g, b, a);
    return 0;
}

static int lua_sdl_render_clear(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    SDL_RenderClear(*renderer);
    return 0;
}

static int lua_sdl_render_fill_rect(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    SDL_Rect *rect = luaL_checkudata(L, 2, SDL_RECT_METATABLE);
    SDL_RenderFillRect(*renderer, rect);
    return 0;
}

static int lua_sdl_render_present(lua_State *L) {
    SDL_Renderer **renderer = luaL_checkudata(L, 1, SDL_RENDERER_METATABLE);
    SDL_RenderPresent(*renderer);
    return 0;
}

static int lua_sdl_event_new(lua_State *L) {
    SDL_Event *event = lua_newuserdata(L, sizeof(SDL_Event));
    SDL_zero(*event);
    luaL_setmetatable(L, SDL_EVENT_METATABLE);
    return 1;
}

static int lua_sdl_event_type(lua_State *L) {
    SDL_Event *event = luaL_checkudata(L, 1, SDL_EVENT_METATABLE);
    lua_pushinteger(L, event->type);
    return 1;
}

static int lua_sdl_event_key_scancode(lua_State *L) {
    SDL_Event *event = luaL_checkudata(L, 1, SDL_EVENT_METATABLE);
    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
        lua_pushinteger(L, event->key.keysym.scancode);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_sdl_rect_new(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);

    SDL_Rect *rect = lua_newuserdata(L, sizeof(SDL_Rect));
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
    luaL_setmetatable(L, SDL_RECT_METATABLE);
    return 1;
}

static const luaL_Reg sdl_funcs[] = {
    {"init", lua_sdl_init},
    {"quit", lua_sdl_quit},
    {"get_error", lua_sdl_get_error},
    {"log_error", lua_sdl_log_error},
    {"create_window", lua_sdl_create_window},
    {"destroy_window", lua_sdl_destroy_window},
    {"create_renderer", lua_sdl_create_renderer},
    {"destroy_renderer", lua_sdl_destroy_renderer},
    {"poll_event", lua_sdl_poll_event},
    {"get_ticks", lua_sdl_get_ticks},
    {"delay", lua_sdl_delay},
    {"set_render_draw_color", lua_sdl_set_render_draw_color},
    {"render_clear", lua_sdl_render_clear},
    {"render_fill_rect", lua_sdl_render_fill_rect},
    {"render_present", lua_sdl_render_present},
    {"event_new", lua_sdl_event_new},
    {"event_type", lua_sdl_event_type},
    {"event_key_scancode", lua_sdl_event_key_scancode},
    {"rect_new", lua_sdl_rect_new},
    {NULL, NULL}
};

int luaopen_sdl(lua_State *L) {
    luaL_newlib(L, sdl_funcs);

    lua_pushinteger(L, SDL_INIT_VIDEO);
    lua_setfield(L, -2, "INIT_VIDEO");

    lua_pushinteger(L, SDL_LOG_CATEGORY_APPLICATION);
    lua_setfield(L, -2, "LOG_CATEGORY_APPLICATION");

    lua_pushinteger(L, SDL_WINDOWPOS_CENTERED);
    lua_setfield(L, -2, "WINDOWPOS_CENTERED");

    lua_pushinteger(L, SDL_WINDOW_SHOWN);
    lua_setfield(L, -2, "WINDOW_SHOWN");

    lua_pushinteger(L, SDL_RENDERER_ACCELERATED);
    lua_setfield(L, -2, "RENDERER_ACCELERATED");

    lua_pushinteger(L, SDL_RENDERER_PRESENTVSYNC);
    lua_setfield(L, -2, "RENDERER_PRESENTVSYNC");

    lua_pushinteger(L, SDL_QUIT);
    lua_setfield(L, -2, "QUIT");

    lua_pushinteger(L, SDL_KEYDOWN);
    lua_setfield(L, -2, "KEYDOWN");

    lua_pushinteger(L, SDL_SCANCODE_ESCAPE);
    lua_setfield(L, -2, "SCANCODE_ESCAPE");

    luaL_newmetatable(L, SDL_WINDOW_METATABLE);
    lua_pop(L, 1);

    luaL_newmetatable(L, SDL_RENDERER_METATABLE);
    lua_pop(L, 1);

    luaL_newmetatable(L, SDL_EVENT_METATABLE);
    lua_pop(L, 1);

    luaL_newmetatable(L, SDL_RECT_METATABLE);
    lua_pop(L, 1);

    return 1;
}
