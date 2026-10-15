#include "raylib.h"

int main(void)
{
 	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800,400, "Apollo");
    SetTargetFPS(60);

	Vector2 pos_shift = {0, 0};
	float scale = 1.0;
	float base = 100;
	bool dragging = false;

    while (!WindowShouldClose())
	{
		float wheelMove = GetMouseWheelMove();
		Vector2 mousePos = GetMousePosition();
		Vector2 mouseDelta = GetMouseDelta();

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			dragging = true;
		}

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			dragging = false;
		}

		if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			pos_shift.x += mouseDelta.x;
			pos_shift.y += mouseDelta.y;
		}

		scale += scale * wheelMove;
		
        BeginDrawing();

			ClearBackground(RAYWHITE);
			float size = base * scale;
			DrawRectangle(100 + pos_shift.x, 100 + pos_shift.y, size, size, BLUE);

        EndDrawing();
    }

    CloseWindow();
	return 0;
}
