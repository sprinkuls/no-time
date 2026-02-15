#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

enum Phase { WORK, SHORT_BREAK, LONG_BREAK };

// Constants that are useful everywhere
const int screenWidth = 600;
const int screenHeight = 400;

const Color OFF_WHITE = {245, 240, 230, 255};
const Color OFF_BLACK = {90, 84, 76, 255};

// const Color MAY_MAGENTA = {189, 71, 116, 255};
// const Color MAY_MAGENTA = {228, 109, 159, 255};
const Color MAY_MAGENTA = {215, 92, 123, 255};
const Color MAY_ORANGE = {224, 127, 19, 255};
const Color MAY_SKYBLUE = {28, 145, 228, 255};
const Color MAY_PURPLE = {119, 126, 232, 255};
const Color MAY_GREEN = {110, 156, 28, 255};

Color lerp(Color one, Color two, float t);
void draw_phase(Phase phase);

Font default_font = GetFontDefault();
void draw_timer(int time_left) {
    // get number of minutes and seconds from the ceiled seconds
    int minutes = time_left / 60;
    int seconds = time_left % 60;
    string min_str = to_string(minutes);
    string sec_str = to_string(seconds);

    // pad with a 0 so both values are always two characters wide
    if (minutes < 10) {
        min_str = "0" + min_str;
    }
    if (seconds < 10) {
        sec_str = "0" + sec_str;
    }

    Color base = {235, 99, 86, 255};
    Color text_color = {255, 231, 213, 255};

    int number_font_size = 100;
    int word_font_size = number_font_size * 0.6;

    int max_num_width = MeasureText("00", number_font_size);
    int max_word_width = MeasureText("seconds", word_font_size);

    int number_base_x = (screenWidth / 2) - ((max_word_width + max_num_width + 20) / 2);
    int number_base_y = 50;

    DrawText(min_str.c_str(), number_base_x, number_base_y, number_font_size, MAY_ORANGE);

    DrawText("minutes", (number_base_x + max_num_width) + (20), number_base_y + (33),
             word_font_size, OFF_BLACK);

    DrawText(sec_str.c_str(), number_base_x, number_base_y + number_font_size, number_font_size,
             MAY_ORANGE);
    DrawText("seconds", number_base_x + max_num_width + 20, number_base_y + number_font_size + 33,
             word_font_size, OFF_BLACK);

    // DrawTextPro(default_font, "hello, world", {50, screenHeight / 2.0}, {0.0, 0.0}, 340, 45, 5,
    //             text_color);

    // // Color bright =3;
    //
    // Color calc = lerp(base, base, 1.0);
    //
    // const int base_font_size = 40;
    // double sin_time = ((sin(GetTime() * 3)) + 2);
    // DrawText("<|:3", 0, 0, (int)(sin_time * base_font_size), base);
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

    const float work_dur = 25 * 60;       // 25 minutes, work
    const float short_break_dur = 5 * 60; // 5  minutes, short break
    const float long_break_dur = 15 * 60; // 15 minutes, short break

    // temporary, much shorter values for development testing
    // const float work_dur = 11;
    // const float short_break_dur = 2;
    // const float long_break_dur = 4;

    const int timer_font_size = 100;
    const int button_font_size = 50;

    // const Color bg_color = {202, 69, 59, 255};
    const Color bg_color = OFF_WHITE;
    Color text_color = OFF_BLACK;
    Color button_color = MAY_ORANGE;
    bool is_paused = true;

    // Setup for the one big button
    int button_width = 200;
    int button_height = 80;
    Rectangle button = {screenWidth / 2.0f - button_width / 2.0f,
                        screenHeight / 2.0f - button_height / 2.0f, (float)button_width,
                        (float)button_height};
    button.height = 80;
    button.y = button.y + 100;
    button.y = (screenHeight * 0.85) - button_height / 2.0f;

    // Setup for the first cycle, which is work
    int pomodoro_nr = 1;
    Phase phase = WORK;
    float timer = work_dur;

    // Values that change loop to loop
    Color cur_bg_color = bg_color;
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

            if (phase == WORK) {
                if ((pomodoro_nr % 4) == 0) {
                    message += "a long break!";
                    phase = LONG_BREAK;
                    timer = long_break_dur;

                } else {
                    message += "a break!";
                    phase = SHORT_BREAK;
                    timer = short_break_dur;
                }
            } else {
                message += "work!";
                phase = WORK;
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
        ClearBackground(cur_bg_color);
        draw_timer(time_left);
        // draw_phase(phase);

        // background_pulse(0.5);

        // BEGIN BUTTON STUFF ---------------------------------------------------------------
        DrawRectangleRec(button, cur_button_color);

        // center the button label within the button
        int button_text_width = MeasureText(cur_button_text.c_str(), button_font_size);

        int btn_center_h = (button.width / 2) + button.x;
        int btn_text_x = btn_center_h - (button_text_width / 2);

        int btn_center_v = (button.height / 2) + button.y;
        int btn_text_y = btn_center_v - (button_font_size / 2);

        DrawText(cur_button_text.c_str(), btn_text_x, btn_text_y, button_font_size, OFF_WHITE);
        // END BUTTON STUFF
        // ---------------------------------------------------------------------

        // timer itself
        // oh, dude, just draw each character individually.

        // first digit = minutes / 10 (truncated so 0 will work properly
        // second digit = minutes % 10
        // ditto for seconds
        // int timer_text_width = MeasureText(timer_display_text.c_str(), timer_font_size);
        // DrawText(timer_display_text.c_str(), (screenWidth / 2) - (timer_text_width / 2), 0,
        //          timer_font_size, text_color);

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

void draw_phase(Phase phase) {
    switch (phase) {
    case WORK:
        DrawText("work..", 0, 0, 40, BLACK);
        break;
    case SHORT_BREAK:
        DrawText("short break!", 0, 0, 40, BLACK);
        break;
    case LONG_BREAK:
        DrawText("long break!", 0, 0, 40, BLACK);
        break;
    }
}
