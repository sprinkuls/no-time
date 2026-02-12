#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

enum State { work, short_break, long_break };

State state = short_break;

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

  // timing-related vars, in seconds
  float counter = 4;
  float elapsed = 0;

  const float work = 25 * 60;       // 25 minutes, work
  const float short_break = 5 * 60; // 5  minutes, short break
  const float long_break = 15 * 60; // 15 minutes, short break

  // how many phases have passed, to determine when should be work vs short vs
  // long timer should be started
  // we 1 index in this household
  int pomodoro_nr = 1;

  // this is temporary i swear it
  string state = "work";

  Color text_color = BLACK;
  Color bg_color = MAROON;
  bool is_paused = false;

  // want to set up pomodoro first, so:
  // - button to start
  // - button to pause
  // - play sound when time up

  // the loop is:
  // 1.
  // work
  // short
  //
  // 2.
  // work
  // short
  //
  // 3.
  // work
  // short
  //
  // 4.
  // work
  // long
  //
  // GOTO begin and repeat.

  // Main loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    float delta = GetFrameTime(); // just returns delta time in seconds

    if (!is_paused) {
      counter -= delta;
    }

    // prevent from going negative so we can check when the timer has run out
    int ceiled = max(0.0f, ceil(counter));

    // get number of minutes and seconds from the ceiled seconds
    int mins = ceiled / 60;
    int secs = ceiled % 60;

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
    //     if it is, then we set the timer to the amount of time in whatever the
    //     next cycle is (say, 05:00 if work just ended), and set is_paused to
    //     true.
    //
    // play sound

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    if (is_paused) {
      ClearBackground(ColorBrightness(bg_color, -0.1f));
    } else {
      ClearBackground(bg_color);
    }

    if (ceiled > 0) {
      DrawText(TextFormat(time_left.c_str(), ceiled), 190, 200, 20, text_color);
      elapsed += delta;
    } else {
      state = "short_break";
      text_color = WHITE;
      bg_color = SKYBLUE;

      DrawText("time up!", 190, 200, 20, text_color);
      DrawText(TextFormat("elapsed: %f", elapsed), 190, 220, 20, text_color);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
}
