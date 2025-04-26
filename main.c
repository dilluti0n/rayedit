#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#ifdef DEBUG
#define MemAlloc malloc
#define MemFree free
#endif

#include "line.h"

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
	Vec_slinep *line_vec;
	size_t cur_row;
	size_t cur_col;
	size_t scroll_row;
};

void eb_init(struct ed_buf **eb) {
	struct ed_buf *neb = MemAlloc(sizeof(struct ed_buf));

	Vec_slinep_init(&neb->line_vec);
	neb->cur_row = neb->cur_col = neb->scroll_row = 0;

	*eb = neb;
}

void eb_insert(struct ed_buf *eb, int ch) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p, %c)\n", __func__, eb, ch);
	printf("eb->cur_row %d\n", eb->cur_row);
#endif
	struct line *line;
	if (Vec_slinep_len(eb->line_vec) <= eb->cur_row ||
	    (line = Vec_slinep_get(eb->line_vec, eb->cur_row)) == NULL) {
		line_init(&line);
		Vec_slinep_insert(eb->line_vec, eb->cur_row, line);
	}
	line_insert(line, eb->cur_col++, ch);
}

void eb_backspace(struct ed_buf *eb) {
	if (eb->cur_col == 0) {	      /* backspace to upper line */
		if (eb->cur_row == 0) /* nothing to remove */
			return;
	size_t upper_line_index = eb->cur_row - 1;
		struct line *upper_line = Vec_slinep_get(eb->line_vec, upper_line_index);
		if (upper_line != NULL)
			line_free(upper_line);
		Vec_slinep_delete(eb->line_vec, upper_line_index);
		eb->cur_row--;
	} else {
		struct line *curr_line = Vec_slinep_get(eb->line_vec, eb->cur_row);
		line_delete(curr_line, --eb->cur_col);
	}

}

void eb_newline(struct ed_buf *eb) {
	assert("TODO - insert newline on middle or end of line" == 0);
	struct line *curr = Vec_slinep_get(eb->line_vec, eb->cur_row);
	if (curr != NULL) {	/* cache the cursor */
		line_set_cursor(curr, eb->cur_col);
	}
	Vec_slinep_insert(eb->line_vec, eb->cur_row++, NULL);
}

void eb_free(struct ed_buf *eb) {
	for (size_t i = 0; i < Vec_slinep_len(eb->line_vec); i++) {
		struct line *curr;
		if ((curr = Vec_slinep_get(eb->line_vec, i)) != NULL)
			MemFree(curr);
	}
	Vec_slinep_free(eb->line_vec);
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
			for (i = 0; i < Vec_slinep_len(eb->line_vec); i++) {
				struct line *line = (eb->line_vec->data[i]);
				char *to_draw;
				if (line != NULL && line->vec != NULL)
					to_draw = line->vec->data;
				else
					to_draw = "\n";
				DrawText(to_draw, 10, 10 + font_size * i,
						 font_size, BLACK);
			}
			EndDrawing();
		}
	}
	CloseWindow();
	eb_free(eb);
}
