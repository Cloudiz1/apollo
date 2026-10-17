#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450 

typedef struct {
	Camera2D camera;
	Vector2 mouse_original;
	bool dragging;
	bool selecting;
} State;

State init_state() {
	// TODO: eventually store the camera settings somewhere and on startup read those
	// that way, the graph looks the same as you left it
	Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH / 2.0, SCREEN_HEIGHT };
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

	return (State){
		.camera = camera,
		.dragging = false
	};
}

void handle_camera(State *state) {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		state->dragging = true;
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		state->dragging = false;
	}

	// dragging 
	if (state->dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !state->selecting) {
		Vector2 delta = GetMouseDelta();
		state->camera.target.x -= delta.x / state->camera.zoom;
		state->camera.target.y -= delta.y / state->camera.zoom;
	}

	// zoom
	float zoom = expf(logf(state->camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
	state->camera.zoom = fmax(0.05, fmin(zoom, 5));

	if (IsKeyPressed(KEY_LEFT_SHIFT)) {
		state->selecting = true;
	}
}

// TODO: maybe make this return the selected squares too?
void draw_selection(State *state) {
	if (IsKeyReleased(KEY_LEFT_SHIFT) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		state->selecting = false;
	}

	if (state->selecting && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		state->mouse_original = GetMousePosition();
	}

	if (state->selecting && state->dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		float x = state->mouse_original.x;
		float y = state->mouse_original.y;
		float width = -(state->mouse_original.x - GetMouseX());
		float height = -(state->mouse_original.y - GetMouseY());
		DrawRectangle(x, y, width, height, BLACK);
	}
}

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Apollo");

	State state = init_state();
    SetTargetFPS(60);

    while (!WindowShouldClose())
	{
		handle_camera(&state);
		draw_selection(&state);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode2D(state.camera);
		
		for (int i = 0; i < 100; i++) {
			DrawRectangle(i * 150, i * 150, 100, 100, BLUE);
		}

		EndMode2D();

		EndDrawing();
    }

    CloseWindow();
	return 0;
}
