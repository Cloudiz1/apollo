#include "raylib.h"
#include "hcl_arena.h"
#include "hcl_da.h"

#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450 


typedef struct {
	Rectangle rect;
	Color color;
	// TODO: any metadata regarding a set
} Set;

hcl_define_da_alias(Set, set);
hcl_define_da(int);

typedef struct {
	hcl_arena *arena;
	hcl_arena *frame_arena;

	Camera2D camera;
	bool dragging;

	Vector2 mouse_original;
	hcl_da_set *sets;
	hcl_da_int *selected;
	Rectangle selection;
	bool selecting;
	bool selection_updated;

} State;

State init_state() {
	// TODO: eventually store the camera settings somewhere and on startup read those
	// that way, the graph looks the same as you left it
	Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH / 2.0, SCREEN_HEIGHT };
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

	hcl_arena *arena = hcl_arena_new(1024 * 1024 * 1024);
	hcl_arena *frame_arena = hcl_arena_new(1024 * 1024 * 1024);

	hcl_da_set *squares = hcl_da_set_init(arena);
	hcl_da_int *selected = hcl_da_int_init(arena);
	Rectangle selection = { 0 };

	return (State){
		.arena = arena,
		.frame_arena = frame_arena,

		.camera = camera,
		.dragging = false,

		.mouse_original = { 0 },
		.sets = squares,
		.selected = selected,
		.selection = selection,
		.selecting = false,
	};
}

void handle_camera(State *state) {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		state->dragging = true;
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		printf("released: from camera");
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
}

void selection(State *state) {
	if (IsKeyPressed(KEY_LEFT_SHIFT)) {
		state->selecting = true;
	}

	if (IsKeyReleased(KEY_LEFT_SHIFT) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		state->selecting = false;
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		printf("released!");
		state->selection_updated = false;
	}

	if (state->selecting && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		state->mouse_original = GetMousePosition();
	}

	if (state->selecting && state->dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		float ox = state->mouse_original.x;
		float oy = state->mouse_original.y;
		float width = -(state->mouse_original.x - GetMouseX());
		float height = -(state->mouse_original.y - GetMouseY());
		Rectangle rec = (Rectangle) {
			.x = ox,
			.y = oy,
			.width = width,
			.height = height,
		};

		state->selection = rec;
		DrawRectangleRec(rec, BLACK);
	}
}

void update_selection(State *state) {
	for (int i = 0; i < state->sets->length; i++) {
		if (CheckCollisionRecs(state->sets->data[i].rect, state->selection)) {
			printf("collision detected\n");
			hcl_da_int_push(state->selected, i);
			state->sets->data[i].color = RED;
		}
	}
}

void draw_rectangles(State *state) {
	for (int i = 0; i < state->sets->length; i++) {
		Set set = state->sets->data[i];
		DrawRectangleRec(set.rect, set.color);
	}
}

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Apollo");

	State state = init_state();
    SetTargetFPS(60);

	for (int i = 0; i < 100; i++) {
		Rectangle rect = (Rectangle) {
			.x = 0,
			.y = 150 * i,
			.width = 100,
			.height = 100,
		};

		Set set = (Set) {
			.rect = rect,
			.color = BLUE,
		};

		hcl_da_set_push(state.sets, set);
	}

    while (!WindowShouldClose())
	{
		hcl_arena_clear(state.frame_arena);

		handle_camera(&state);
		selection(&state);

		if (!state.selection_updated) {
			update_selection(&state);
			state.selection_updated = true;
		}

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode2D(state.camera);

		draw_rectangles(&state);

		EndMode2D();

		EndDrawing();
    }

    CloseWindow();
	return 0;
}
