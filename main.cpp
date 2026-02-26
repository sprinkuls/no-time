#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

const Color OFF_WHITE = {245, 240, 230, 255};
const Color OFF_BLACK = {90, 84, 76, 255};
const Color OFF_GREY = {238, 231, 221, 255};

const Color MAY_ORANGE = {224, 127, 19, 255};
const Color MAY_MAGENTA = {215, 92, 123, 255};
const Color MAY_PURPLE = {166, 109, 211, 255};
const Color MAY_SKYBLUE = {28, 145, 228, 255};

const Color BG_COLOR = OFF_WHITE;
const Color FG_COLOR = OFF_BLACK;
Color ACCENT = MAY_ORANGE; // non-const as the accent color changes with phase

enum Phase { WORK, SHORT_BREAK, LONG_BREAK };

const char *APP_NAME = "Tomayto";
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 380;

const float WORK_DUR = 25 * 60;       // 25 minutes, work
const float SHORT_BREAK_DUR = 5 * 60; // 5  minutes, short break
const float LONG_BREAK_DUR = 15 * 60; // 15 minutes, short break
// short testing values:
// const float work_dur = 61;
// const float short_break_dur = 1;
// const float long_break_dur = 2;

void draw_header(Phase phase, int pomodoro_nr);
void draw_timer(int time_left);

class Button {
  private:
    static vector<Button *> buttons;

    int width;
    int height;

    int x;
    int y;
    Rectangle rect;

    Color body_color;
    Color text_color;

    void update() {
        // derive what color the button should be from base color, as
        // well as from if the button is being hovered / clicked
        Color draw_color;
        if (CheckCollisionPointRec(GetMousePosition(), this->rect)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                draw_color = ColorBrightness(body_color, -0.1f);
            } else {
                draw_color = ColorBrightness(body_color, 0.1f);
            }
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            draw_color = body_color;
        }
        DrawRectangleRec(this->rect, draw_color);
    }

  public:
    static void update_buttons() {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        for (auto btn : buttons) {
            btn->update();
        }
    }

    Button(float x, float y, float width, float height) {
        this->rect = {x, y, width, height};
        buttons.push_back(this);
    };
};

// TODO: this
void notify() {
#if defined(__linux__)
    std::cout << "linux" << "\n";
#elif defined(_WIN32)
    std::cout << "not on linux" << "\n";
#else

#endif
}

// store the amount of working time that's been tracked by tomayto.
// returns the number of SECONDS that have been spent working,
// or -1 on failure of some sort
int update_time_on_disk(int increment = WORK_DUR) {
    fs::path base_path;

#if defined(__linux__)
    // based on: https://specifications.freedesktop.org/basedir/latest/
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (!xdg_data_home || strcmp(xdg_data_home, "") == 0) {

        // $HOME always set on linux, so don't need to check validity
        fs::path path_to_use = string(getenv("HOME"));
        path_to_use = path_to_use / ".local/share";

        if (!fs::is_directory(path_to_use)) {
            return -1; // TODO: handle this?
        }
        base_path = path_to_use;
    } else {
        // non-empty, non-null XDG_DATA_HOME path given
        if (!fs::is_directory(xdg_data_home)) {
            return -1; // TODO: handle this?
        }
        base_path = xdg_data_home;
    }

#elif defined(_WIN32)
    const char *appdata = getenv("APPDATA");
    cout << "appdata path: " << appdata << "\n";
    return -1;
#elif defined(__APPLE__)
    return -1;
#else
    return -1;
#endif
    cout << "Base path for data: " << base_path << "\n";

    // create/find tomayto directory
    fs::path tomayto_path;
    tomayto_path = base_path / "tomayto";
    cout << "Tomayto path: " << tomayto_path << "\n";

    if (!fs::is_directory(tomayto_path)) {
        cout << "Creating directory tomayto at " << base_path << "\n";
        if (!fs::create_directory(tomayto_path)) {
            cout << "ERROR: failed to create directory " << tomayto_path << "\n";
            return -1; // TODO: handle this?
        }
    } else {
        cout << "Found Tomayto data folder at " << tomayto_path << "\n";
    }

    // create/locate data.txt file
    fs::path data_file_path = tomayto_path / "data.txt";
    int seconds = 0;
    if (!fs::exists(data_file_path)) {
        ofstream data_file{data_file_path};
        // data_file << 0 << endl;
        // data_file.close();
        cout << "Created new data file at: " << data_file_path << "\n";
    } else {
        ifstream data_file(data_file_path);
        data_file >> seconds;
        data_file.close();
        cout << "Data file found at: " << data_file_path << "\n";
    }

    // update the file with the new time spent working
    seconds += increment;
    ofstream data_file(data_file_path);
    data_file << seconds << endl;
    data_file.close();

    return seconds;

    // TODO: non-linux
    return -1;
}

int main() {
    // Raylib Initialization -----------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME);
    SetExitKey(KEY_NULL); // prevent ESC from closing the window

    InitAudioDevice();
    Sound changeover_sound = LoadSound("explosion_good.wav");

    int monitor_nr = GetCurrentMonitor();
    int refresh_rate = GetMonitorRefreshRate(monitor_nr);
    cout << monitor_nr << ": " << refresh_rate << endl;
    SetTargetFPS(refresh_rate);
    //----------------------------------------------------------------------------------

    bool is_paused = true;
    const int button_font_size = 50;

    // Setup for the one big button
    int button_width = 300;
    int button_height = 80;
    Rectangle button = {SCREEN_WIDTH / 2.0f - button_width / 2.0f,
                        SCREEN_HEIGHT / 2.0f - button_height / 2.0f, (float)button_width,
                        (float)button_height};
    button.height = 80;
    button.y = (310) - button_height / 2.0f;

    // Initialization for the first cycle, which is work
    int pomodoro_nr = 1;
    Phase phase = WORK;
    float timer = WORK_DUR;

    // Values that change loop to loop
    Color cur_bg_color = BG_COLOR;
    string cur_button_text;
    Color cur_button_color = ACCENT;

    // Main loop
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
            string title = APP_NAME;
            string message = "time for ";

            if (phase == WORK) {
                int seconds_working = update_time_on_disk();
                if (seconds_working != -1)
                    cout << "wow! you've spent " << (int)(seconds_working / 60) << "minutes and "
                         << (int)(seconds_working % 60) << "seconds working!"
                         << "we're all so proud\n";
                if ((pomodoro_nr % 4) == 0) {
                    message += "a long break!";
                    phase = LONG_BREAK;
                    timer = LONG_BREAK_DUR;
                    ACCENT = MAY_PURPLE;
                } else {
                    message += "a break!";
                    phase = SHORT_BREAK;
                    timer = SHORT_BREAK_DUR;
                    ACCENT = MAY_MAGENTA;
                }
            } else {
                message += "work!";
                phase = WORK;
                timer = WORK_DUR;
                ACCENT = MAY_ORANGE;
                pomodoro_nr++;
            }

            // TODO: this is a hack to send a notification on linux, i'm sure there's a better
            // or more general way to do this though
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
    // even if we end the phase early, stil
    if (phase == WORK) {
        int seconds_worked = WORK_DUR - max(0.0f, ceil(timer));
        update_time_on_disk(seconds_worked);
    }

    UnloadSound(changeover_sound);
    CloseAudioDevice();
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
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

    int number_font_size = 100;
    int word_font_size = number_font_size * 0.6;

    // in the default font, each number character (besides 1) is (font_size / 2) px wide, and
    // will have (font_size / 10) px space between them. the '1' character, however, is far
    // slimmer (font_size / 5), and so simply drawing the '1' character as part of a longer
    // string will mess up the nice alignment that all other numbers have.

    // example with font size 100:
    // so, with most number chars, it's:
    // 50px - 10px - 50px
    // number, space between chars, number
    // but if we have a one in the tens place, it needs to be:
    // 30px - 20px - 10px - 50px
    // padding, '1', space between chars, number

    int max_num_width = MeasureText("00", number_font_size);
    int max_word_width = MeasureText("seconds", word_font_size);

    int number_base_x = (SCREEN_WIDTH / 2) - ((max_word_width + max_num_width + 20) / 2);
    int number_base_y = 60;

    // grey transparency (? i'm not sure if i ought to keep this)
    DrawText("00", number_base_x, number_base_y, number_font_size, OFF_GREY);
    DrawText("00", number_base_x, number_base_y + number_font_size, number_font_size, OFF_GREY);

    // minutes
    if (minutes_tens_place == "1") {
        DrawText(minutes_tens_place.c_str(),
                 number_base_x + (number_font_size / 2) - (number_font_size / 5), number_base_y,
                 number_font_size, ACCENT);
    } else if (minutes_tens_place == "0") {
        // don't draw the leading zero
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

    if (minutes == 1) {
        DrawText("minute", (number_base_x + max_num_width) + (20), number_base_y + (33),
                 word_font_size, OFF_BLACK);
    } else {
        DrawText("minutes", (number_base_x + max_num_width) + (20), number_base_y + (33),
                 word_font_size, OFF_BLACK);
    }

    // seconds
    // DrawText(sec_str.c_str(), number_base_x, number_base_y + number_font_size,
    // number_font_size,
    //          ACCENT);
    if (seconds_tens_place == "1") {
        DrawText(seconds_tens_place.c_str(),
                 number_base_x + (number_font_size / 2) - (number_font_size / 5),
                 number_base_y + number_font_size, number_font_size, ACCENT);
    } else if (seconds_tens_place == "0") {
        // don't draw the leading zero
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
    if (seconds == 1) {
        DrawText("second", number_base_x + max_num_width + 20,
                 number_base_y + number_font_size + 33, word_font_size, FG_COLOR);
    } else {
        DrawText("seconds", number_base_x + max_num_width + 20,
                 number_base_y + number_font_size + 33, word_font_size, FG_COLOR);
    }
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

    int text_x = (SCREEN_WIDTH * 0.5) - (text_width / 2.0);
    int text_y = 5;

    Rectangle header = {0, 0, SCREEN_WIDTH, 40};
    DrawRectangleRec(header, ACCENT);
    DrawText(text.c_str(), text_x, text_y, font_size, BG_COLOR);
};

/* TODO List
 * + display the # of minutes/seconds worked somewhere
 *
 * + improve how the data is written/read, specifically, keep track of the
 *   number of seconds worked /in memory/ and read it in when the application starts
 *
 * + get this whole thing compiling on windows, figure out the project structure for that
 *   + really, use a better build system than just "make with one rule".
 *     This is especially true if i want to figure out windows toast notifications
 *
 * + allow skipping forward/back a phase. allow resetting to Pomodoro #1 so you don't have to just
 *   restart the application if you go away from your computer for a while.
 *   + skipping should use song skip mechanics, "back" during a phase puts you at the start of that
 *     phase, back again will put you in the previous phase, but "forward" will send you to the next
 *     phase.
 *
 *   + also, keyboard shortcuts for these. H and L for back / forward
 *
 * + maybe a tray icon?
 *
 * + reminder to start the timer again? quite a few times now i've finished a work period, but,
 * wanting to just finish some particular /thing/ that i'm on, kept working for a minute or two and
 * then forgotten to take the break because the prompt to take one has already ended, i've alreaady
 * heard the sound.
 *
 * + Sound that plays when you hit start/pause, and when the timer ends. Clapboard? It needs to be a
 * /good/ sound when you hit start, and a neutral (not not good, just an indication) one when you
 * pause, if that makes any sense. You want to hear a /satisfying/ thing when you start the timer
 * up.
 *
 * + Proper icon for the application
 *
 */
