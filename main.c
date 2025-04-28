#include <stdbool.h>
#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "editor.h"
#include "line.h"
#include "config.h"

#include <string.h>

#ifndef MAIN_WINDOW_TITLE
#define MAIN_WINDOW_TITLE "window"
#endif

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

int main() {
	SetTraceLogCallback(CustomLogCallback);
	InitWindow(window_size.x, window_size.y, MAIN_WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();

	struct ed_buf *eb;
	eb_init(&eb);

	while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			window_size.x = GetScreenWidth();
			window_size.y = GetScreenHeight();
		}

		{
			int c;
			while ((c = GetCharPressed())) {
				eb_insert(eb, c);
			}
		}

		if (IsKeyPressed(KEY_ENTER)) {
			eb_newline(eb);
		} else if (IsKeyPressed(KEY_BACKSPACE)) {
			eb_backspace(eb);
		} else if (IsKeyPressed(KEY_LEFT)) {
			eb_set_cur_back(eb);
		} else if (IsKeyPressed(KEY_UP)) {
			eb_set_cur_prev(eb);
		} else if (IsKeyPressed(KEY_RIGHT)) {
			eb_set_cur_forward(eb);
		} else if (IsKeyPressed(KEY_DOWN)) {
			eb_set_cur_next(eb);
		}

		{
			int font_size = 20;
			/* int text_width = MeasureText(line->vec->data, font_size); */

			BeginDrawing();
			ClearBackground(RAYWHITE);
			/* DrawText(lineh->vec->data, */
			/*	 (window_size.x - text_width) / 2, 0, */
			/*	 font_size, BLACK); */

#ifdef DEBUG
			printf("[draw]: %lu lines ------\n", eb_get_line_num(eb));
			printf("[draw]: cursor: (%lu, %lu) ------\n", eb_get_cur_col(eb), eb_get_cur_row(eb));
#endif
			size_t i;
			for (i = 0; i <= eb_get_line_num(eb); i++) {
				/* TODO: cannot draw cursor if it is on non_allocated line */
				struct line *line = eb_get_line(eb, i);
				const char *to_draw = (line != NULL)? line_get_string(line) : "\n";
#ifdef DEBUG
				printf("[draw]: line %lu = %p\n", i, line);
#endif
				if (i != eb_get_cur_row(eb)) {
					DrawText(to_draw, 10, 10 + font_size * i,
						 font_size, BLACK);
				} else { /* draw cursor */
					char buf[4096];
					size_t cur_col = eb_get_cur_col(eb);
					strcpy(buf, to_draw);
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
