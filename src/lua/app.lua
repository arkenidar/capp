local sdl = require("sdl")

local CELL_SIZE = 40
local FPS = 60

if sdl.init(sdl.INIT_VIDEO) < 0 then
    sdl.log_error(sdl.LOG_CATEGORY_APPLICATION, "SDL_Init failed: " .. sdl.get_error())
    os.exit(1)
end

local dm = sdl.get_desktop_display_mode(0)
if dm == nil then
    sdl.log_error(sdl.LOG_CATEGORY_APPLICATION, "SDL_GetDesktopDisplayMode failed: " .. sdl.get_error())
    sdl.quit()
    os.exit(1)
end

local WINDOW_WIDTH = dm.w
local WINDOW_HEIGHT = dm.h

local window = sdl.create_window(
    "Checkerboard Demo",
    0,
    0,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    sdl.WINDOW_SHOWN
)

if window == nil then
    sdl.log_error(sdl.LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: " .. sdl.get_error())
    sdl.quit()
    os.exit(1)
end

sdl.set_window_bordered(window, sdl.FALSE)
sdl.set_window_position(window, 0, 0)
sdl.set_window_size(window, WINDOW_WIDTH, WINDOW_HEIGHT)

local renderer = sdl.create_renderer(window, -1, sdl.RENDERER_ACCELERATED + sdl.RENDERER_PRESENTVSYNC)

if renderer == nil then
    sdl.log_error(sdl.LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: " .. sdl.get_error())
    sdl.destroy_window(window)
    sdl.quit()
    os.exit(1)
end

local function handle_events()
    local event = sdl.event_new()
    while sdl.poll_event(event) ~= 0 do
        local event_type = sdl.event_type(event)
        if event_type == sdl.QUIT then
            return false
        elseif event_type == sdl.KEYDOWN then
            if sdl.event_key_scancode(event) == sdl.SCANCODE_ESCAPE then
                return false
            end
        end
    end
    return true
end

local function draw_checkerboard(shift)
    for y = 0, WINDOW_HEIGHT - 1, CELL_SIZE do
        for x = 0, WINDOW_WIDTH - 1, CELL_SIZE do
            local pattern_x = math.floor((x + shift) / CELL_SIZE)
            local pattern_y = math.floor(y / CELL_SIZE)
            local is_white = math.fmod(pattern_x + pattern_y, 2) == 0

            if is_white then
                sdl.set_render_draw_color(renderer, 255, 255, 255, 255)
            else
                sdl.set_render_draw_color(renderer, 50, 50, 50, 255)
            end

            local cell = sdl.rect_new(x, y, CELL_SIZE, CELL_SIZE)
            sdl.render_fill_rect(renderer, cell)
        end
    end
end

local function render_frame()
    local ticks = sdl.get_ticks()
    local shift = math.fmod(ticks / 100, 2 * CELL_SIZE)

    sdl.set_render_draw_color(renderer, 0, 0, 0, 255)
    sdl.render_clear(renderer)

    draw_checkerboard(shift)

    sdl.render_present(renderer)
end

local running = true
local frame_time = math.floor(1000 / FPS)

while running do
    local frame_start = sdl.get_ticks()

    running = handle_events()

    render_frame()

    local frame_elapsed = sdl.get_ticks() - frame_start
    if frame_elapsed < frame_time then
        sdl.delay(math.floor(frame_time - frame_elapsed))
    end
end

sdl.destroy_renderer(renderer)
sdl.destroy_window(window)
sdl.quit()
