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

  float counter = 10;

  // Main game loop
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

    // yeah this works ok
    counter -= delta;
    int ceiled = ceil(counter);
    if (ceiled >= 0) {
      DrawText(TextFormat("counter: %i", ceiled), 190, 200, 20, BLACK);
    } else {
      DrawText("time up!", 190, 200, 20, BLACK);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
}
