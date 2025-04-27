#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "line.h"
#include "config.h"

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
	neb->cur_row = 0;
	neb->cur_col = 0;
	neb->scroll_row = 0;

	*eb = neb;
}

/* insert ch to cursor */
void eb_insert(struct ed_buf *eb, int ch) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p, %c)\n", __func__, eb, ch);
	printf("eb->cur_row %lu\n", eb->cur_row);
#endif
	struct line *line;
	if ((line = Vec_slinep_get(eb->line_vec, eb->cur_row)) == NULL) {
		line_init(&line);
		if (Vec_slinep_len(eb->line_vec) == eb->cur_row)
			Vec_slinep_push(eb->line_vec, line);
		else
			Vec_slinep_set(eb->line_vec, eb->cur_row, line);
#ifdef DEBUG
		printf("new line %p allocated\n", line);
#endif
	}
	line_insert(line, eb->cur_col++, ch);
}

void eb_backspace(struct ed_buf *eb) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p)\n", __func__, eb);
	printf("eb->cur_row %lu\n", eb->cur_row);
#endif
	struct line *curr = Vec_slinep_get(eb->line_vec, eb->cur_row);
	if (eb->cur_col == 0) {	      /* backspace to upper line */
		if (eb->cur_row == 0) /* nothing to remove */
			return;

		size_t upper_index = eb->cur_row - 1;
		struct line *upper = Vec_slinep_get(eb->line_vec, upper_index);

		/* cat to upper line and delete curr; if curr is null, cat and free are no needed */
		if (upper != NULL) {
			const size_t upper_last = line_get_last(upper);
			if (curr != NULL && line_get_last(curr) > 0) {
				line_cat(upper, curr);
				line_free(curr);
			}
			Vec_slinep_delete(eb->line_vec, eb->cur_row);
			eb->cur_col = upper_last;
		} else { /* upper line is NULL; just delete it! */
			Vec_slinep_delete(eb->line_vec, upper_index);
			eb->cur_col = 0;
		}

		--eb->cur_row;
	} else {		/* backspace curr line */
		line_delete(curr, --eb->cur_col);
	}

}

void eb_newline(struct ed_buf *eb) {
	struct line *curr_line = Vec_slinep_get(eb->line_vec, eb->cur_row);
	struct line *newline = NULL;

	if (curr_line != NULL && eb->cur_col < line_get_last(curr_line)) {
		line_set_cursor(curr_line, eb->cur_col); /* cache the cursor for upper line */
		line_split(curr_line, eb->cur_col, &newline);
	} else {
		newline = NULL;
	}

	Vec_slinep_insert(eb->line_vec, ++eb->cur_row, newline);
	eb->cur_col = 0;
}

struct line *eb_get_line(struct ed_buf *eb, size_t index) {
	return Vec_slinep_get(eb->line_vec, index);
}

void eb_set_cur_back(struct ed_buf *eb) {
	if (eb->cur_col > 0) {
		--eb->cur_col;
	} else if (eb->cur_row > 0) {
		--eb->cur_row;
		eb->cur_col = line_get_last(eb_get_line(eb, eb->cur_row));
	}
}

void eb_set_cur_prev(struct ed_buf *eb) {
	if (eb->cur_row > 0) {
		struct line *prev = Vec_slinep_get(eb->line_vec, --eb->cur_row);
		eb->cur_col = prev != NULL? line_get_cursor(prev) : 0;
	}
}

size_t eb_get_cur_col(struct ed_buf *eb) {
	return eb->cur_col;
}

size_t eb_get_cur_row(struct ed_buf *eb) {
	return eb->cur_row;
}

size_t eb_get_line_num(struct ed_buf *eb) {
	return Vec_slinep_len(eb->line_vec);
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
		} else if (IsKeyPressed(KEY_LEFT)) {
			eb_set_cur_back(eb);
		} else if (IsKeyPressed(KEY_UP)) {
			eb_set_cur_prev(eb);
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
#endif
			size_t i;
			for (i = 0; i < eb_get_line_num(eb); i++) {
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
