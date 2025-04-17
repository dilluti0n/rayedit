#include <raylib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "line.h"

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

DEFINE_VECTOR(VEC_LINE, struct line *);

struct lines {
	VEC_LINE *arr;
	size_t curr_line;
};

void lines_init(struct lines **p) {
	struct lines *lines = MemAlloc(sizeof(struct lines));
	lines->arr = MemAlloc(sizeof(VEC_LINE));
	VEC_LINE_init(lines->arr);
	lines->curr_line = 0;
	*p = lines;
}

void lines_append_last(struct lines *p, struct line *line) {
	VEC_LINE_push(p->arr, line);
}

void lines_get_last_line(struct lines *p, struct line **last_line) {
#ifdef DEBUG
	assert(last_line != NULL);
#endif
	*last_line = VEC_LINE_get(p->arr, p->arr->size - 1);
}

void lines_free(struct lines *p) {
	for (int i = 0; i < p->arr->size; i++)
		line_free(p->arr->data[i]);
        MemFree(p->arr);
        MemFree(p);
}

int main() {
	SetTraceLogCallback(CustomLogCallback);
	InitWindow(window_size.x, window_size.y, MAIN_WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();

        struct lines *lines;
        struct line *line;
        lines_init(&lines);
	line_init(&line);

	while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			window_size.x = GetScreenWidth();
			window_size.y = GetScreenHeight();
		}

		{
			int c;
			while ((c = GetCharPressed())) {
				line_append(line, c);
			}
		}

		if (IsKeyPressed(KEY_ENTER)) {
			lines_append_last(lines, line);
			line_init(&line);
		} else if (IsKeyPressed(KEY_BACKSPACE) && line->last > 0) {
			line_delete_trailing(line);
		}

		{
			int font_size = 20;
			int text_width = MeasureText(line->vec->data, font_size);

			BeginDrawing();
			ClearBackground(RAYWHITE);
			/* DrawText(lineh->vec->data, */
			/*	 (window_size.x - text_width) / 2, 0, */
                        /*	 font_size, BLACK); */
                        int i;
                        for (i = 0; i < lines->arr->size; i++) {
				struct line *p = VEC_LINE_get(lines->arr, i);
				DrawText(p->vec->data, 10, 10 + font_size * i, font_size, BLACK);
                        }
                        DrawText(line->vec->data, 10, 10 + font_size * i, font_size, BLACK);
			EndDrawing();
		}
	}
	lines_free(lines);
	CloseWindow();
}
