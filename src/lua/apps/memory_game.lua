-- Memory Game
-- Tap/click pairs of matching cards. Grid size and card size adapt to
-- whatever screen this ends up running on (desktop window or Android
-- phone, portrait or landscape), so it stays touch-friendly on small
-- screens instead of just shrinking to fit.

local sdl = require("sdl")

local FPS = 60
local MISMATCH_DELAY_MS = 700

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
    "Memory Game",
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

-- Load fonts (and their texture atlases) as early as possible, before any
-- other setup work. On some Android devices, the window-focus-change that
-- follows shortly after activity start triggers a redraw on Android's own
-- (Java-side) UI thread; if that lands at the same moment as our own first
-- big GPU upload (the font atlas texture), it can race the platform's EGL
-- driver. Racing to get the upload done first avoids the overlap in
-- practice - empirically, deferring it with a delay instead made the crash
-- more consistent, not less.
local FONT_SMALL_SIZE = math.floor(math.min(WINDOW_WIDTH, WINDOW_HEIGHT) * 0.035)
local FONT_BIG_SIZE = math.floor(math.min(WINDOW_WIDTH, WINDOW_HEIGHT) * 0.09)
local HEADER_HEIGHT = FONT_SMALL_SIZE * 3

local FONT_PATH = sdl.get_asset_path("assets/fonts/PressStart2P-Regular.ttf")

local font_small = sdl.load_font(renderer, FONT_PATH, FONT_SMALL_SIZE)
local font_big = sdl.load_font(renderer, FONT_PATH, FONT_BIG_SIZE)

if font_small == nil or font_big == nil then
    sdl.log_error(sdl.LOG_CATEGORY_APPLICATION, "Failed to load font from " .. FONT_PATH)
    sdl.destroy_renderer(renderer)
    sdl.destroy_window(window)
    sdl.quit()
    os.exit(1)
end

math.randomseed(os.time())

--- helpers ------------------------------------------------------------

local function shuffle(t)
    for i = #t, 2, -1 do
        local j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end
end

local function hsv_to_rgb(h, s, v)
    local i = math.floor(h * 6)
    local f = h * 6 - i
    local p = v * (1 - s)
    local q = v * (1 - f * s)
    local t = v * (1 - (1 - f) * s)
    i = i % 6
    local r, g, b
    if i == 0 then r, g, b = v, t, p
    elseif i == 1 then r, g, b = q, v, p
    elseif i == 2 then r, g, b = p, v, t
    elseif i == 3 then r, g, b = p, q, v
    elseif i == 4 then r, g, b = t, p, v
    else r, g, b = v, q, p
    end
    return math.floor(r * 255), math.floor(g * 255), math.floor(b * 255)
end

local function value_color(value, pairs)
    return hsv_to_rgb((value - 1) / pairs, 0.55, 0.85)
end

-- Try grids from most cards (8 pairs) down to fewest (4 pairs), largest
-- layout first, and pick the first whose cell size clears a touch-friendly
-- minimum expressed as a fraction of the screen's short side (a fixed
-- pixel minimum wouldn't mean the same thing on a low-res vs a high-res
-- phone). Falls back to whichever candidate had the biggest cells if none
-- clear the threshold (e.g. a very small window).
local function compute_layout(w, h, header_height)
    local candidates = {
        { pairs = 8, grids = { { 4, 4 }, { 2, 8 }, { 8, 2 } } },
        { pairs = 6, grids = { { 3, 4 }, { 4, 3 }, { 2, 6 }, { 6, 2 } } },
        { pairs = 4, grids = { { 2, 4 }, { 4, 2 } } },
    }
    local margin = math.floor(math.min(w, h) * 0.03)
    local gap = math.floor(margin * 0.6)
    local avail_w = w - margin * 2
    local avail_h = h - margin * 2 - header_height
    local min_cell = math.min(w, h) * 0.11

    local best_pairs, best_rows, best_cols, best_cell = 4, 2, 4, 0
    for _, c in ipairs(candidates) do
        for _, grid in ipairs(c.grids) do
            local rows, cols = grid[1], grid[2]
            local cell_w = (avail_w - gap * (cols - 1)) / cols
            local cell_h = (avail_h - gap * (rows - 1)) / rows
            local cell = math.floor(math.min(cell_w, cell_h))
            if cell > best_cell then
                best_pairs, best_rows, best_cols, best_cell = c.pairs, rows, cols, cell
            end
            if cell >= min_cell then
                best_pairs, best_rows, best_cols, best_cell = c.pairs, rows, cols, cell
                break
            end
        end
        if best_cell >= min_cell then break end
    end

    local grid_w = best_cols * best_cell + gap * (best_cols - 1)
    local grid_h = best_rows * best_cell + gap * (best_rows - 1)
    local grid_x = margin + math.floor((avail_w - grid_w) / 2)
    local grid_y = header_height + margin + math.floor((avail_h - grid_h) / 2)

    local positions = {}
    for r = 0, best_rows - 1 do
        for c = 0, best_cols - 1 do
            positions[#positions + 1] = {
                x = grid_x + c * (best_cell + gap),
                y = grid_y + r * (best_cell + gap),
                w = best_cell,
                h = best_cell,
            }
        end
    end

    return best_pairs, best_cell, positions
end

--- setup -----------------------------------------------------------------

local pairs, cell, positions = compute_layout(WINDOW_WIDTH, WINDOW_HEIGHT, HEADER_HEIGHT)

local cards = {}
local moves, matches, win, revealed, lock_until = 0, 0, false, {}, 0

local function new_game()
    local values = {}
    for v = 1, pairs do
        values[#values + 1] = v
        values[#values + 1] = v
    end
    shuffle(values)

    cards = {}
    for i, pos in ipairs(positions) do
        cards[i] = {
            value = values[i],
            x = pos.x, y = pos.y, w = pos.w, h = pos.h,
            flipped = false, matched = false,
        }
    end
    moves = 0
    matches = 0
    win = false
    revealed = {}
    lock_until = 0
end

new_game()

--- input -------------------------------------------------------------

local function handle_tap(px, py)
    if win then
        new_game()
        return
    end
    if lock_until > 0 then
        return
    end
    for idx, card in ipairs(cards) do
        if not card.matched and not card.flipped and
            px >= card.x and px < card.x + card.w and
            py >= card.y and py < card.y + card.h then
            card.flipped = true
            table.insert(revealed, idx)
            if #revealed == 2 then
                moves = moves + 1
                local a, b = cards[revealed[1]], cards[revealed[2]]
                if a.value == b.value then
                    a.matched = true
                    b.matched = true
                    matches = matches + 1
                    revealed = {}
                    if matches == pairs then
                        win = true
                    end
                else
                    lock_until = sdl.get_ticks() + MISMATCH_DELAY_MS
                end
            end
            return
        end
    end
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
        elseif event_type == sdl.MOUSEBUTTONDOWN then
            local _, x, y = sdl.event_mouse_button(event)
            handle_tap(x, y)
        elseif event_type == sdl.FINGERDOWN then
            local x, y = sdl.event_finger(event, window)
            handle_tap(x, y)
        end
    end
    return true
end

--- rendering -----------------------------------------------------------

-- stb_truetype positions glyphs on their baseline; the binding doesn't
-- expose font ascent/descent, so nudge down from vertical-center by a
-- fraction of the pixel size to approximate a centered baseline.
local BASELINE_NUDGE = 0.35

local function draw_centered_glyph(font, size, text, cx, cy, r, g, b, a)
    local tw = sdl.text_width(font, text)
    local tx = cx - tw / 2
    local ty = cy + size * BASELINE_NUDGE
    sdl.render_text(renderer, font, text, tx, ty, r, g, b, a)
end

local function render_frame()
    if lock_until > 0 and sdl.get_ticks() >= lock_until then
        for _, idx in ipairs(revealed) do
            cards[idx].flipped = false
        end
        revealed = {}
        lock_until = 0
    end

    sdl.set_render_draw_color(renderer, 18, 18, 28, 255)
    sdl.render_clear(renderer)

    local header = string.format("MOVES %d   PAIRS %d/%d", moves, matches, pairs)
    sdl.render_text(renderer, font_small, header, 12, FONT_SMALL_SIZE, 230, 230, 240, 255)
    if win then
        sdl.render_text(renderer, font_small, "YOU WIN - TAP TO PLAY AGAIN",
            12, FONT_SMALL_SIZE * 2 + 4, 120, 220, 140, 255)
    end

    for _, card in ipairs(cards) do
        local rect = sdl.rect_new(card.x, card.y, card.w, card.h)
        local cx, cy = card.x + card.w / 2, card.y + card.h / 2
        if card.matched or card.flipped then
            local r, g, b = value_color(card.value, pairs)
            sdl.set_render_draw_color(renderer, r, g, b, 255)
            sdl.render_fill_rect(renderer, rect)
            draw_centered_glyph(font_big, FONT_BIG_SIZE, tostring(card.value), cx, cy, 20, 20, 24, 255)
        else
            sdl.set_render_draw_color(renderer, 60, 70, 110, 255)
            sdl.render_fill_rect(renderer, rect)
            draw_centered_glyph(font_big, FONT_BIG_SIZE, "?", cx, cy, 220, 225, 240, 255)
        end
    end

    sdl.render_present(renderer)
end

--- main loop ------------------------------------------------------------

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

sdl.destroy_font(font_small)
sdl.destroy_font(font_big)
sdl.destroy_renderer(renderer)
sdl.destroy_window(window)
sdl.quit()
