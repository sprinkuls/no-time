// call it san marzano?
// cherry tomato? it's supposed to be a small little program

#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

// awesome premature abstraction
struct Style {
    Color fg;
    Color bg;
};

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // const float work_dur = 25 * 60;       // 25 minutes, work
    // const float short_break_dur = 5 * 60; // 5  minutes, short break
    // const float long_break_dur = 15 * 60; // 15 minutes, short break

    // temporary, much shorter values for testing
    const float work_dur = 25;       // 25 minutes, work
    const float short_break_dur = 5; // 5  minutes, short break
    const float long_break_dur = 15; // 15 minutes, short break

    Color text_color = BLACK;

    Color button_color = ORANGE;
    bool is_paused = true;

    // stuff for the one big button
    int button_width = 200;
    int button_height = 80;
    Rectangle button = {screenWidth / 2.0f - button_width / 2.0f,
                        screenHeight / 2.0f - button_height / 2.0f, (float)button_width,
                        (float)button_height};

    // set up for the first cycle, which will be work
    // how many phases have passed, to determine when should be work vs short vs
    // long timer should be started
    // we 1 index in this household
    int pomodoro_nr = 1;

    enum State { work, short_break, long_break };
    State state = work;
    float timer = work_dur;

    // want to set up pomodoro first, so:
    // - button to start
    // - button to pause
    // - play sound when time up

    // the loop is:
    // 1. work, short break
    // 2. work, short break
    // 3. work, short break
    // 4. work, long break
    // and then repeat back at 1.

    // stuff that may change loop to loop
    Color cur_bg_color = RAYWHITE;
    string cur_button_text = "[ start ]";
    Color cur_button_color = button_color;

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float delta = GetFrameTime(); // just returns delta time in seconds
        if (!is_paused) {
            timer -= delta;
        }

        // prevent from going negative so we can check when the timer has run
        // out
        int ceiled_time = max(0.0f, ceil(timer));

        // get number of minutes and seconds from the ceiled seconds
        int mins = ceiled_time / 60;
        int secs = ceiled_time % 60;
        string min_str = to_string(mins);
        string sec_str = to_string(secs);

        // pad with a 0 so both values are always two characters wide
        if (mins < 10) {
            min_str = "0" + min_str;
        }
        if (secs < 10) {
            sec_str = "0" + sec_str;
        }
        string time_left = min_str + ":" + sec_str;

        // Update the state based on time left
        // so it'll be like:
        //
        // we check if timer is zero
        //     if it is, then we set the timer to the amount of time in whatever
        //     the next cycle is (say, 05:00 if work just ended), and set
        //     is_paused to true.
        //
        // play sound

        // awful
        cur_button_color = button_color;
        // set the correct button color based on
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

            // mouse over the button
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                // clicking the button
                cur_button_color = ColorBrightness(cur_button_color, -0.1f);
            } else {
                // just hovering
                cur_button_color = ColorBrightness(cur_button_color, 0.1f);
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                is_paused = !is_paused;
            }

        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        if (ceiled_time == 0) {
            if (state == work) {
                if ((pomodoro_nr % 4) == 0) {
                    state = long_break;
                } else {
                    state = short_break;
                }
            } else {
                state = work;
                pomodoro_nr++;
            }
        }

        // END UPDATE SECTION
        //----------------------------------------------------------------------------------

        // BEGIN DRAWING
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(cur_bg_color);
        DrawRectangleRec(button, cur_button_color);

        int text_width = MeasureText(cur_button_text.c_str(), 20);
        DrawText(cur_button_text.c_str(), button.x, button.y, 20, BLACK);

        // draw button and button text

        if (is_paused) {
            DrawText("[ paused ]", 300, 100, 20, BLACK);
        }
        // setwindowfocused

        // set the right cursor based on if we're hovering the button
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        // if (ceiled_time > 0) {
        //     DrawText(TextFormat(time_left.c_str(), ceiled_time), 190, 200, 20, text_color);
        //     // elapsed += delta;
        // } else {
        //     state = short_break;
        //     text_color = WHITE;
        //     bg_color = SKYBLUE;
        //
        //     DrawText("time up!", 190, 200, 20, text_color);
        //     // DrawText(TextFormat("elapsed: %f", elapsed), 190, 220, 20, text_color);
        //     SetWindowFocused();
        // }

        // stuff that always gets drawn, but that changes based on the state of the timer
        // (colors, exact text, etc.),

        EndDrawing();
        // END DRAWING
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}
