#!/usr/bin/env luajit

package.path = package.path .. ";./src/luajit/?.lua"
local SDL2 = require("sdl2")

local SDL = SDL2.lib
local WINDOW_WIDTH = 800
local WINDOW_HEIGHT = 600
local CELL_SIZE = 40
local FPS = 60

local function init_sdl()
    if SDL.SDL_Init(SDL2.INIT_VIDEO) < 0 then
        SDL.SDL_LogError(SDL2.LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL.SDL_GetError())
        os.exit(1)
    end
end

local function create_window()
    local window = SDL.SDL_CreateWindow(
        "Checkerboard Demo",
        SDL2.WINDOWPOS_CENTERED,
        SDL2.WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL2.WINDOW_SHOWN
    )

    if window == nil then
        SDL.SDL_LogError(SDL2.LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL.SDL_GetError())
        SDL.SDL_Quit()
        os.exit(1)
    end

    return window
end

local function create_renderer(window)
    local renderer = SDL.SDL_CreateRenderer(window, -1, SDL2.RENDERER_ACCELERATED + SDL2.RENDERER_PRESENTVSYNC)

    if renderer == nil then
        SDL.SDL_LogError(SDL2.LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL.SDL_GetError())
        SDL.SDL_DestroyWindow(window)
        SDL.SDL_Quit()
        os.exit(1)
    end

    return renderer
end

local function handle_events()
    local event = SDL2.Event()
    while SDL.SDL_PollEvent(event) ~= 0 do
        if event.type == SDL2.QUIT then
            return false
        elseif event.type == SDL2.KEYDOWN then
            if event.key.keysym.scancode == SDL2.SCANCODE_ESCAPE then
                return false
            end
        end
    end
    return true
end

local function draw_checkerboard(renderer, shift)
    for y = 0, WINDOW_HEIGHT - 1, CELL_SIZE do
        for x = 0, WINDOW_WIDTH - 1, CELL_SIZE do
            local pattern_x = math.floor((x + shift) / CELL_SIZE)
            local pattern_y = math.floor(y / CELL_SIZE)
            local is_white = math.fmod(pattern_x + pattern_y, 2) == 0

            if is_white then
                SDL.SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)
            else
                SDL.SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255)
            end

            local cell = SDL2.Rect(x, y, CELL_SIZE, CELL_SIZE)
            SDL.SDL_RenderFillRect(renderer, cell)
        end
    end
end

local function render_frame(renderer)
    local ticks = SDL.SDL_GetTicks()
    local shift = math.fmod(ticks / 100, 2 * CELL_SIZE)

    SDL.SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255)
    SDL.SDL_RenderClear(renderer)

    draw_checkerboard(renderer, shift)

    SDL.SDL_RenderPresent(renderer)
end

local function main()
    init_sdl()
    local window = create_window()
    local renderer = create_renderer(window)

    local running = true
    local frame_time = 1000 / FPS

    while running do
        local frame_start = SDL.SDL_GetTicks()

        running = handle_events()

        render_frame(renderer)

        local frame_elapsed = SDL.SDL_GetTicks() - frame_start
        if frame_elapsed < frame_time then
            SDL.SDL_Delay(frame_time - frame_elapsed)
        end
    end

    SDL.SDL_DestroyRenderer(renderer)
    SDL.SDL_DestroyWindow(window)
    SDL.SDL_Quit()
end

main()
