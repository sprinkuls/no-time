#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

enum Phase { WORK, SHORT_BREAK, LONG_BREAK };

// Constants that are useful everywhere
const int screenWidth = 600;
const int screenHeight = 380;

const Color OFF_WHITE = {245, 240, 230, 255};
const Color OFF_BLACK = {90, 84, 76, 255};
// const Color OFF_GREY = {225, 217, 208, 255};
const Color OFF_GREY = {238, 231, 221, 255};

const Color MAY_MAGENTA = {215, 92, 123, 255};
const Color MAY_ORANGE = {224, 127, 19, 255};
const Color MAY_SKYBLUE = {28, 145, 228, 255};
const Color MAY_PURPLE = {166, 109, 211, 255};
const Color MAY_ROSE = {218, 93, 94, 255};
const Color MAY_CYAN = {0, 166, 176, 255};
const Color MAY_GREEN = {110, 156, 28, 255};

// "linear interpolation"
Color lerp(Color one, Color two, float t);

void draw_header(Phase phase, int pomodoro_nr);
void draw_timer(int time_left);

Color ACCENT = MAY_ORANGE;

int main() {
    // Raylib Initialization -----------------------------------------------------------

    const string application_name = "May's";

    InitWindow(screenWidth, screenHeight, application_name.c_str());
    SetExitKey(KEY_NULL); // prevent ESC from closing the window

    InitAudioDevice();
    Sound changeover_sound = LoadSound("explosion_good.wav");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------------------------------

    // const float work_dur = 25 * 60;       // 25 minutes, work
    // const float short_break_dur = 5 * 60; // 5  minutes, short break
    // const float long_break_dur = 15 * 60; // 15 minutes, short break

    // temporary, much shorter values for development testing
    const float work_dur = 63;
    const float short_break_dur = 1;
    const float long_break_dur = 2;

    const int timer_font_size = 100;
    const int button_font_size = 50;

    // const Color bg_color = {202, 69, 59, 255};
    const Color bg_color = OFF_WHITE;
    Color text_color = OFF_BLACK;
    bool is_paused = true;

    // Setup for the one big button
    int button_width = 300;
    int button_height = 80;
    Rectangle button = {screenWidth / 2.0f - button_width / 2.0f,
                        screenHeight / 2.0f - button_height / 2.0f, (float)button_width,
                        (float)button_height};
    button.height = 80;
    button.y = (310) - button_height / 2.0f;

    // Initialization for the first cycle, which is work
    int pomodoro_nr = 1;
    Phase phase = WORK;
    float timer = work_dur;

    // Values that change loop to loop
    Color cur_bg_color = bg_color;
    string cur_button_text;
    Color cur_button_color = ACCENT;

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
            string title = application_name;
            string message = "time for ";

            if (phase == WORK) {
                if ((pomodoro_nr % 4) == 0) {
                    message += "a long break!";
                    phase = LONG_BREAK;
                    timer = long_break_dur;
                    ACCENT = MAY_PURPLE;
                } else {
                    message += "a break!";
                    phase = SHORT_BREAK;
                    timer = short_break_dur;
                    ACCENT = MAY_MAGENTA;
                }
            } else {
                message += "work!";
                phase = WORK;
                timer = work_dur;
                ACCENT = MAY_ORANGE;
                pomodoro_nr++;
            }

            // TODO: this is a hack to send a notification on linux, i'm sure there's a better or
            // more general way to do this though
            string command = "notify-send \"" + title + "\"" + " " + "\"" + message + "\"";
            system(command.c_str());
        }

        // handle behavior related to the one button. change colors on hover/click
        // set the correct cursor when hovering the button. also handles the behavior related to
        // what happens when the button is pressed.
        cur_button_color = ACCENT;
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                // clicking
                cur_button_color = ColorBrightness(ACCENT, -0.1f);
            } else {
                // hovering
                cur_button_color = ColorBrightness(ACCENT, 0.1f);
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
        draw_header(phase, pomodoro_nr);
        // draw_pomodoro_nr(pomodoro_nr);

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

void draw_timer(int time_left) {
    int minutes = time_left / 60;
    int seconds = time_left % 60;
    string min_str = to_string(minutes);
    string sec_str = to_string(seconds);

    string minutes_tens_place = to_string(minutes / 10);
    string minutes_ones_place = to_string(minutes % 10);

    string seconds_tens_place = to_string(seconds / 10);
    string seconds_ones_place = to_string(seconds % 10);

    Color base = {235, 99, 86, 255};
    Color text_color = {255, 231, 213, 255};

    int number_font_size = 100;
    int word_font_size = number_font_size * 0.6;

    // in the default font, each number character (besides 1) is (font_size / 2) px wide, and will
    // have (font_size / 10) px space between them. the '1' character, however, is far slimmer
    // (font_size / 5), and so simply drawing the '1' character as part of a longer string will mess
    // up the nice alignment that all other numbers have.

    // example with font size 100:
    // so, with most number chars, it's:
    // 50px - 10px - 50px
    // number, space between chars, number
    // but if we have a one in the tens place, it needs to be:
    // 30px - 20px - 10px - 50px
    // padding, '1', space between chars, number

    int zero_width = MeasureText("0", number_font_size);
    cout << "zw: " << zero_width << endl;
    int one_width = MeasureText("1", 200);
    cout << "1w: " << one_width << endl;

    int max_num_width = MeasureText("00", number_font_size);
    cout << "mnw: " << max_num_width << endl;
    int max_word_width = MeasureText("seconds", word_font_size);

    int number_base_x = (screenWidth / 2) - ((max_word_width + max_num_width + 20) / 2);
    int number_base_y = 60;

    // grey transparency (?)
    DrawText("00", number_base_x, number_base_y, number_font_size, OFF_GREY);
    DrawText("00", number_base_x, number_base_y + number_font_size, number_font_size, OFF_GREY);

    // minutes
    if (minutes_tens_place == "1") {
        DrawText(minutes_tens_place.c_str(),
                 number_base_x + (number_font_size / 2) - (number_font_size / 5), number_base_y,
                 number_font_size, ACCENT);
    } else {
        DrawText(minutes_tens_place.c_str(), number_base_x, number_base_y, number_font_size,
                 ACCENT);
    }

    if (minutes_ones_place == "1") {
        DrawText(minutes_ones_place.c_str(),
                 (number_base_x + (number_font_size / 2)) +
                     ((number_font_size / 2) - (number_font_size / 5)) + (number_font_size / 10),
                 number_base_y, number_font_size, ACCENT);
    } else {
        DrawText(minutes_ones_place.c_str(),
                 number_base_x + (number_font_size / 2) + (number_font_size / 10), number_base_y,
                 number_font_size, ACCENT);
    }

    DrawText("minutes", (number_base_x + max_num_width) + (20), number_base_y + (33),
             word_font_size, OFF_BLACK);

    // seconds
    // DrawText(sec_str.c_str(), number_base_x, number_base_y + number_font_size, number_font_size,
    //          ACCENT);
    if (seconds_tens_place == "1") {
        DrawText(seconds_tens_place.c_str(),
                 number_base_x + (number_font_size / 2) - (number_font_size / 5),
                 number_base_y + number_font_size, number_font_size, ACCENT);
    } else {
        DrawText(seconds_tens_place.c_str(), number_base_x, number_base_y + number_font_size,
                 number_font_size, ACCENT);
    }

    if (seconds_ones_place == "1") {
        DrawText(seconds_ones_place.c_str(),
                 (number_base_x + (number_font_size / 2)) +
                     ((number_font_size / 2) - (number_font_size / 5)) + (number_font_size / 10),
                 number_base_y + number_font_size, number_font_size, ACCENT);
    } else {
        DrawText(seconds_ones_place.c_str(),
                 number_base_x + (number_font_size / 2) + (number_font_size / 10),
                 number_base_y + number_font_size, number_font_size, ACCENT);
    }

    DrawText("seconds", number_base_x + max_num_width + 20, number_base_y + number_font_size + 33,
             word_font_size, OFF_BLACK);
}

void draw_header(Phase phase, int pomodoro_nr) {
    string text = "pomodoro #" + to_string(pomodoro_nr) + " - ";

    switch (phase) {
    case WORK:
        text += "work";
        break;
    case SHORT_BREAK:
        text += "short break";
        break;
    case LONG_BREAK:
        text += "long break";
        break;
    }

    int font_size = 30;
    int text_width = MeasureText(text.c_str(), font_size);

    int text_x = (screenWidth * 0.5) - (text_width / 2.0);
    int text_y = 5;

    Rectangle header = {0, 0, screenWidth, 40};
    DrawRectangleRec(header, ACCENT);
    DrawText(text.c_str(), text_x, text_y, font_size, OFF_WHITE);
};

/* TODO List
 * I'd like to store, somewhere, the number of minutes of work done. It'd be nice to have as a
 * little statistic to display.
 *
 * + Sound that plays when you hit start/pause
 *
 * + Fix spacing for timer characters (specifically that all number characters are the same width,
 *   except '1' which is far less wide, and messes up alignment of everything.
 */
