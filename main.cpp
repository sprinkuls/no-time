#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // timing-related vars, in *seconds*
  float counter = 4;
  float elapsed = 0;

  float work = 25 * 60;       // 25 minutes, work
  float short_break = 5 * 60; // 5  minutes, short break
  float long_break = 15 * 60; // 15 minutes, short break

  // how many phases have passed, to determine when should be work vs short vs
  // long timer should be started
  // we 1 index in this household
  int pomodoro_nr = 1;

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

  cout << "overflow: " << UINT_MAX + 1 << "\n";

  // Main loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    // literally just delta time, given in seconds rather than milliseconds
    float delta = GetFrameTime();
    cout << delta * 60 << "\n";
    // round the difference (really, take the ceil)

    // yeah this works
    counter -= delta;
    int ceiled = ceil(counter);
    if (ceiled > 0) {
      DrawText(TextFormat("counter: %i", ceiled), 190, 200, 20, BLACK);
      elapsed += delta;
    } else {
      DrawText("time up!", 190, 200, 20, BLACK);
      DrawText(TextFormat("elapsed: %f", elapsed), 190, 200, 20, BLACK);
    }

    // want to set up pomodoro first, so:
    // - button to start
    // - button to pause
    // - play sound when time up

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
}
