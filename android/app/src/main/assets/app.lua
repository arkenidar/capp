-- capp_lua for Android
-- Checkerboard animation demo (Lua-only, no SDL on Android)
-- Time-based animation synchronized with 60 FPS target

local CELL_SIZE = 20
local FPS = 60
local frame_time_ms = math.floor(1000 / FPS)  -- ~16.67ms per frame

-- Animation state (time-based shift, matches desktop version)
local start_ticks = os.time() * 1000  -- Approximate millisecond timer
local animation_shift = 0

-- Called by Java on each frame with current milliseconds
function update(current_ticks_ms)
    if current_ticks_ms == nil or current_ticks_ms == 0 then
        -- Fallback if timing not provided by Java
        current_ticks_ms = os.time() * 1000
    end

    -- Time-based animation: shift moves based on elapsed time
    -- This matches the desktop version's smooth scrolling
    animation_shift = math.fmod(current_ticks_ms / 100, 2 * CELL_SIZE)

    -- Return shift for rendering (Java will use this)
    return animation_shift
end

-- Return initial state
return {
    version = "1.0",
    cell_size = CELL_SIZE,
    fps = FPS,
    frame_time = frame_time_ms
}
