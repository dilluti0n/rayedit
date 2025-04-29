#include <stdbool.h>
#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "editor.h"
#include "config.h"

#include <string.h>

#ifndef MAIN_WINDOW_TITLE
#define MAIN_WINDOW_TITLE "window"
#endif

#define MIN(a, b) ((a) < (b))? (a) : (b)

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

Vector2 window_size = {.x = INIT_WIDTH, .y = INIT_HEIGHT};

void CustomLogCallback(int logLevel, const char *text, va_list args) {
	FILE *out = stderr;
	fprintf(out, "[%s] ", (logLevel == LOG_INFO) ? "INFO" :
		(logLevel == LOG_WARNING) ? "WARN" :
		(logLevel == LOG_ERROR) ? "ERR" : "DEBUG");

	vfprintf(out, text, args);
	fprintf(out, "\n");
}

static inline void draw_text_slice(int x, int y, int size, Color c,
				   const struct slice sl) {
	char line[4096];
	size_t len;
	if ((len = sl.len) > sizeof (line) - 1)
		len = sizeof(line) - 1;
	memcpy(line, sl.ptr, len);
	line[len] = '\0';
	DrawText(line, x, y, size, c);
}


int main(int argc, char *argv[]) {

	if (argc != 2) {
		return 1;
	}

	SetTraceLogCallback(CustomLogCallback);
	InitWindow(window_size.x, window_size.y, MAIN_WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();

	struct ed_buf *eb;
	eb_init(&eb);
	eb_bind(eb, argv[1]);
	eb_load_file(eb);

	while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			window_size.x = GetScreenWidth();
			window_size.y = GetScreenHeight();
		}

		if (IsKeyPressed(KEY_ENTER)) {
			eb_newline(eb);
		} else if (IsKeyPressed(KEY_BACKSPACE)) {
			eb_backspace(eb);
		} else if (IsKeyPressed(KEY_LEFT)) {
			eb_set_cur_backward(eb);
		} else if (IsKeyPressed(KEY_UP)) {
			eb_set_cur_prev_line(eb);
		} else if (IsKeyPressed(KEY_RIGHT)) {
			eb_set_cur_forward(eb);
		} else if (IsKeyPressed(KEY_DOWN)) {
			eb_set_cur_next_line(eb);
		}

		if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
			if (IsKeyPressed(KEY_S))
				eb_save_file(eb);
		}

		{
			int c;
			while ((c = GetCharPressed())) {
				eb_insert(eb, c);
			}
		}

		{
			int font_size = 20;
			float spacing = 1.0f;
			float font_height = MeasureTextEx(GetFontDefault(), "a", font_size, spacing).y;

			BeginDrawing();
			ClearBackground(RAYWHITE);
			/* DrawText(lineh->vec->data, */
			/*	 (window_size.x - text_width) / 2, 0, */
			/*	 font_size, BLACK); */

			const size_t linenum_to_draw = MIN(eb_get_line_num(eb), window_size.y / font_height);

#ifdef DEBUG
			printf("[draw]: %lu lines ------\n", linenum_to_draw);
			printf("[draw]: cursor: (%lu, %lu) ------\n", eb_get_cur_col(eb), eb_get_cur_row(eb));
#endif

			for (size_t i = 0; i <= linenum_to_draw; i++) {
				/* TODO: cannot draw cursor if it is on non_allocated line */
				struct slice sl = {};
				eb_get_line_slice(eb, i, &sl);
#ifdef DEBUG
				printf("[draw]: line %lu, ptr=%p, len=%lu\n", i, sl.ptr, sl.len);
#endif
				if (i != eb_get_cur_row(eb)) {
					draw_text_slice(10, 10 + font_size * i,
							font_size, BLACK, sl);
				} else { /* draw cursor */
					char buf[4096];
					size_t cur_col = eb_get_cur_col(eb);
					strncpy(buf, sl.ptr, sl.len);
					if (sl.len < 4096)
						buf[sl.len] = '\0';
					if (buf[cur_col] == '\0')
						buf[cur_col + 1] = '\0';
					buf[cur_col] = '_';
					DrawText(buf, 10, 10 + font_size * i,
						 font_size, BLACK);
				}
			}
			EndDrawing();
		}
	}
	CloseWindow();
	eb_free(eb);
}
