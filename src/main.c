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
	Vector2 pos;
	bool ldown;
	bool lpressed;
} Mouse;

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

	Mouse mouse;
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

		.mouse = { 0 }
	};
}

void update_mouse(Mouse *mouse) {
	mouse->pos = GetMousePosition();
	mouse->ldown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	mouse->lpressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
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
}

void selection(State *state) {
	if (IsKeyPressed(KEY_LEFT_SHIFT)) {
		state->selecting = true;
	}

	if (IsKeyReleased(KEY_LEFT_SHIFT) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		state->selecting = false;
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		state->selection_updated = false;
	}

	if (state->selecting && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		state->mouse_original = GetMousePosition();
	}

	if (state->selecting && state->dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		// TODO: lwk put mouse_oriignal into mouse

		// world vars
		Vector2 wpos = GetScreenToWorld2D(state->mouse_original, state->camera);
		Vector2 wend = GetScreenToWorld2D(state->mouse.pos, state->camera);

		float wx = fminf(wpos.x, wend.x);
		float wy = fminf(wpos.y, wend.y);
		float ww = fabsf(wpos.x - wend.x);
		float wh = fabsf(wpos.y - wend.y);

		// selection box in world coords to find what user selected
		state->selection = (Rectangle) { wx, wy, ww, wh };

		// screen cords; draw box
		Vector2 spos = state->mouse_original;
		Vector2 send = state->mouse.pos;

		float sx = fminf(spos.x, send.x);
		float sy = fminf(spos.y, send.y);
		float sw = fabsf(spos.x - send.x);
		float sh = fabsf(spos.y - send.y);

		DrawRectangle(sx, sy, sw, sh, BLACK);
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
		update_mouse(&state.mouse);

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
