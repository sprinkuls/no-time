#include <bits/stdc++.h>
#include <cstdint>
#include <iostream>
#include <raylib.h>
#include <stdexcept>

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
    int lineWidth = 4;    // px
    int cornerGapLen = 5; // px
    int segmentLen = 20;  // px

    if (numToDraw > 9 || numToDraw < 0) {
        throw invalid_argument("0-9 please boss");
    }

    int bits[10];
    bits[0] = 0b0111111;
    bits[1] = 0b0000110;
    bits[2] = 0b1011011;
    bits[3] = 0b1001111;
    bits[4] = 0b1100110;
    bits[5] = 0b1101101;
    bits[6] = 0b1111101;
    bits[7] = 0b0000111;
    bits[8] = 0b1111111;
    bits[9] = 0b1100111;

    int num = bits[numToDraw];

    Color tmp;

    // top hori
    tmp = ((num & (1 << 0)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1 + cornerGapLen, y1, segmentLen, lineWidth, tmp);
    // mid hori
    tmp = ((num & (1 << 6)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1 + cornerGapLen, y1 + segmentLen + cornerGapLen + (cornerGapLen - lineWidth),
                  segmentLen, lineWidth, tmp);
    // bottom hori
    tmp = ((num & (1 << 3)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1 + cornerGapLen,
                  y1 + segmentLen + (cornerGapLen + 1) + (cornerGapLen - lineWidth) + lineWidth +
                      segmentLen + (cornerGapLen - lineWidth),
                  segmentLen, lineWidth, tmp);

    // top left verti
    tmp = ((num & (1 << 5)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1, y1 + cornerGapLen, lineWidth, segmentLen, tmp);
    // bottom left verti
    tmp = ((num & (1 << 4)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1, y1 + cornerGapLen + (cornerGapLen + 1) + segmentLen, lineWidth, segmentLen,
                  tmp);
    // top right verti
    tmp = ((num & (1 << 1)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1 + cornerGapLen + segmentLen + (cornerGapLen - lineWidth), y1 + cornerGapLen,
                  lineWidth, segmentLen, tmp);
    // bottom right verti
    tmp = ((num & (1 << 2)) != 0) ? segColor : GRAPHBRITE;
    DrawRectangle(x1 + cornerGapLen + segmentLen + (cornerGapLen - lineWidth),
                  y1 + cornerGapLen + (cornerGapLen + 1) + segmentLen, lineWidth, segmentLen, tmp);
}

// 56px tall
// 164px wide?
void DrawTime(float x, float y) {
    // min
    Color segColor = YELLOWGREEN;
    int padding = 10;
    DrawSevenSeg(x, y, 2, segColor);
    DrawSevenSeg(x + 30 + padding, y, 5, segColor);

    // colon
    int colonX = x + 30 + padding + 30 + padding;
    int colonY = y + 5 + (20.0 / 2) - (4.0 / 2);
    DrawRectangle(colonX, colonY, 4, 4, segColor);
    DrawRectangle(colonX, colonY + 26, 4, 4, segColor);

    // sec
    DrawSevenSeg(colonX + 4 + padding, y, 6, segColor);
    DrawSevenSeg(colonX + 4 + padding + 30 + padding, y, 7, segColor);
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
