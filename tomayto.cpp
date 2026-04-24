#include <bits/stdc++.h>
#include <cstdint>
#include <iostream>
#include <raylib.h>

using namespace std;

bool paused = false;

void DrawHoriThick(float x1, float y1, float x2, float y2, Color color, float thickness) {
    DrawRectangle(x1, y1 - (thickness / 2), x2 - x1, thickness, color);
}
void DrawVertThick(float x1, float y1, float x2, float y2, Color color, float thickness) {
    DrawRectangle(x1 - (thickness / 2), y1, thickness, y2 - y1, color);
}

#define GRAPHITE (Color){37, 33, 29, 255}
#define GRAPHBRITE (Color){50, 46, 42, 255}

#define YELLOWGREEN (Color){200, 229, 0, 255}
#define AMBER (Color){255, 180, 0, 255}

// 30px wide
// 56px tall
void DrawSevenSeg(float x1, float y1, int numToDraw, Color segColor) {
    // 5px corners => 30px middle segment?
    int overallWidth = 30; // px
    int lineWidth = 4;     // px
    int cornerGapLen = 5;  // px
    int segmentLen = 20;   // px
    int a = 0x00001000;
    cout << a << endl;

    // top hori
    DrawRectangle(x1 + cornerGapLen, y1, segmentLen, lineWidth, segColor);
    // mid hori
    DrawRectangle(x1 + cornerGapLen, y1 + segmentLen + cornerGapLen + (cornerGapLen - lineWidth),
                  segmentLen, lineWidth, GRAPHBRITE);
    // bottom hori
    DrawRectangle(x1 + cornerGapLen,
                  y1 + segmentLen + (cornerGapLen + 1) + (cornerGapLen - lineWidth) + lineWidth +
                      segmentLen + (cornerGapLen - lineWidth),
                  segmentLen, lineWidth, segColor);

    // top left verti
    DrawRectangle(x1, y1 + cornerGapLen, lineWidth, segmentLen, GRAPHBRITE);
    // bottom left verti
    DrawRectangle(x1, y1 + cornerGapLen + (cornerGapLen + 1) + segmentLen, lineWidth, segmentLen,
                  GRAPHBRITE);
    // top right verti
    DrawRectangle(x1 + cornerGapLen + segmentLen + (cornerGapLen - lineWidth), y1 + cornerGapLen,
                  lineWidth, segmentLen, segColor);
    // bottom right verti
    DrawRectangle(x1 + cornerGapLen + segmentLen + (cornerGapLen - lineWidth),
                  y1 + cornerGapLen + (cornerGapLen + 1) + segmentLen, lineWidth, segmentLen,
                  segColor);
}

// 56px tall
// 164px wide?
void DrawTime(float x, float y) {
    // min
    Color segColor = YELLOWGREEN;
    int padding = 10;
    DrawSevenSeg(x, y, 1, segColor);
    DrawSevenSeg(x + 30 + padding, y, 1, segColor);

    // colon
    int colonX = x + 30 + padding + 30 + padding;
    int colonY = y + 5 + (20.0 / 2) - (4.0 / 2);
    DrawRectangle(colonX, colonY, 4, 4, segColor);
    DrawRectangle(colonX, colonY + 26, 4, 4, segColor);

    // sec
    DrawSevenSeg(colonX + 4 + padding, y, 1, segColor);
    DrawSevenSeg(colonX + 4 + padding + 30 + padding, y, 1, segColor);
}

int main() {
    const int SCREEN_WIDTH = 450;
    const int SCREEN_HEIGHT = 150;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tomayto");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyReleased(' ')) {
            paused = !paused;
            if (paused) {
                SetWindowTitle("Tomayto (paused)");
            } else {
                SetWindowTitle("Tomayto");
            }
        }
        ////////// drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, GRAPHITE);

        // DrawSevenSeg(10, 10, 1);
        // DrawSevenSeg(10 + 30 + 10, 10, 1);
        // DrawSevenSeg(10 + 30 + 10 , 10, 1);
        // DrawSevenSeg(10 + 30 + 10, 10, 1);

        for (int i = 0; i < 4; i++) {
            // DrawSevenSeg(10 + (30 * i) + (10 * i), 10, 1);
        }

        // 164 wide, 30 tall
        DrawTime(((SCREEN_WIDTH / 2.0) - 164) / 2.0, ((SCREEN_HEIGHT / 2.0) - 56) / 2.0);

        int lineWidth = 4; // px
        DrawVertThick((SCREEN_WIDTH / 2.0), 0, (SCREEN_WIDTH / 2.0), SCREEN_HEIGHT, DARKGRAY,
                      lineWidth);
        DrawVertThick(SCREEN_WIDTH * (0.75), SCREEN_HEIGHT / 2, SCREEN_WIDTH * 0.75, SCREEN_HEIGHT,
                      DARKGRAY, lineWidth);
        DrawHoriThick(0, (SCREEN_HEIGHT / 2.0), SCREEN_WIDTH, (SCREEN_HEIGHT / 2.0), DARKGRAY,
                      lineWidth);

        EndDrawing();
    }

    CloseWindow();
}
