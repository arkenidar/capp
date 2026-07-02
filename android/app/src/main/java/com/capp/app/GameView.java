package com.capp.app;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.View;

public class GameView extends View {
    private Paint paint;
    private int cellSize = 20;
    private long startTime = 0;
    private boolean isRunning = true;

    public GameView(Context context) {
        super(context);
        paint = new Paint();
        paint.setStyle(Paint.Style.FILL);
        startTime = System.currentTimeMillis();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (!isRunning) return;

        int width = getWidth();
        int height = getHeight();

        // Get animation shift from Lua (time-based)
        long currentTimeMs = System.currentTimeMillis() - startTime;
        MainActivity mainActivity = (MainActivity) getContext();
        double shift = 0;

        if (mainActivity != null) {
            shift = mainActivity.runLuaFrame(currentTimeMs);
        }

        // Draw checkerboard pattern with Lua-calculated animation
        for (int y = 0; y < height; y += cellSize) {
            for (int x = 0; x < width; x += cellSize) {
                // Pattern calculation matches desktop version:
                // shift-based pattern with time-dependent animation
                int patternX = (int) Math.floor((x + shift) / cellSize);
                int patternY = (int) Math.floor(y / cellSize);
                boolean isWhite = (patternX + patternY) % 2 == 0;

                if (isWhite) {
                    paint.setColor(Color.WHITE);
                } else {
                    paint.setColor(Color.rgb(50, 50, 50));  // Dark gray like desktop
                }

                canvas.drawRect(x, y, x + cellSize, y + cellSize, paint);
            }
        }

        // Render at ~60 FPS (16.67ms per frame)
        postInvalidate();
    }

    public void onResume() {
        isRunning = true;
        startTime = System.currentTimeMillis();
        invalidate();
    }

    public void onPause() {
        isRunning = false;
    }

    public void stop() {
        isRunning = false;
    }
}
