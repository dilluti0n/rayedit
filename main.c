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
        CloseWindow();
        lines_free(lines);
}
