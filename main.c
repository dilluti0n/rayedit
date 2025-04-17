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
			printf("%s\n", lineh->vec->data);
			fflush(stdout);
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
