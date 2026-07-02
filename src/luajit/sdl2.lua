local ffi = require("ffi")

ffi.cdef [[
    typedef struct {
        int32_t scancode;
        int32_t sym;
        uint16_t mod;
        uint32_t unused;
    } SDL_Keysym;

    typedef struct {
        uint32_t type;
        uint32_t timestamp;
        uint32_t windowID;
        uint8_t state;
        uint8_t repeat;
        uint8_t padding1;
        uint8_t padding2;
        SDL_Keysym keysym;
    } SDL_KeyboardEvent;

    typedef struct {
        uint32_t type;
        uint32_t timestamp;
    } SDL_QuitEvent;

    typedef union {
        uint32_t type;
        SDL_QuitEvent quit;
        SDL_KeyboardEvent key;
        uint8_t padding[56];
    } SDL_Event;

    typedef struct {
        int x, y, w, h;
    } SDL_Rect;

    typedef void SDL_Window;
    typedef void SDL_Renderer;

    int SDL_Init(uint32_t flags);
    void SDL_Quit(void);
    const char* SDL_GetError(void);

    SDL_Window* SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags);
    void SDL_DestroyWindow(SDL_Window *window);

    SDL_Renderer* SDL_CreateRenderer(SDL_Window *window, int index, uint32_t flags);
    void SDL_DestroyRenderer(SDL_Renderer *renderer);

    int SDL_PollEvent(SDL_Event *event);
    uint32_t SDL_GetTicks(void);
    void SDL_Delay(uint32_t ms);

    int SDL_SetRenderDrawColor(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    int SDL_RenderClear(SDL_Renderer *renderer);
    int SDL_RenderFillRect(SDL_Renderer *renderer, const SDL_Rect *rect);
    int SDL_RenderPresent(SDL_Renderer *renderer);

    void SDL_Log(const char *fmt, ...);
    void SDL_LogError(int category, const char *fmt, ...);
]]

local SDL = ffi.load("SDL2")

return {
    lib = SDL,
    Event = ffi.typeof("SDL_Event"),
    Rect = ffi.typeof("SDL_Rect"),

    -- Event types
    QUIT = 0x100,
    KEYDOWN = 0x300,
    KEYUP = 0x301,

    -- Key codes
    SCANCODE_ESCAPE = 41,

    -- Initialization flags
    INIT_VIDEO = 0x00000020,

    -- Window position
    WINDOWPOS_CENTERED = 0x2FFF0000,

    -- Window flags
    WINDOW_SHOWN = 0x00000004,

    -- Renderer flags
    RENDERER_ACCELERATED = 0x00000001,
    RENDERER_PRESENTVSYNC = 0x00000004,

    -- Log category
    LOG_CATEGORY_APPLICATION = 0,
}
