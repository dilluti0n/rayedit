#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "line.h"
#include "lines.h"

#define STR_LEN 255

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

#include "vector.h"

DEFINE_VECTOR(Vec_slinep, struct line *);

struct ed_buf {
	Vec_slinep *line_arr;
	size_t cur_row;
	size_t cur_col;
	size_t scroll_row;
};

void eb_init(struct ed_buf **eb) {
	struct ed_buf *neb = MemAlloc(sizeof(struct ed_buf));

	neb->line_arr = MemAlloc(sizeof(Vec_slinep *));
	Vec_slinep_init(neb->line_arr);
	neb->cur_row = neb->cur_col = neb->scroll_row = 0;

	*eb = neb;
}

void eb_insert(struct ed_buf *eb, int ch) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p, %c)\n", __func__, eb, ch);
	printf("eb->cur_col %d\n", eb->cur_col);
#endif
	struct line *line;
	if (Vec_slinep_len(eb->line_arr) <= eb->cur_col) {
		line_init(&line);
		Vec_slinep_insert(eb->line_arr, eb->cur_col, line);
	}
	line = Vec_slinep_get(eb->line_arr, eb->cur_col);
	line_insert(line, eb->cur_row, ch);
}

void eb_backspace(struct ed_buf *eb) {
	assert("TODO" == 0);
}
void eb_newline(struct ed_buf *eb) {
	assert("TODO" == 0);
}

void eb_free(struct ed_buf *eb) {
	for (size_t i = 0; i < Vec_slinep_len(eb->line_arr); i++) {
		struct line *curr;
		if ((curr = Vec_slinep_get(eb->line_arr, i)) != NULL)
			MemFree(curr);
	}
	MemFree(eb->line_arr);
	MemFree(eb);
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
		}

		{
			int font_size = 20;
			/* int text_width = MeasureText(line->vec->data, font_size); */

			BeginDrawing();
			ClearBackground(RAYWHITE);
			/* DrawText(lineh->vec->data, */
			/*	 (window_size.x - text_width) / 2, 0, */
			/*	 font_size, BLACK); */
			size_t i;
			for (i = 0; i < Vec_slinep_len(eb->line_arr); i++) {
				struct line *p = Vec_slinep_get(eb->line_arr, i);
				DrawText(p->vec->data, 10, 10 + font_size * i, font_size, BLACK);
			}
			EndDrawing();
		}
	}
	CloseWindow();
	eb_free(eb);
}
