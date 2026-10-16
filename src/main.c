#include "raylib.h"

#include <math.h>

int main(void)
{
	const float screenWidth = 800;
	const float screenHeight = 450;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Apollo");

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ screenWidth / 2, screenHeight };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

	bool dragging = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
	{
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			dragging = true;
		}

		if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Vector2 delta = GetMouseDelta();
			camera.target.x -= delta.x / camera.zoom;
			camera.target.y -= delta.y / camera.zoom;
		}

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			dragging = false;
		}

		BeginDrawing();
			float zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
			camera.zoom = fmax(0.05, fmin(zoom, 5));

			ClearBackground(RAYWHITE);

			BeginMode2D(camera);

				for (int i = 0; i < 100; i++) {
					DrawRectangle(i * 150, i * 150, 100, 100, BLUE);
				}

			EndMode2D();

		EndDrawing();
    }

    CloseWindow();
	return 0;
}
