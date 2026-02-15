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

const int screenWidth = 800;
const int screenHeight = 450;

// since the included raylib font has numbers that vary in width, (really, only the 1 is less wide
// than the numbers), it's necessary to do all this mess to draw the timer so that, when a 1 appears
// in either the tens place in the seconds, or the ones place in minutes, things don't jump around.

// i could /also/ always not use a standard ticking down timer as the graphic. pomodoro originated
// from a kitchen timer, no? why not make some fun little graphic for that then?
Color lerp(Color one, Color two, float t) {
    if (t > 1)
        return two;
    if (t < 0)
        return one;

    unsigned char r = one.r + (two.r - one.r) * t;
    unsigned char g = one.g + (two.g - one.g) * t;
    unsigned char b = one.b + (two.b - one.b) * t;
    unsigned char a = one.a + (two.a - one.a) * t;
    return {r, g, b, a};
}

void draw_timer(int time_left) {
    // // get number of minutes and seconds from the ceiled seconds
    // int minutes = time_left / 60;
    // int seconds = time_left % 60;
    // string min_str = to_string(minutes);
    // string sec_str = to_string(seconds);
    //
    // // pad with a 0 so both values are always two characters wide
    // if (minutes < 10) {
    //     min_str = "0" + min_str;
    // }
    // if (seconds < 10) {
    //     sec_str = "0" + sec_str;
    // }

    Color base = {235, 99, 86, 255};
    // Color bright =3;

    Color calc = lerp(base, base, 1.0);

    const int base_font_size = 40;
    double sin_time = ((sin(GetTime() * 3)) + 2);
    DrawText("<|:3", 0, 0, (int)(sin_time * base_font_size), base);
}

double last_hit = -1;
void background_pulse(double duration) {
    // say, duration = 3 seconds

    Color bright = {255, 255, 255, 255};
    Color dark = {100, 100, 100, 255};

    double cur_time = GetTime();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        last_hit = cur_time;
    }
    auto time_since = cur_time - last_hit;
    if (time_since > duration) {
        ClearBackground(dark);
    } else {
        auto progression = time_since / duration;
        auto progression_cos = cos((PI / 2) * progression);

        ClearBackground(lerp(dark, bright, progression_cos));
    }
}

int main() {
    // Raylib Initialization -----------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "san marzano");

    InitAudioDevice();
    Sound changeover_sound = LoadSound("explosion_good.wav");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------------------------------

    // const float work_dur = 25 * 60;       // 25 minutes, work
    // const float short_break_dur = 5 * 60; // 5  minutes, short break
    // const float long_break_dur = 15 * 60; // 15 minutes, short break

    // temporary, much shorter values for development testing
    const float work_dur = 11;
    const float short_break_dur = 2;
    const float long_break_dur = 4;

    const int timer_font_size = 50;
    const int button_font_size = 50;

    Color text_color = BLACK;
    Color button_color = ORANGE;
    bool is_paused = true;

    // Setup for the one big button
    int button_width = 200;
    int button_height = 80;
    Rectangle button = {screenWidth / 2.0f - button_width / 2.0f,
                        screenHeight / 2.0f - button_height / 2.0f, (float)button_width,
                        (float)button_height};
    button.height = 200;

    // Setup for the first cycle, which is work
    int pomodoro_nr = 1;
    enum State { work, short_break, long_break };
    State state = work;
    float timer = work_dur;

    // Values that change loop to loop
    Color cur_bg_color = {202, 69, 59, 255};
    string cur_button_text;
    Color cur_button_color = button_color;

    // Main loop
    // TODO: don't close on ESC being hit
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        float delta = GetFrameTime(); // just returns delta time in seconds
        if (!is_paused) {
            timer -= delta;
        }

        int time_left = max(0.0f, ceil(timer));

        // get number of minutes and seconds from the ceiled seconds
        int mins = time_left / 60;
        int secs = time_left % 60;
        string min_str = to_string(mins);
        string sec_str = to_string(secs);

        // pad with a 0 so both values are always two characters wide
        if (mins < 10) {
            min_str = "0" + min_str;
        }
        if (secs < 10) {
            sec_str = "0" + sec_str;
        }
        string timer_display_text = min_str + ":" + sec_str;

        // change states, reset timer
        if (time_left == 0) {
            PlaySound(changeover_sound);
            is_paused = true;
            string title = "May's Computer Program";
            string message = "time for ";

            if (state == work) {
                if ((pomodoro_nr % 4) == 0) {
                    message += "a long break!";
                    state = long_break;
                    timer = long_break_dur;

                } else {
                    message += "a break!";
                    state = short_break;
                    timer = short_break_dur;
                }
            } else {
                message += "work!";
                state = work;
                timer = work_dur;
                pomodoro_nr++;
            }

            string command = "notify-send \"" + title + "\"" + " " + "\"" + message + "\"";
            system(command.c_str());
        }

        // handle behavior related to the one button. change colors on hover/click
        // set the correct cursor when hovering the button. also handles the behavior related to
        // what happens when the button is pressed.
        cur_button_color = button_color;
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
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
        }

        if (IsKeyReleased(' ')) {
            is_paused = !is_paused;
        }
        cur_button_text = (is_paused) ? ("start") : "pause";

        // END UPDATE SECTION
        //----------------------------------------------------------------------------------

        // BEGIN DRAWING
        //----------------------------------------------------------------------------------
        BeginDrawing();
        // ClearBackground(cur_bg_color);
        background_pulse(0.5);

        // BEGIN BUTTON STUFF ---------------------------------------------------------------
        DrawRectangleRec(button, cur_button_color);

        // center the button label within the button
        int button_text_width = MeasureText(cur_button_text.c_str(), button_font_size);

        int btn_center_h = (button.width / 2) + button.x;
        int btn_text_x = btn_center_h - (button_text_width / 2);

        int btn_center_v = (button.height / 2) + button.y;
        int btn_text_y = btn_center_v - (button_font_size / 2);

        DrawText(cur_button_text.c_str(), btn_text_x, btn_text_y, button_font_size, text_color);
        // END BUTTON STUFF
        // ---------------------------------------------------------------------

        // timer itself
        // oh, dude, just draw each character individually.

        // first digit = minutes / 10 (truncated so 0 will work properly
        // second digit = minutes % 10
        // ditto for seconds
        int timer_text_width = MeasureText(timer_display_text.c_str(), timer_font_size);
        DrawText(timer_display_text.c_str(), (screenWidth / 2) - (timer_text_width / 2), 0,
                 timer_font_size, text_color);

        draw_timer(time_left);

        // set the right cursor based on if we're hovering the button
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        EndDrawing();
        // END DRAWING
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadSound(changeover_sound);
    CloseAudioDevice();
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}
