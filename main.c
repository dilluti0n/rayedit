#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "vec.h"

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

struct line {
	VEC *vec;
        size_t cursor;
        size_t last;            /* always pointing first '\0' */
};

void line_init(struct line **lip) {
	struct line *li = MemAlloc(sizeof(struct line));
        vec_init(&(li->vec));
        li->cursor = li->last = 0;
        vec_set(li->vec, li->last, '\0');
	*lip = li;
}

void line_append(struct line *li, char c) {
	if (li->vec->size != 0) {
		vec_set(li->vec, li->last++, c);
        } else {
		vec_push(li->vec, c);
		li->last++;
        }
        vec_push(li->vec, '\0');
}

void line_delete_trailing(struct line *li) {
	vec_set(li->vec, --li->last, '\0');
}

void line_clear(struct line *li) {
	vec_set(li->vec, 0, '\0');
	li->vec->size = li->last = 0;
}

void line_free(struct line *li) {
	vec_free(li->vec);
	MemFree(li);
}

int main() {
	SetTraceLogCallback(CustomLogCallback);
	InitWindow(window_size.x, window_size.y, MAIN_WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();

        struct line *lineh;
        line_init(&lineh);

	while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			window_size.x = GetScreenWidth();
			window_size.y = GetScreenHeight();
		}

		{
			int c;
                        while ((c = GetCharPressed())) {
				line_append(lineh, c);
                        }
                }

                if (IsKeyPressed(KEY_ENTER)) {
			assert(write(STDOUT_FILENO, lineh->vec->data, lineh->last) >= 0 );
			line_clear(lineh);
                } else if (IsKeyPressed(KEY_BACKSPACE) && lineh->last > 0) {
			line_delete_trailing(lineh);
                }

                {
			int font_size = 20;
			int text_width = MeasureText(lineh->vec->data, font_size);

			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText(lineh->vec->data,
				 (window_size.x - text_width) / 2, (window_size.y - font_size) / 2,
				 font_size, LIGHTGRAY);
			EndDrawing();
		}
        }
	line_free(lineh);
	CloseWindow();
}
